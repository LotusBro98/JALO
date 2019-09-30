//
// Created by alex on 13.09.2019.
//

#ifndef JALO_CAMERA_H
#define JALO_CAMERA_H

namespace jalo {
    class Camera;
}

#include <darknet.h>
#include <opencv2/opencv.hpp>
#include <openpose/headers.hpp>
#include "Person.h"
#include "Room.h"

namespace jalo {

class Camera {
public:
    Camera(int id, const char* source);

    cv::Point2f project(cv::Point3f point_real);
    void project(const std::vector<cv::Point3f>& points_real, std::vector<cv::Point2f>& points_cam);
    bool projectLine(cv::Point3f ptr1, cv::Point3f ptr2, cv::Point2f& pt1, cv::Point2f& pt2);

    cv::Point3f unproject(cv::Point2f point_cam, float z = 0);
    bool unproject(cv::Point2f point_cam, cv::Point3f& point_real, std::vector<cv::Point3f>& edge);

    cv::Point3f rotateToReal(cv::Point3f point_cam);
    void rotateToReal(std::vector<cv::Point3f>& points_cam);

    float getFOV();
    float getFOVR();
    float getVFOV();
    float getVFOVR();
    int getID();

    bool isVisible(cv::Point3f point_real, bool check_bounds = true);
    bool isVisible(std::vector<cv::Point3f> points_real, bool check_bounds = true);
    bool isVisible(cv::Point2f point);

    void calibrate(std::vector<cv::Point2f> cam_points, cv::Point3f shift);

    void capture();
    void detectPeople(float shoulder_height = 1.6);
    void show(Room* room, bool fill = true, bool wireframe = true, bool points = true, bool text = true);

    const std::vector<Person>& getVisiblePeople();

private:
    int id;
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

    std::vector<cv::Rect2f> detectPeopleBoxes(cv::Mat& frame, float thresh=0.4, float nms=0.4);

    bool isVisibleReal(cv::Point2f point);

    friend void camera_mouse_callback(int event, int x, int y, int flags, void* userdata);
    void mouse_callback(int event, cv::Point2f point, int flags);

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

    struct yolo_net {
        network *net;
        yolo_net() {
            char * cfgfile = strdup(Config::getString("yolo_cfg", "../3party/darknet/cfg/yolov3-tiny.cfg").data());
            char * weightfile = strdup(Config::getString("yolo_weights", "../3party/darknet/yolov3-tiny.weights").data());
//            gpu_index = -1;
            net = load_network(cfgfile, weightfile, 0);
            free(cfgfile);
            free(weightfile);
        }
    };
    static network* getYOLOnet() {
        static yolo_net yolo;
        return yolo.net;
    }

    bool isLBDown = false;
    Room *room;
    bool fill;
    bool wireframe;
    bool isRBDown = false;
    cv::Vec3f shift;
    cv::Point3f dragStart3D;
    bool text;
    bool points;
};

}


#endif //JALO_CAMERA_H
