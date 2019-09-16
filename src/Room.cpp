//
// Created by alex on 13.09.2019.
//

#include <Config.h>
#include <MathUtils.h>
#include "Room.h"

namespace jalo {

Room::Room() {
}

void Room::add_object(std::string name, std::string filename) {
    auto data = stl::parseSTL(filename);
    std::vector<Edge> model;
    for (auto triangle : data.triangles) {
        std::vector<cv::Point3f> edge;
        model.push_back({{
                                 cv::Point3f{(float) triangle.v1[0], (float) triangle.v1[1],
                                             (float) triangle.v1[2]},
                                 cv::Point3f{(float) triangle.v2[0], (float) triangle.v2[1],
                                             (float) triangle.v2[2]},
                                 cv::Point3f{(float) triangle.v3[0], (float) triangle.v3[1],
                                             (float) triangle.v3[2]},
                         }, 0});
    }
    objects[name] = {model, 0};
}

void Room::addCamera(Camera *camera) {
    this->cameras.push_back(camera);
}

void Room::showCameras() {
    bool fill = Config::getBool("draw_fill", false);
    bool wireframe = Config::getBool("draw_wireframe", true);
    bool text = Config::getBool("draw_text", true);
    for (auto cam : cameras)
        cam->show(this, fill, wireframe, text);
}

void Room::capture() {
    for (auto cam : cameras)
    {
        cam->capture();
    }
}

void Room::detectPeople() {
    float shoulder_height = Config::getFloat("shoulder_height", 1.6);
    float person_radius = Config::getFloat("person_radius", 1);
    std::vector<Person> newPeople;
    for (auto cam : cameras)
    {
        cam->detectPeople();
        auto& people = cam->getVisiblePeople();
        for (auto& pers : people)
        {
            bool found = false;
            for (auto& detectedPers : newPeople)
            {

                if (cv::norm(detectedPers.position - pers.position) < person_radius)
                {
                    detectedPers.position = (detectedPers.position * detectedPers.views + pers.position) / (detectedPers.views + 1);
                    detectedPers.views++;
                    found = true;
                    break;
                }
            }
            if (!found)
                newPeople.push_back(pers);
        }
    }
    this->people = newPeople;
}

int Room::getObjectHits(std::string name) {
    return objects[name].hits;
}

void Room::intersectShouldersDirectionWithObjects() {
    float view_distance = Config::getFloat("view_distance", 3);
    for (auto& person : people) {
        Edge* lastHitEdge;
        Model* lastHitModel;
        std::string lastHitModelName;
        bool hit = false;
        cv::Point3f pos = person.position;
        cv::Point3f target = pos + person.shoulders_dir * view_distance;
        for (std::map<std::string, Room::Model>::iterator it = objects.begin(); it != objects.end(); it++) {
            for (auto &edge : it->second.edges) {
                bool hitthis = intersect(edge.points, pos, target);
                if (hitthis)
                {
                    hit = true;
                    lastHitEdge = &edge;
                    lastHitModel = &(it->second);
                    lastHitModelName = it->first;
                }
            }
        }
        if (hit) {
            lastHitEdge->hits++;
            lastHitModel->hits++;
            person.hit = true;
            person.target = target;
            person.target_object = lastHitModelName;
        }
    }
}

std::vector<Person> Room::getPeople() {
    return people;
}

}