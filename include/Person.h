//
// Created by alex on 13.09.2019.
//

#ifndef JALO_PERSON_H
#define JALO_PERSON_H


#include <opencv2/core/types.hpp>

class Person {
public:
    Person();

    void setPosition(cv::Point3f position);
    void setShouldersDirection(cv::Point3f shoulders_dir);

private:
    cv::Point3f position;
    cv::Point3f shoulders_dir;
    cv::Point3f target;
};


#endif //JALO_PERSON_H
