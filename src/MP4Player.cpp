#include "MP4Player.h"
#include <iostream>

MP4Player::MP4Player()
    : formatContext(nullptr), videoCodecContext(nullptr), audioCodecContext(nullptr),
      swsContext(nullptr), swrContext(nullptr), videoStreamIndex(-1), audioStreamIndex(-1),
      window(nullptr), renderer(nullptr), texture(nullptr), audioDeviceID(0) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
    }
}

MP4Player::~MP4Player() {
    // Clean up resources
    if (videoCodecContext) {
        avcodec_free_context(&videoCodecContext);
    }
    if (audioCodecContext) {
        avcodec_free_context(&audioCodecContext);
    }
    if (formatContext) {
        avformat_close_input(&formatContext);
    }
    if (swsContext) {
        sws_freeContext(swsContext);
    }
    if (swrContext) {
        swr_free(&swrContext);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (texture) {
        SDL_DestroyTexture(texture);
    }
    if (audioDeviceID > 0) {
        SDL_CloseAudioDevice(audioDeviceID);
    }
    SDL_Quit();
}

bool MP4Player::openFile(const char* filename) {
    if (avformat_open_input(&formatContext, filename, nullptr, nullptr) != 0) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return false;
    }

    if (avformat_find_stream_info(formatContext, nullptr) < 0) {
        std::cerr << "Could not find stream information" << std::endl;
        return false;
    }

    for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
        AVCodecParameters* codecParams = formatContext->streams[i]->codecpar;
        const AVCodec* codec = avcodec_find_decoder(codecParams->codec_id);

        if (codecParams->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            videoCodecContext = avcodec_alloc_context3(codec);
            avcodec_parameters_to_context(videoCodecContext, codecParams);
            if (avcodec_open2(videoCodecContext, codec, nullptr) < 0) {
                std::cerr << "Could not open video codec" << std::endl;
                return false;
            }
        } else if (codecParams->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIndex = i;
            audioCodecContext = avcodec_alloc_context3(codec);
            avcodec_parameters_to_context(audioCodecContext, codecParams);
            if (avcodec_open2(audioCodecContext, codec, nullptr) < 0) {
                std::cerr << "Could not open audio codec" << std::endl;
                return false;
            }
        }
    }

    return true;
}

void MP4Player::play() {
    if (!initializeVideo() || !initializeAudio()) {
        return;
    }

    AVPacket packet;
    av_init_packet(&packet);

    while (av_read_frame(formatContext, &packet) >= 0) {
        if (packet.stream_index == videoStreamIndex) {
            decodeVideoFrame(&packet);
        } else if (packet.stream_index == audioStreamIndex) {
            decodeAudioFrame(&packet);
        }
        av_packet_unref(&packet);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                stop();
                return;
            }
        }
    }
}

void MP4Player::stop() {
    // Stop playback and clean up resources
    // ...
}

bool MP4Player::initializeVideo() {
    int screenWidth = videoCodecContext->width;
    int screenHeight = videoCodecContext->height;

    window = SDL_CreateWindow("MP4 Player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Failed to create SDL renderer: " << SDL_GetError() << std::endl;
        return false;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING,
                                screenWidth, screenHeight);
    if (!texture) {
        std::cerr << "Failed to create SDL texture: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

bool MP4Player::initializeAudio() {
    // Set up SDL audio
    SDL_AudioSpec desiredSpec;
    desiredSpec.freq = audioCodecContext->sample_rate;
    desiredSpec.format = AUDIO_S16SYS;
    desiredSpec.channels = audioCodecContext->channels;
    desiredSpec.silence = 0;
    desiredSpec.samples = 1024;
    desiredSpec.callback = nullptr;
    desiredSpec.userdata = this;

    audioDeviceID = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, &audioSpec, 0);
    if (audioDeviceID == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize the audio resampling context
    swrContext = swr_alloc_set_opts(nullptr,
                                    av_get_default_channel_layout(audioSpec.channels),
                                    AV_SAMPLE_FMT_S16,
                                    audioSpec.freq,
                                    av_get_default_channel_layout(audioCodecContext->channels),
                                    audioCodecContext->sample_fmt,
                                    audioCodecContext->sample_rate,
                                    0,
                                    nullptr);
    if (!swrContext) {
        std::cerr << "Failed to allocate audio resampling context" << std::endl;
        return false;
    }
    if (swr_init(swrContext) < 0) {
        std::cerr << "Failed to initialize audio resampling context" << std::endl;
        return false;
    }

    // Start playing audio
    SDL_PauseAudioDevice(audioDeviceID, 0);

    return true;
}

void MP4Player::decodeVideoFrame(AVPacket* packet) {
    int ret = avcodec_send_packet(videoCodecContext, packet);
    if (ret < 0) {
        std::cerr << "Error sending video packet for decoding" << std::endl;
        return;
    }

    AVFrame* frame = av_frame_alloc();
    while (ret >= 0) {
        ret = avcodec_receive_frame(videoCodecContext, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            std::cerr << "Error during video decoding" << std::endl;
            break;
        }

        // Convert frame to YV12 format
        AVFrame* yv12Frame = av_frame_alloc();
        av_image_alloc(yv12Frame->data, yv12Frame->linesize, videoCodecContext->width, videoCodecContext->height, AV_PIX_FMT_YUV420P, 1);
        swsContext = sws_getContext(videoCodecContext->width, videoCodecContext->height, videoCodecContext->pix_fmt,
                                    videoCodecContext->width, videoCodecContext->height, AV_PIX_FMT_YUV420P,
                                    SWS_BICUBIC, nullptr, nullptr, nullptr);
        sws_scale(swsContext, frame->data, frame->linesize, 0, videoCodecContext->height,
                  yv12Frame->data, yv12Frame->linesize);

        // Render frame using SDL
        SDL_UpdateYUVTexture(texture, nullptr,
                             yv12Frame->data[0], yv12Frame->linesize[0],
                             yv12Frame->data[1], yv12Frame->linesize[1],
                             yv12Frame->data[2], yv12Frame->linesize[2]);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        av_frame_free(&yv12Frame);
        av_frame_unref(frame);
    }
    av_frame_free(&frame);
}

void MP4Player::decodeAudioFrame(AVPacket* packet) {
    int ret = avcodec_send_packet(audioCodecContext, packet);
    if (ret < 0) {
        std::cerr << "Error sending audio packet for decoding" << std::endl;
        return;
    }

    AVFrame* frame = av_frame_alloc();
    while (ret >= 0) {
        ret = avcodec_receive_frame(audioCodecContext, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            std::cerr << "Error during audio decoding" << std::endl;
            break;
        }

        // Convert audio frame to SDL format
        uint8_t* audioBuffer = nullptr;
        int audioBufferSize = 0;
        int bufferSize = av_samples_get_buffer_size(nullptr, audioCodecContext->channels,
                                                    frame->nb_samples, audioCodecContext->sample_fmt, 1);
        audioBuffer = static_cast<uint8_t*>(av_malloc(bufferSize));
        int convertedSamples = swr_convert(swrContext, &audioBuffer, bufferSize,
                                           (const uint8_t**)(frame->data), frame->nb_samples);
        audioBufferSize = convertedSamples * audioCodecContext->channels * sizeof(int16_t);

        // Play audio using SDL
        SDL_QueueAudio(audioDeviceID, audioBuffer, audioBufferSize);
        av_free(audioBuffer);

        av_frame_unref(frame);
    }
    av_frame_free(&frame);
}