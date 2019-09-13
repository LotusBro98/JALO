//
// Created by alex on 13.09.2019.
//

#include "Config.h"

#include <opencv2/opencv.hpp>
#include "Camera.h"

int main()
{
    jalo::Camera camera(0, "../videos/Воровство в одном из магазинов.mp4");
    jalo::Room room("../models/1st_Floor.stl");

    camera.show(room);
    cv::waitKey();
}