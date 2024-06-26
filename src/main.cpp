/* main.cpp

Copyright (c) Flogistix, LP 2024, All rights reserved.
Author: Chris Norman
        cnorman@enyuare.com
*/
#include <iostream>
#include "camera_app.h"

int main(int argc, char* argv[]) {
    std::cout << "Flogistix / Commaris Camera Application" << std::endl;
    // check if 1 argument besides the program name is passed
    if (argc != 2) {
        std::cout << "Usage: camera_app <command>" << std::endl;
        std::cout << "Commands: " << std::endl;
        std::cout << "  snap - snap single frame" << std::endl;
 //       std::cout << "  start - start camera video" << std::endl;
 //       std::cout << "  stop - stop camera video" << std::endl;
        return 1;
    }
    std::string command = argv[1];
    CameraApp camera;
    camera.init();
    camera.run(command);
    return 0;
}
