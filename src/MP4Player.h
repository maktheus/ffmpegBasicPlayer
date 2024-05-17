// MP4Player.h
#ifndef MP4PLAYER_H
#define MP4PLAYER_H

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

#include <SDL2/SDL.h>

class MP4Player {
public:
    MP4Player();
    ~MP4Player();

    bool openFile(const char* filename);
    void play();
    void stop();

private:
    AVFormatContext* formatContext;
    AVCodecContext* videoCodecContext;
    AVCodecContext* audioCodecContext;
    SwsContext* swsContext;
    SwrContext* swrContext;
    int videoStreamIndex;
    int audioStreamIndex;

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    SDL_AudioDeviceID audioDeviceID;
    SDL_AudioSpec audioSpec;

    bool initializeVideo();
    bool initializeAudio();
    void decodeVideoFrame(AVPacket* packet);
    void decodeAudioFrame(AVPacket* packet);
};

#endif // MP4PLAYER_H