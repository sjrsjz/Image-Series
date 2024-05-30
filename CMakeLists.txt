cmake_minimum_required(VERSION 3.5)
set(OpenCV_DIR "F:/NDM/Application/opencv/build")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# build the executable
project( OpenCV )
set( SOURCES main.cpp )
set( CMAKE_CXX_STANDARD 11 )
set( PROJECT_NAME OpenCV )
add_executable( ${PROJECT_NAME} ${SOURCES} )
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS})