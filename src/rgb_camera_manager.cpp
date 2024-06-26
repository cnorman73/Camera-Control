/* rgb_camera_manager.cpp

Copyright (c) Flogistix, LP 2024, All rights reserved.
Author: Chris Norman
        cnorman@enyuare.com

*/

#include <iostream>
#include <regex>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <filesystem>
#include "CRSDK/CameraRemote_SDK.h"
#include "CRSDK/CrError.h"
#include "CameraDevice.h"
#include "ICrCameraObjectInfo.h"
#include "Text.h"
#include "rgb_camera_manager.h"

namespace SDK = SCRSDK;

namespace fs = std::filesystem;

RGBCameraManager::RGBCameraManager() {} // Constructor

RGBCameraManager::~RGBCameraManager() {} // Destructor

// This function is only for debug purposes
void RGBCameraManager::runDemo() {
    // Run a demo that shows features of the camera manager
    std::cout << "RGBCameraManager: Running demo" << std::endl;
    initializeCamera();
    connectCamera("1"); // capture image mode
    std::this_thread::sleep_for(std::chrono::seconds(1));
    captureImage();
    disconnnectCamera();
    connectCamera("2");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (camera->is_connected()) {
        camera->getFoldersFiles();
    }
    else
    {
        cli::tout << "get contents fail because camera disconnected\n";
    }
    disconnnectCamera();

#if 0
    connectCamera("2");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (camera->is_connected()) {
        camera->getFoldersFiles();
    }
    else
    {
        cli::tout << "get contents fail because camera disconnected\n";
    }
    disconnnectCamera();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    connectCamera("1"); // capture image mode
    captureImage();
    disconnnectCamera();
    std::this_thread::sleep_for(std::chrono::seconds(1));
        connectCamera("2");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    if (camera->is_connected()) {
        camera->getFoldersFiles();
    }
    else
    {
        cli::tout << "get contents fail because camera disconnected\n";
    }
    disconnnectCamera();
#endif

#if 0
    connectCamera("1"); // capture image mode
    captureImage();
    disconnnectCamera();
    connectCamera("2"); // download image mode
    camera->getContentsList();
    disconnnectCamera();
    connectCamera("1"); // capture image mode
    disconnnectCamera();
    captureImage();
    disconnnectCamera();
    connectCamera("2"); // download image mode
    camera->getContentsList();
    disconnnectCamera();
#endif
}

int RGBCameraManager::initializeCamera() {
    // Initialize SDK
    bool ret = SCRSDK::Init(0);
    if (!ret) {
        // code to handle the error
        std::cout << "Sony SDK failed to initialize" << std::endl;
        return -1;
    }
    std::cout << "Sony Remote SDK successfully initialized" << std::endl;
    return 0;
}

void RGBCameraManager::connectCamera(cli::text action) {
    cli::tout << "Enumerate connected camera devices...\n";
    SDK::ICrEnumCameraObjectInfo* camera_list = nullptr;
    auto enum_status = SDK::EnumCameraObjects(&camera_list);
    if (CR_FAILED(enum_status) || camera_list == nullptr) {
        cli::tout << "No cameras detected. Connect a camera and retry.\n";
        SDK::Release();
        std::exit(EXIT_FAILURE);
    }
    auto ncams = camera_list->GetCount();
    cli::tout << "Camera enumeration successful. " << ncams << " detected.\n\n";

    for (CrInt32u i = 0; i < ncams; ++i) {
        auto camera_info = camera_list->GetCameraObjectInfo(i);
        cli::text conn_type(camera_info->GetConnectionTypeName());
        cli::text model(camera_info->GetModel());
        cli::text id = TEXT("");
        if (TEXT("IP") == conn_type) {
            cli::NetworkInfo ni = cli::parse_ip_info(camera_info->GetId(), camera_info->GetIdSize());
            id = ni.mac_address;
        }
        else id = ((TCHAR*)camera_info->GetId());
        cli::tout << '[' << i + 1 << "] " << model.data() << " (" << id.data() << ")\n";
    }
#if 0
    cli::tout << std::endl << "Connect to camera with input number...\n";
    cli::tout << "input> ";
    cli::text connectNo;
    std::getline(cli::tin, connectNo);
    cli::tout << '\n';
#endif
    // hard coding the 1 rather than taking input
    cli::text connectNo = "1";
    cli::tsmatch smatch;
    CrInt32u no = 0;
    while (true) {
        no = 0;
        if (std::regex_search(connectNo, smatch, cli::tregex(TEXT("[0-9]")))) {
            no = stoi(connectNo);
            if (no == 0)
                break; // finish

            if (camera_list->GetCount() < no) {
                cli::tout << "input value over \n";
                cli::tout << "input> "; // Redo
                std::getline(cli::tin, connectNo);
                continue;
            }
            else
                break; // ok
        }
        else
            break; // not number
    }

    if (no == 0) {
        cli::tout << "Invalid Number. Finish App.\n";
        SDK::Release();
        std::exit(EXIT_FAILURE);
    }

    std::int32_t cameraNumUniq = 1;
    std::int32_t selectCamera = 1;

    cli::tout << "Connect to selected camera...\n";
    camera_info = camera_list->GetCameraObjectInfo(no - 1);

    cli::tout << "Create camera SDK camera callback object.\n";
    camera = CameraDevicePtr(new cli::CameraDevice(cameraNumUniq, camera_info));
    cameraList.push_back(camera); // add 1st

    cli::tout << "Release enumerated camera list.\n";
    camera_list->Release();

    // line added to Sony code. Hard coding answer
    // cli::text action = TEXT("1");

    if (action == TEXT("1")) { /* connect Remote */
        if (camera->is_connected()) {
            cli::tout << "Please disconnect\n";
        }
        else {
            cli::tout << "Connect Control Mode.\n";
            camera->connect(SDK::CrSdkControlMode_Remote, SDK::CrReconnecting_ON);
        }
    }
    else if (action == TEXT("2")) { /* connect Contents Transfer */
        if (camera->is_connected()) {
            cli::tout << "Please disconnect\n";
        }
        else {
            cli::tout << "Connect Transfer Mode.\n";
            camera->connect(SDK::CrSdkControlMode_ContentsTransfer, SDK::CrReconnecting_ON);
        }
    }

#if 0
    // In this section of code, the ICrEnumCameraObjectInfo object will contain all devices and
    // the ICrCameraObjectInfo object is just one device and we get that device from the ICrEnumCameraObjectInfo object
    // after that object is populated.
    std::cout << "Enumerate connected camera\n";
    // create a pointer to the info object that can be populated by the call to EnumCameraObjects
    SCRSDK::ICrEnumCameraObjectInfo *camera_list = NULL;
    // get the pointer to the info object
    SCRSDK::CrError err = SCRSDK::EnumCameraObjects(&camera_list);
    if (camera_list == NULL) {
        // no cameras found
        // TO-DO: Set a flag in health status that will then be sent back to the user during health check and/or an alert
        std::cout << "No cameras detected\n";
        SCRSDK::Release();
        return;
    }
    // get the number of cameras found
    auto ncams = camera_list->GetCount();
    std::cout << "Camera enumeration successful. " << ncams << " detected.\n";

    auto camera_info = camera_list->GetCameraObjectInfo(0);
    cli::text conn_type(camera_info->GetConnectionTypeName());
    cli::text model(camera_info->GetModel());
    cli::text id = TEXT("");
    //if (TEXT("IP") == conn_type) {
        //cli::NetworkInfo ni = cli::parse_ip_info(camera_info->GetId(), camera_info->GetIdSize());
        //id = ni.mac_address;
    //}
    //else 
    id = ((TCHAR*)camera_info->GetId());
    std::cout << "Camera Model:" << model.data() << std::endl;
    std::cout << "Camera Serial No:" << id.data() << std::endl;

    typedef std::shared_ptr<cli::CameraDevice> CameraDevicePtr;
    std::int32_t cameraNumUniq = 1;

    SCRSDK::CrDeviceHandle hCamDev;
    auto CameraInfoPtr = camera_list->GetCameraObjectInfo(0);
    // release the enumerated camera list

    cli::tout << "Create CameraDevice callback object.\n";
    CameraDevicePtr camera = CameraDevicePtr(new cli::CameraDevice(cameraNumUniq, CameraInfoPtr));
    cli::tout << "Connect to Callback Object.\n";
    camera->connect(SCRSDK::CrSdkControlMode_Remote, SCRSDK::CrReconnecting_ON);
    //err = SCRSDK::Connect(CameraInfoPtr, camera, &hCamDev);

    camera_list->Release();
#endif
}

// Release cameras
void RGBCameraManager::disconnnectCamera() {
    std::cout << "Disconnected Camera\n";
    CameraDeviceList::const_iterator it = cameraList.begin();
    for (std::int32_t j = 0; it != cameraList.end(); ++j, ++it) {
        if ((*it)->is_connected()) {
            cli::tout << "Initiate disconnect sequence.\n";
            auto disconnect_status = (*it)->disconnect();
            if (!disconnect_status) {
                // try again
                disconnect_status = (*it)->disconnect();
            }
            if (!disconnect_status)
                cli::tout << "Disconnect failed to initiate.\n";
            else
                cli::tout << "Disconnect successfully initiated!\n\n";
        }
        (*it)->release();
    }
}

void RGBCameraManager::captureImage() {
    try {
        // Example: camera.setMode(CameraMode::StillImage);
        // Example: camera.setBurstSettings(BurstMode::On, 5);
        
        std::cout << "Starting capture" << std::endl;
        camera->capture_image();
        
        std::cout << "Capture complete" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "An error occurred during image capture: " << e.what() << std::endl;
    }
}

void RGBCameraManager::getCamFileList() {

}

void RGBCameraManager::setupPort(std::string port) {
    this->port = port;
    std::cout << "RGBCameraManager: Port set to " << this->port << std::endl;
}
