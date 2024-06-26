/* rgb_camera_manager.h

Author: Chris Norman
        cnorman@enyuare.com

*/
#ifndef RGB_CAMERA_MANAGER_H
#define RGB_CAMERA_MANAGER_H

#include <memory>
#include "CameraDevice.h"
//#include "SonyCameraRemoteSDK.h" // Replace with the actual SDK header

typedef std::shared_ptr<cli::CameraDevice> CameraDevicePtr;
typedef std::vector<CameraDevicePtr> CameraDeviceList;
   
class RGBCameraManager {
public:
    RGBCameraManager(); // Constructor
    ~RGBCameraManager(); // Destructor
    // public member functions
    void runDemo(); // Run a demo that shows features of the camera manager
    void setupPort(std::string port);

private:
    // private member variables
    const SCRSDK::ICrCameraObjectInfo *camera_info;
    CameraDeviceList cameraList; // all
    CameraDevicePtr camera;
    std::string port;
    // private member functions
    int initializeCamera(); // Initialize camera connection
    void connectCamera(cli::text action); // Connect to camera
    void disconnnectCamera(); // Disconnect from camera
    void captureImage(); // Capture an image
    void getCamFileList();

};

#endif // RGB_CAMERA_MANAGER_H
