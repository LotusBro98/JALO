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
    bool points = Config::getBool("draw_points", true);
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

    for (std::map<std::string, Room::Model>::iterator it = objects.begin(); it != objects.end(); it++)
    {
        it->second.hits = 0;
        for (auto& edge : it->second.edges)
            edge.hits = 0;
    }

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
        } else {
            person.hit = false;
            person.target = person.position;
            person.target_object = "";
        }
    }
}

std::vector<Person> Room::getPeople() {
    return people;
}

void Room::show2D() {
    float width_real = Config::getFloat("width_real", 22);
    float height_real = Config::getFloat("height_real", 10);
    float display_scale = Config::getFloat("display_scale", 100);
    cv::Point2f origin = {Config::getFloat("origin_width", 0), Config::getFloat("origin_height", 0)};
    cv::Point2f center{width_real / 2, height_real / 2};
    center += origin;
    bool text = Config::getBool("draw_text", true);

    cv::Mat canvas(height_real * display_scale, width_real * display_scale, CV_8UC3, {0, 0, 0});

    for (std::map<std::string, Room::Model>::iterator it = objects.begin(); it != objects.end(); it++) {
        for (int i = 0; i < it->second.edges.size(); i++) {
            std::vector<cv::Point2f> edge;
            for (auto &pt : it->second.edges[i].points)
                edge.push_back((cv::Point2f{pt.y, pt.x} + center) * display_scale);
            float heat = it->second.hits / 3.0;
            if (heat > 1) heat = 1;
            cv::Scalar color = cv::Scalar(0, 0, 255, 255) * heat + cv::Scalar(255, 0, 0, 255) * (1 - heat);
            cv::Mat edgemat(edge);
            edgemat.convertTo(edgemat, CV_32S);
            cv::fillConvexPoly(canvas, {edgemat}, color);
        }

        if (text) {
            cv::Point3f center_of_mass(0, 0, 0);
            int n = 0;
            for (auto &edge : it->second.edges) {
                for (auto &pt : edge.points) {
                    center_of_mass += pt;
                    n++;
                }
            }
            center_of_mass /= n;
            cv::putText(canvas, it->first, (cv::Point2f{center_of_mass.y, center_of_mass.x} + center) * display_scale, cv::FONT_HERSHEY_SIMPLEX,
                        1,
                        {255, 255, 255});
        }
    }

    for (auto &person : people) {
        cv::Point2f pos = (cv::Point2f{person.position.y, person.position.x} + center) * display_scale;
        cv::Point2f tar = person.hit ? (cv::Point2f{person.target.y, person.target.x} + center) * display_scale : pos + cv::Point2f{person.shoulders_dir.y, person.shoulders_dir.x} * display_scale;
        cv::line(canvas, pos, tar, person.hit ? cv::Scalar{0, 255, 255} : cv::Scalar{0, 255, 0});
        cv::circle(canvas, pos, 4, {0, 0, 255}, -1);
    }

    for (auto cam : cameras)
    {
        float fov = cam->getFOV() / 180.0 * M_PIf32;
        float vfov = cam->getVFOV() / 180.0 * M_PIf32;
        std::vector<std::vector<cv::Point3f>> camBox = {
                {{0,0,0}, {-fov/2, -vfov/2, 1}, {-fov/2, vfov/2, 1}},
                {{0,0,0}, {-fov/2, vfov/2, 1}, {fov/2, vfov/2, 1}},
                {{0,0,0}, {fov/2, vfov/2, 1}, {fov/2, -vfov/2, 1}},
                {{0,0,0}, {fov/2, -vfov/2, 1}, {-fov/2, -vfov/2, 1}},
                {{0,0,0}, {0, -vfov/2, 1}},
        };
        for (auto& face : camBox)
            cam->rotateToReal(face);
        std::vector<std::vector<cv::Point2i>> camBox2d;
        for (auto& face : camBox)
        {
            std::vector<cv::Point2i> face2d;
            for (auto& pt: face)
                face2d.push_back((cv::Point2f{pt.y, pt.x} + center) * display_scale);
            camBox2d.push_back(face2d);
        }
        cv::polylines(canvas, camBox2d, true, {128,255,255});
        cv::putText(canvas, "cam"+std::to_string(cam->getID()), camBox2d[0][0], cv::FONT_HERSHEY_SIMPLEX, 1, {255,255,255});
    }

    cv::imshow("Top View", canvas);
}

}
