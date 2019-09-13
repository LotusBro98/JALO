//
// Created by alex on 13.09.2019.
//

#include <Config.h>
#include <MathUtils.h>
#include "../include/Camera.h"

namespace jalo {

auto object_points = std::vector<cv::Point3f>{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

Camera::Camera(int id, const char* source) {
    this->id = id;
    cap = cv::VideoCapture(source);
    if (!cap.isOpened())
        throw std::runtime_error("Failed to open camera " + std::to_string(id));

    cap >> lastFrame;

    std::string key_R = std::string("cam") + std::to_string(id) + "_R";
    std::string key_r0 = std::string("cam") + std::to_string(id) + "_r0";
    if (!Config::haskey(key_R))
    {
        key_R = "default_R";
        key_r0 = "default_r0";
    }

    R = Config::getMat(key_R);
    r0 = Config::getMat(key_r0);

    float fov = Config::getFloat("fov");
    fov = fov / 180.0f * M_PIf32;
    float camera_matrix_data[] = {lastFrame.cols / fov, 0, lastFrame.cols / 2.0f, 0, lastFrame.cols / fov, lastFrame.rows / 2.0f, 0, 0, 1};
    camera_matrix = cv::Mat(3, 3, CV_32F);
    cv::Mat tocp(3, 3, CV_32F, camera_matrix_data);
    tocp.copyTo(camera_matrix);
    camera_matrix_inv = camera_matrix.inv();

    dist_coeffs = Config::getMat("dist-coeffs");

    project(object_points, cam_points_calib);
}

cv::Point2f Camera::project(cv::Point3f point_real) {
    cv::Point3f point = (cv::Vec3f)(cv::Mat)(R.t()*(cv::Mat)(cv::Vec3f(point_real) - r0));
    point /= point.z;
    return distortPoint({point.x, point.y}, camera_matrix, dist_coeffs);
}

cv::Point3f Camera::unproject(cv::Point2f point_cam, float z) {
    point_cam = undistortPoint(point_cam, camera_matrix, dist_coeffs);

    cv::Vec3f dir = (cv::Mat) (R * (cv::Mat)cv::Vec3f(point_cam.x, point_cam.y, 1));
    float r = (z-r0[2]) / dir[2];
    cv::Vec3f proj = r0 + (r * dir);
    return proj;
}

bool Camera::unproject(cv::Point2f point_cam, cv::Point3f &point_real, std::vector<cv::Point3f> &edge) {
    point_cam = undistortPoint(point_cam, camera_matrix, dist_coeffs);
    cv::Vec3f dir = (cv::Mat) (R * camera_matrix_inv * (cv::Mat)cv::Vec3f(point_cam.x, point_cam.y, 1));
    cv::Point3f target = r0 + (dir * 1000);
    if (intersect(edge, r0, target))
    {
        point_real = target;
        return true;
    } else {
        return false;
    }
}

void Camera::calibrate(std::vector<cv::Point2f> cam_points, cv::Point3f shift) {
    cv::Mat rvecs(cv::Vec3f{0,0,0});
    cv::Mat tvecs(cv::Vec3f{0,0,0});

    cv::solvePnP(object_points, cam_points, camera_matrix, dist_coeffs, rvecs, tvecs, false, cv::SOLVEPNP_P3P);
    r0 = (cv::Mat)(tvecs + cv::Mat(shift));
    cv::Rodrigues(rvecs, R);
    R = R.t();
    r0 = (cv::Mat)(-R * (cv::Mat)r0);
}

void Camera::project(const std::vector<cv::Point3f> &points_real, std::vector<cv::Point2f> &points_cam) {
    points_cam.clear();
    for (auto& pt : points_real)
        points_cam.push_back(project(pt));
}

void Camera::capture() {
    cap >> lastFrame;
}

void Camera::detectPeople(float shoulder_height) {
    people.clear();
    auto datum = getOP().emplaceAndPop(lastFrame);
    auto& keypoints = datum->front()->poseKeypoints;
    for (int i = 0; i < keypoints.getSize(0); i++) {
        cv::Point2f leftShoulder = {keypoints.at({i, 5, 0}), keypoints.at({i, 5, 1})};
        cv::Point2f rightShoulder = {keypoints.at({i, 2, 0}), keypoints.at({i, 2, 1})};

        cv::Point3f ls3D = unproject(leftShoulder, shoulder_height);
        cv::Point3f rs3D = unproject(rightShoulder, shoulder_height);
        cv::Point3f center = 0.5 * (ls3D + rs3D);
        cv::Point3f dir = {-(rs3D.y - ls3D.y), rs3D.x - ls3D.x, 0};
        dir /= cv::norm(dir);

        Person person;
        person.position = center;
        person.shoulders_dir = dir;
        people.push_back(person);
    }
}

void Camera::show(Room &room) {
    lastFrame.copyTo(dispFrame);
    for (int i = 0; i < room.model.size(); i++) {
        std::vector<cv::Point2f> edge;
        project(room.model[i].points, edge);
        std::cout << room.model[i].points << "\n";
        float heat = room.model[i].heat;
        cv::Scalar color = cv::Scalar(0, 0, 255) * heat + cv::Scalar(255, 0, 0) * (1 - heat);
        cv::Mat edgemat(edge);
        edgemat.convertTo(edgemat, CV_32S);
        std::cout << edge << "\n";
        cv::polylines(dispFrame, {edgemat }, true, color);
    }

    std::vector<cv::Scalar> colors = {{255, 255, 255}, {0, 0, 255}, {0, 255, 0}, {255, 0, 0}};
    for (int i = 0; i < cam_points_calib.size(); i++)
    {
        cv::circle(dispFrame, cam_points_calib[i], 3, colors[i], -1);
    }

    for (auto& person : people)
    {
        cv::circle(dispFrame, project(person.position), 2, {0,0,255}, -1);
        cv::circle(dispFrame, project(person.position + person.shoulders_dir * 0.2), 2, {0,255,255}, -1);
    }

    cv::imshow("Cam" + std::to_string(id), dispFrame);
}


}