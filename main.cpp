#include <iostream>
#include "MP4Player.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
        return -1;
    }

    const char* filename = argv[1];

    MP4Player player;

    if (!player.openFile(filename)) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return -1;
    }

    std::cout << "Playing " << filename << "..." << std::endl;
    player.play();

    return 0;
}
