# MP4 Player

This project is a simple MP4 player implemented in C++ using the FFmpeg libraries for media decoding and SDL for video rendering and audio playback.

## Prerequisites

Before building and running the MP4 player, make sure you have the following dependencies installed:

- FFmpeg libraries (libavformat, libavcodec, libavutil, libswscale, libswresample)
- SDL2 library

## Building the Project

To build the MP4 player, follow these steps:

1. Clone the repository:

```
git clone https://github.com/your-username/mp4-player.git

```

2. Navigate to the project directory:

```
cd mp4-player

```
3. Build the project using the provided Makefile:
```
make

```




## Running the Player

After building the project, you can run the MP4 player using the following command:

Replace `<path_to_mp4_file>` with the actual path to the MP4 file you want to play.

```
./mp4player <path_to_mp4_file>
```

## Supported Features

The MP4 player supports the following features:

- Opening and playing MP4 files
- Video decoding and rendering using FFmpeg and SDL
- Audio decoding and playback using FFmpeg and SDL
- Basic playback controls (play, stop)

## Code Structure

The project consists of the following files:

- `MP4Player.h`: Header file containing the declaration of the `MP4Player` class.
- `MP4Player.cpp`: Implementation file containing the definition of the `MP4Player` class and its member functions.
- `main.cpp`: Main entry point of the program, responsible for creating an instance of the `MP4Player` class and starting the playback.
- `Makefile`: Makefile for building the project.

## Troubleshooting

If you encounter any issues while building or running the MP4 player, consider the following:

- Make sure you have the required dependencies (FFmpeg and SDL) installed correctly.
- Check that the paths to the FFmpeg and SDL libraries are correctly specified in the Makefile.
- Verify that the MP4 file you are trying to play is valid and supported by FFmpeg.
- If you encounter segmentation faults or other runtime errors, use a debugger (e.g., gdb) to identify the cause of the issue.

## License

This project is licensed under the [MIT License](LICENSE).

## Acknowledgements

This project makes use of the following libraries:

- FFmpeg: https://ffmpeg.org
- SDL: https://www.libsdl.org

## Contributing

Contributions to the MP4 player project are welcome. If you find any bugs or have suggestions for improvements, please open an issue or submit a pull request on the project's GitHub repository.