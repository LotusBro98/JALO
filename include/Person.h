//
// Created by alex on 13.09.2019.
//

#ifndef JALO_PERSON_H
#define JALO_PERSON_H


#include <opencv2/core/types.hpp>

class Person {
public:
    Person();

    cv::Point3f position;
    cv::Point3f shoulders_dir;
    int views;

    static int lastID;
    int id;

    bool hit;
    cv::Point3f target;
    std::string target_object;

private:

};


#endif //JALO_PERSON_H
