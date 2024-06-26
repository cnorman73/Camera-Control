# CMakeLists.txt
# Copyright (c) 2024 Flogistix
# Author: Chris Norman  cnorman@enyuare.com
# This is a CMakeLists.txt file for the TDLAS RGB Camera Payload Application

# These variables allow the user to specify the architecture for building the application
# Usage example: cmake -DBUILD_FOR_X86=ON ..
# with vscode this flag is set in settings.json

option(BUILD_FOR_X86 "Build for x86 architecture" ON)
option(BUILD_FOR_RPI_ON_X86 "Cross-compile for Raspberry Pi on x86" OFF)
option(BUILD_ON_RPI "Build directly on Raspberry Pi" OFF)

if(BUILD_ON_RPI)
    add_definitions(-DRPI_PLATFORM)
elseif(BUILD_FOR_RPI_ON_X86)
    add_definitions(-DRPI_PLATFORM)
endif()

cmake_minimum_required(VERSION 3.12)
project(camera)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add your source files here
set(SOURCES
    src/main.cpp
    src/rgb_camera_manager.cpp
    src/camera_app.cpp
    src/utils.cpp

    src/sony_src/CameraDevice.cpp
    src/sony_src/ConnectionInfo.cpp
    src/sony_src/MessageDefine.cpp
    src/sony_src/PropertyValueTable.cpp
    src/sony_src/Text.cpp
)

# Add your header files here
set(HEADERS
    include/rgb_camera_manager.h
    include/camera_app.h
    include/utils.h
    include/toml.hpp
    include/sony_include/CameraDevice.h
    include/sony_include/ConnectionInfo.h
    include/sony_include/MessageDefine.h
    include/sony_include/PropertyValueTable.h
    include/sony_include/Text.h
)

# Find pkg-config first
find_package(PkgConfig REQUIRED)

# Add the executable target
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Assuming ${CMAKE_CURRENT_SOURCE_DIR} is your project's root directory
set(ldir ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(cr_ldir ${ldir}/crsdk)

if(BUILD_FOR_X86)
    # Use Sony SDK libraries for x86
    # Use the fake pigpio library for x86
    include_directories("${PROJECT_SOURCE_DIR}/lib/sony_sdk/x86/include")
    link_directories("${PROJECT_SOURCE_DIR}/lib/sony_sdk/x86/lib")

elseif(BUILD_FOR_RPI_ON_X86)
    # Use Sony SDK libraries for ARM
    include_directories("${PROJECT_SOURCE_DIR}/lib/sony_sdk/arm/include")
    link_directories("${PROJECT_SOURCE_DIR}/lib/sony_sdk/arm/lib")

elseif(BUILD_ON_RPI)
    # Use Sony SDK libraries for ARM
    include_directories("${PROJECT_SOURCE_DIR}/lib/sony_sdk/arm/include")
    link_directories("${PROJECT_SOURCE_DIR}/lib/sony_sdk/arm/lib")

endif()


# Include the Sony SDK headers and Sony App headers
    target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/CRSDK ${CMAKE_CURRENT_SOURCE_DIR}/include/sony_include ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Find the Sony SDK Cr_Core library within your specified library directory
    find_library(camera_remote Cr_Core HINTS ${cr_ldir})

# link against camera libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE ${camera_remote} gps)

    add_compile_options(-std=c++17)
