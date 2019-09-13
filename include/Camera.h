//
// Created by alex on 13.09.2019.
//

#ifndef JALO_CAMERA_H
#define JALO_CAMERA_H

#include <opencv2/opencv.hpp>
#include <openpose/headers.hpp>
#include "Person.h"

namespace jalo {

auto object_points = std::vector<cv::Point3f>{{0, 0, 0}, {0, 1, 0}, {1, 0, 0}, {1, 1, 0}};

class Camera {
public:
    Camera(int id, const char* source);

    cv::Point2f project(cv::Point3f point_real);
    void project(const std::vector<cv::Point3f>& points_real, std::vector<cv::Point2f>& points_cam);

    cv::Point3f unproject(cv::Point2f point_cam, float z = 0);
    bool unproject(cv::Point2f point_cam, cv::Point3f& point_real, std::vector<cv::Point3f>& edge);

    void calibrate(std::vector<cv::Point2f> cam_points, cv::Point3f shift);

    void capture();
    void detectPeople(float shoulder_height = 1.6);


private:
    cv::Mat R;
    cv::Vec3f r0;
    cv::Mat camera_matrix;
    cv::Mat camera_matrix_inv;
    cv::Mat dist_coeffs;

    cv::VideoCapture cap;
    cv::Mat lastFrame;
    cv::Mat dispFrame;
    std::vector<cv::Point2f> cam_points_calib;

    std::vector<Person> people;

    struct openpose_single {
        op::Wrapper opWrapper;
        openpose_single(): opWrapper(op::ThreadManagerMode::Asynchronous) {
            opWrapper.disableMultiThreading();
            opWrapper.start();
        }
    };
    static op::Wrapper& getOP() {
        static openpose_single openpose;
        return openpose.opWrapper;
    }
};

}


#endif //JALO_CAMERA_H
