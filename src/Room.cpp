//
// Created by alex on 13.09.2019.
//

#include "Room.h"

namespace jalo {

void Room::load_model(std::string filename) {
    auto data = stl::parseSTL(filename);
    model.clear();
    for (auto triangle : data.triangles) {
        std::vector<cv::Point3f> edge;
        model.push_back({{
             cv::Point3f{(float) triangle.v1[0], (float) triangle.v1[1],
                         (float) triangle.v1[2]} / 100,
             cv::Point3f{(float) triangle.v2[0], (float) triangle.v2[1],
                         (float) triangle.v2[2]} / 100,
             cv::Point3f{(float) triangle.v3[0], (float) triangle.v3[1],
                         (float) triangle.v3[2]} / 100,
                         }, 0});
    }
}

Room::Room(std::string model_filename) {
    load_model(model_filename);
}

}