//
// Created by alex on 13.09.2019.
//

#include <Config.h>
#include <MathUtils.h>
#include <map>
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
    std::string key_r1 = std::string("cam") + std::to_string(id) + "_r1";
    if (!Config::haskey(key_R))
    {
        key_R = "default_R";
        key_r0 = "default_r0";
        key_r1 = "default_r1";
    }

    R = Config::getMat(key_R);
    r0 = Config::getMat(key_r0);
    shift = Config::getMat(key_r1);

    float fov = Config::getFloat("fov");
    fov = fov / 180.0f * M_PIf32;
    float camera_matrix_data[] = {lastFrame.cols / fov, 0, lastFrame.cols / 2.0f, 0, lastFrame.cols / fov, lastFrame.rows / 2.0f, 0, 0, 1};
    camera_matrix = cv::Mat(3, 3, CV_32F);
    cv::Mat tocp(3, 3, CV_32F, camera_matrix_data);
    tocp.copyTo(camera_matrix);
    camera_matrix_inv = camera_matrix.inv();

    dist_coeffs = Config::getMat("dist-coeffs");

    std::vector<cv::Point3f> points_shifted;
    for (auto& pt : object_points)
        points_shifted.push_back(pt + (cv::Point3f)shift);
    project(points_shifted, cam_points_calib);

    capture();
}

cv::Point2f Camera::project(cv::Point3f point_real) {
    cv::Point3f point = (cv::Vec3f)(cv::Mat)(R.t()*(cv::Mat)(cv::Vec3f(point_real) - r0));
    if (point.z <= 0)
        return {0,0};
    point /= point.z;
    if (!isVisibleReal({point.x, point.y}))
        return {0, 0};
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
    r0 = (cv::Mat)(tvecs);
    cv::Rodrigues(rvecs, R);
    R = R.t();
    r0 = (cv::Mat)(-R * (cv::Mat)r0);
    r0 = r0 + (cv::Vec3f)shift;
    this->shift = shift;

    std::string key_R = std::string("cam") + std::to_string(id) + "_R";
    std::string key_r0 = std::string("cam") + std::to_string(id) + "_r0";
    std::string key_r1 = std::string("cam") + std::to_string(id) + "_r1";

    Config::setMat(key_R, R);
    Config::setMat(key_r0, (cv::Mat)r0);
    Config::setMat(key_r1, (cv::Mat)shift);
    Config::save();
}

void Camera::project(const std::vector<cv::Point3f> &points_real, std::vector<cv::Point2f> &points_cam) {
    points_cam.clear();
    for (auto& pt : points_real)
        points_cam.push_back(project(pt));
}

void Camera::capture() {
    cv::Mat buf;
    cap >> buf;
    float scale = Config::getFloat("camera_scale", 1);
    cv::resize(buf, lastFrame, {0,0}, scale, scale);
//    cv::undistort(buf, lastFrame, camera_matrix, dist_coeffs);
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
        person.views = 1;
        people.push_back(person);
    }
}

void camera_mouse_callback(int event, int x, int y, int flags, void *userdata) {
    ((Camera*)userdata)->mouse_callback(event, {(float)x, (float)y}, flags);
}

void Camera::show(Room* room, bool fill, bool wireframe, bool points, bool text) {
    this->room = room;
    this->fill = fill;
    this->wireframe = wireframe;
    this->points = points;
    this->text = text;
    lastFrame.copyTo(dispFrame);

    for (std::map<std::string, Room::Model>::iterator it = room->objects.begin(); it != room->objects.end(); it++) {
        for (int i = 0; i < it->second.edges.size(); i++) {
            if (fill) {
                cv::Mat canvas(dispFrame.rows, dispFrame.cols, CV_8UC3, cv::Scalar{0, 0, 0, 0});
                cv::Mat alpha_mask(dispFrame.rows, dispFrame.cols, CV_8UC3, cv::Scalar{0});
                cv::Mat alpha_mask0(dispFrame.rows, dispFrame.cols, CV_8UC3, cv::Scalar{0});
                float alpha = Config::getFloat("draw_alpha", 0.1);
                std::vector<cv::Point2f> edge;
//                if (!isVisible(it->second.edges[i].points, true))
//                    continue;
                project(it->second.edges[i].points, edge);
                float heat = it->second.hits / 3;
                if (heat > 1) heat = 1;
                cv::Scalar color = cv::Scalar(0, 0, 255, 255) * heat + cv::Scalar(255, 0, 0, 255) * (1 - heat);
                cv::Mat edgemat(edge);
                edgemat.convertTo(edgemat, CV_32S);
                cv::fillConvexPoly(canvas, {edgemat}, color);
                cv::fillConvexPoly(alpha_mask, {edgemat}, alpha * cv::Scalar{255, 255, 255});
                alpha_mask0 += alpha_mask;
                alpha_mask.setTo(0);
                cv::multiply(canvas, alpha_mask0, canvas, 1.0 / 255.0, CV_8UC3);
                cv::subtract(cv::Mat(dispFrame.rows, dispFrame.cols, CV_8UC3, {255, 255, 255}), alpha_mask0,
                             alpha_mask0);
                cv::multiply(dispFrame, alpha_mask0, dispFrame, 1.0 / 255.0, CV_8UC3);

                dispFrame += canvas;
            }

            if (wireframe) {
                for (int i = 0; i < it->second.edges.size(); i++) {
//                    std::vector<cv::Point2f> edge;
//                    if (!isVisible(it->second.edges[i].points, true))
//                        continue;
//                    project(it->second.edges[i].points, edge);
                    float heat = it->second.hits / 3;
                    if (heat > 1) heat = 1;
                    cv::Scalar color = fill ? cv::Scalar{0, 0, 0} : cv::Scalar(0, 0, 255, 255) * heat +
                                                                    cv::Scalar(255, 0, 0, 255) * (1 - heat);
                    for (int j = 0; j < it->second.edges[i].points.size(); j++)
                    {
                        auto ptr1 = it->second.edges[i].points[j % it->second.edges[i].points.size()];
                        auto ptr2 = it->second.edges[i].points[(j+1) % it->second.edges[i].points.size()];
                        cv::Point2f pt1;
                        cv::Point2f pt2;
                        if (projectLine(ptr1, ptr2, pt1, pt2))
                            cv::line(dispFrame, pt1, pt2, color, 1);
                    }
                }
            }

            if (points)
            {
                for (int i = 0; i < it->second.edges.size(); i++) {
                    float heat = it->second.hits / 3;
                    if (heat > 1) heat = 1;
                    cv::Scalar color = wireframe ? cv::Scalar{0, 0, 0} : cv::Scalar(0, 0, 255, 255) * heat +
                                                                    cv::Scalar(255, 0, 0, 255) * (1 - heat);
                    for (auto& pt3 : it->second.edges[i].points)
                        cv::circle(dispFrame, project(pt3), 2, color, -1);
                }
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
                cv::putText(dispFrame, it->first, project(center_of_mass), cv::FONT_HERSHEY_SIMPLEX, 1,
                            {255, 255, 255});
            }

            std::vector<cv::Scalar> colors = {{255, 255, 255},
                                              {0,   0,   255},
                                              {0,   255, 0},
                                              {255, 0,   0}};
            for (int i = 0; i < cam_points_calib.size(); i++) {
                cv::circle(dispFrame, cam_points_calib[i], 3, colors[i], -1);
            }

            for (auto &person : room->getPeople()) {
                cv::Point2f pt1;
                cv::Point2f pt2;
                if (!projectLine(person.position, person.target, pt1, pt2))
                    continue;
                cv::line(dispFrame, pt1, pt2, {0, 255, 255});
                cv::circle(dispFrame, pt1, 4, {0, 0, 255}, -1);
            }

        }
    }

    cv::imshow("Cam" + std::to_string(id), dispFrame);
    cv::setMouseCallback("Cam" + std::to_string(id), camera_mouse_callback, this);
}

void Camera::mouse_callback(int event, cv::Point2f point, int flags) {
    if (event == cv::EVENT_LBUTTONDOWN)
        isLBDown = true;
    else if (event == cv::EVENT_LBUTTONUP)
        isLBDown = false;
    else if (event == cv::EVENT_RBUTTONDOWN) {
        dragStart3D = unproject(point);
        isRBDown = true;
    }
    else if (event == cv::EVENT_RBUTTONUP)
        isRBDown = false;
    else if (event == cv::EVENT_MOUSEHWHEEL)
    {
        float delta = cv::getMouseWheelDelta(flags);
        float r = cv::norm(r0 - shift) * std::exp(delta / 10.0);
        if (r < 3) r = 3;
        if (r > 10) r = 10;
        r0 = (r0 - shift) * r / cv::norm(r0 - shift) + shift;
    }

    if (isLBDown) {
        float minDist = cv::norm(point - cam_points_calib[0]);
        float nearest_i = 0;
        for (int i = 1; i < cam_points_calib.size(); i++) {
            float dist = cv::norm(point - cam_points_calib[i]);
            if (dist < minDist) {
                minDist = dist;
                nearest_i = i;
            }
        }
        cam_points_calib[nearest_i] = point;
    } else if (isRBDown) {
        cv::Point3f drag = dragStart3D - unproject(point);
        shift = (cv::Point3f)shift + drag;
    } else if (event == cv::EVENT_MOUSEHWHEEL) {
        std::vector<cv::Point3f> points_shifted;
        for (auto& pt : object_points)
            points_shifted.push_back(pt + (cv::Point3f)shift);
        project(points_shifted, cam_points_calib);
    } else
        return;

    calibrate(cam_points_calib, shift);
    show(room, fill, wireframe, points, text);
    room->show2D();
}

bool Camera::isVisible(cv::Point3f point_real, bool check_bounds) {
    cv::Point3f point = (cv::Vec3f)(cv::Mat)(R.t()*(cv::Mat)(cv::Vec3f(point_real) - r0));
    if (point.z <= 0)
        return false;
    if (!check_bounds)
        return true;
    cv::Point2f point2 = distortPoint({point.x, point.y}, camera_matrix, dist_coeffs);
    return isVisible(point2);
}

bool Camera::isVisible(std::vector<cv::Point3f> points_real, bool check_bounds) {
    for (auto& point : points_real)
        if (isVisible(point, check_bounds))
            return true;
    return false;
}

const std::vector<Person> &Camera::getVisiblePeople() {
    return people;
}

bool Camera::isVisible(cv::Point2f point) {
    return point.x > 0 && point.y > 0 && point.x < lastFrame.cols && point.y < lastFrame.rows;
}

bool Camera::projectLine(cv::Point3f ptr1, cv::Point3f ptr2, cv::Point2f &pt1, cv::Point2f &pt2) {
    ptr1 = (cv::Vec3f)(cv::Mat)(R.t()*(cv::Mat)(cv::Vec3f(ptr1) - r0));
    ptr2 = (cv::Vec3f)(cv::Mat)(R.t()*(cv::Mat)(cv::Vec3f(ptr2) - r0));
    if (ptr1.z <= 0)
    {
        if (ptr2.z <= 0)
            return false;
        else // ptr2.z > 0
            ptr1 = ptr2 + (ptr1 - ptr2) * (0.001 - ptr2.z) / (ptr1.z - ptr2.z);
    } else { // ptr1.z > 0
        if (ptr2.z <= 0)
            ptr2 = ptr1 + (ptr2 - ptr1) * (0.001 - ptr1.z) / (ptr2.z - ptr1.z);
    }
    ptr1 /= ptr1.z;
    ptr2 /= ptr2.z;

    cv::Point2f pt1t = {ptr1.x, ptr1.y};
    cv::Point2f pt2t = {ptr2.x, ptr2.y};

    if (!isVisibleReal(pt1t) && !isVisibleReal(pt1t))
        return false;

    if (!isVisibleReal(pt1t) || !isVisibleReal(pt2t))
    {
        if (!isVisibleReal(pt1t))
        {
            cv::Point2f tmp = pt1t;
            pt1t = pt2t;
            pt2t = tmp;
        } // pt1t is visible, pt2t is invisible;
        if (pt2t.x < -getFOVR()/2) pt2t = pt1t + (pt2t - pt1t) * (-getFOVR()/2-pt1t.x) / (pt2t.x - pt1t.x);
        if (pt2t.y < -getVFOVR()/2) pt2t = pt1t + (pt2t - pt1t) * (-getVFOVR()/2-pt1t.y) / (pt2t.y - pt1t.y);
        if (pt2t.x > getFOVR()/2) pt2t = pt1t + (pt2t - pt1t) * (getFOVR()/2-pt1t.x) / (pt2t.x - pt1t.x);
        if (pt2t.y > getVFOVR()/2) pt2t = pt1t + (pt2t - pt1t) * (getVFOVR()/2-pt1t.y) / (pt2t.y - pt1t.y);
    }
    pt1 = distortPoint(pt1t, camera_matrix, dist_coeffs);
    pt2 = distortPoint(pt2t, camera_matrix, dist_coeffs);
    return true;
}

cv::Point3f Camera::rotateToReal(cv::Point3f point_cam) {
    cv::Point3f rot = (cv::Vec3f)(cv::Mat)(R * (cv::Mat)point_cam);
    return rot + (cv::Point3f)r0;
}

void Camera::rotateToReal(std::vector<cv::Point3f>& points_cam) {
    for (auto& pt : points_cam)
        pt = rotateToReal(pt);
}

float Camera::getFOV() {
    return Config::getFloat("fov");
}

float Camera::getVFOV() {
    return lastFrame.rows * getFOV() / lastFrame.cols;
}

int Camera::getID() {
    return id;
}

bool Camera::isVisibleReal(cv::Point2f point) {
    return point.x > -getFOVR()/2 && point.x < getFOVR()/2 && point.y > -getVFOVR()/2 && point.y < getVFOVR()/2;
}

float Camera::getFOVR() {
    return Config::getFloat("fov") / 180.0 * M_PIf32;
}

float Camera::getVFOVR() {
    return lastFrame.rows * getFOVR() / lastFrame.cols;
}


}