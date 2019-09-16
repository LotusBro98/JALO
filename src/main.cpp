//
// Created by alex on 13.09.2019.
//

#include "Config.h"

#include <opencv2/opencv.hpp>
#include "Camera.h"

int main()
{
    jalo::Room room;

    room.add_object("walls", "../models/room1.stl");

    room.addCamera(new jalo::Camera(0, "../videos/cam16.mp4"));
    room.addCamera(new jalo::Camera(1, "../videos/cam29.mp4"));
    room.addCamera(new jalo::Camera(2, "../videos/cam31.mp4"));

    room.showCameras();
    room.show2D();
    cv::waitKey();

    while(true)
    {
        room.capture();
        room.detectPeople();
        room.intersectShouldersDirectionWithObjects();
        room.showCameras();
        room.show2D();
        if (cv::waitKey(1) != -1)
            break;
    }
}