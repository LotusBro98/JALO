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

    room.addCamera(new jalo::Camera(0, "../videos/Воровство в одном из магазинов.mp4"));

    room.showCameras();
    cv::waitKey();

    while(true)
    {
        room.capture();
        room.detectPeople();
        room.intersectShouldersDirectionWithObjects();
        room.showCameras();
        if (cv::waitKey(1) != -1)
            break;
    }
}