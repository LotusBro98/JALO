//
// Created by alex on 13.09.2019.
//

#include "Person.h"

void Person::setShouldersDirection(cv::Point3f shoulders_dir) {
    this->shoulders_dir = shoulders_dir;
}

void Person::setPosition(cv::Point3f position) {
    this->position = position;
}

Person::Person() {

}
