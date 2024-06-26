/* payload_app.cpp

Copyright (c) Flogistix, LP 2024, All rights reserved.
Author: Chris Norman
        cnorman@enyuare.com
Date: 2024-02-01

*/


#include <iostream>
#include <filesystem>
#include <unistd.h>
#include "camera_app.h"
#include "toml.hpp"

#define DEBUG_TOML

namespace fs = std::filesystem;

CameraApp::CameraApp() {
}

int CameraApp::init() {

    // Other variables
    std::string rgb_camera_device_port; 

    try {        
        std::string configPath = getConfigFilePath();
        auto config = toml::parse_file(configPath);

        // Using TOML++ library to accessing and setting variables
        std::string rgb_camera_device_port = config["RGBCamera"]["device_port"].value_or(""); 

        #ifdef DEBUG_TOML
        std::cout << "RGB Camera Port: " << rgb_camera_device_port << std::endl;
        #endif
    } catch (const toml::parse_error& err) {
        std::cerr << "Payload App: Toml parsing failed:\n" << err << "\n";
        return 1;
    } catch (const std::exception& err) {
        std::cerr << "Payload App: Error " << err.what() << "\n";
        return 1;
    }
    camera.setupPort(rgb_camera_device_port);
    std::cout << "PayloadApp: Initialized" << std::endl;
    return 0;
}

int CameraApp::run(const std::string &command) {
    std::cout << "PayloadApp: Running" << std::endl;
    if (command=="snap") {
        //camera.captureImage();
        camera.runDemo();
    } else {
        std::cout << "PayloadApp: Unknown command" << std::endl;
    }
    return 0;
}

std::string CameraApp::getConfigFilePath() {
    // Obtain the path to the current executable
    fs::path execPath = fs::current_path() / fs::read_symlink("/proc/self/exe");

    // Get the directory containing the executable
    fs::path execDir = execPath.parent_path();

    // Construct the path to the configuration file relative to the executable's directory
    fs::path configFilePath = execDir / "../camera_config.toml";

    #ifdef DEBUG_TOML
    std::cout << "CameraApp: configFilePath " << configFilePath.string() << std::endl;
    #endif
    return configFilePath.string();
}


