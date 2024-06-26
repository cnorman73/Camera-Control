/* payload_app.h

Author: Chris Norman
        cnorman@enyuare.com
Date: 2024-06-26

*/ 

#ifndef CAMERA_APP_H
#define CAMERA_APP_H

#include "rgb_camera_manager.h"

class CameraApp {
public:
    CameraApp();
    int init();
    int run(const std::string &command);

private:
    // Remember to update this compiled-in version number
    std::string version = "{\"version\": \"0.1.0\"}";
    RGBCameraManager camera;
    std::string RGBCamera_port;
    std::string getConfigFilePath();
};

#endif // CAMERA_APP_H
