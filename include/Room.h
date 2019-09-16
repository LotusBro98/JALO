//
// Created by alex on 13.09.2019.
//

#ifndef JALO_ROOM_H
#define JALO_ROOM_H

namespace jalo {
    class Room;
}

#include "stlParser.h"
#include "Camera.h"
#include <opencv2/opencv.hpp>

namespace jalo {

class Room {
public:
    struct Edge {
        std::vector<cv::Point3f> points;
        int hits;
    };

    struct Model {
        std::vector<Edge> edges;
        int hits;
    };

    explicit Room();

    void add_object(std::string name, std::string filename);
    void addCamera(Camera* camera);
    void showCameras();

    void capture();
    void detectPeople();
    void intersectShouldersDirectionWithObjects();

    int getObjectHits(std::string name);
    std::vector<Person> getPeople();

    void show2D();

    std::map<std::string, Model> objects;

private:
    std::vector<Camera*> cameras;
    std::vector<Person> people;
};

}


#endif //JALO_ROOM_H
