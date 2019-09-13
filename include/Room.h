//
// Created by alex on 13.09.2019.
//

#ifndef JALO_ROOM_H
#define JALO_ROOM_H

#include "stlParser.h"
#include <opencv2/opencv.hpp>

class Room {
public:
    Room(std::string model_filename);

    void load_model(std::string filename);

private:
    std::vector<std::vector<cv::Point3f>> model;
};


#endif //JALO_ROOM_H
