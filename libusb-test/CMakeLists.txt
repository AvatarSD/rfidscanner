cmake_minimum_required(VERSION 2.8)

project(libusb-test)

add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} usb-1.0 pthread)
#add_definitions( -std=c++1y )


