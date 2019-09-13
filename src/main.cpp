//
// Created by alex on 13.09.2019.
//

#include "Config.h"

#include <opencv2/opencv.hpp>

int main()
{
    float Rinit[] = {-0.66325486, 0.40076259, -0.63204616, 0.74838018, 0.36022136, -0.55692697, 0.0044810306, -0.84239537, -0.53884137};
    cv::Mat R(3, 3, CV_32F, Rinit);

    std::cout << jalo::Config::getMat("R") << "\n";

    jalo::Config::setMat("R", R);
    jalo::Config::save();
}