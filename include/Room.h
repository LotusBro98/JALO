//
// Created by alex on 13.09.2019.
//

#ifndef JALO_ROOM_H
#define JALO_ROOM_H

#include "stlParser.h"
#include <opencv2/opencv.hpp>

namespace jalo {

class Room {
    struct Edge {
        std::vector<cv::Point3f> points;
        float heat;
    };

public:
    explicit Room(std::string model_filename);

    void load_model(std::string filename);

    std::vector<Edge> model;

private:

};

}


#endif //JALO_ROOM_H
