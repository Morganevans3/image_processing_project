cmake_minimum_required(VERSION 3.24)
project(ImageProcessing_Spring_2023_2)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(ImageProcessing_Spring_2023_2
        cmake-build-debug/input/car.tga
        cmake-build-debug/input/circles.tga
        cmake-build-debug/input/layer1.tga
        cmake-build-debug/input/layer2.tga
        cmake-build-debug/input/layer_blue.tga
        cmake-build-debug/input/layer_green.tga
        cmake-build-debug/input/layer_red.tga
        cmake-build-debug/input/pattern1.tga
        cmake-build-debug/input/pattern2.tga
        cmake-build-debug/input/text.tga
        cmake-build-debug/input/text2.tga
        src/main.cpp)
