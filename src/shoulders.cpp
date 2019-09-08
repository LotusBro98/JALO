//
// Created by alex on 30.08.2019.
//

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <openpose/headers.hpp>


auto points = std::vector<cv::Point2f>{{0, 0}, {0, 1}, {1, 0}, {1, 1}};
auto object_points = std::vector<cv::Point3f>{{0, 0, 0}, {0, 1, 0}, {1, 0, 0}, {1, 1, 0}};

auto box = std::vector<std::vector<cv::Point3f>>{
    {{-1, -2, 0}, {-1, -1.5, 0}, {-1, -1.5, 2}, {-1, -2, 2}},
    {{-1, -1.5, 0}, {-1, -1, 0}, {-1, -1, 2}, {-1, -1.5, 2}},
    {{-1, -1, 0}, {-1, -0.5, 0}, {-1, -0.5, 2}, {-1, -1, 2}},
    {{-1, -0.5, 0}, {-1, 0, 0}, {-1, 0, 2}, {-1, -0.5, 2}},
    {{-1, 0, 0}, {-1, 0.5, 0}, {-1, 0.5, 2}, {-1, 0, 2}},
    {{-1, 0.5, 0}, {-1, 1, 0}, {-1, 1, 2}, {-1, 0.5, 2}},
    {{-1, 1, 0}, {-1, 1.5, 0}, {-1, 1.5, 2}, {-1, 1, 2}},
    {{-1, 1.5, 0}, {-1, 2, 0}, {-1, 2, 2}, {-1, 1.5, 2}},
    {{-1, 2, 0}, {-1, 2.5, 0}, {-1, 2.5, 2}, {-1, 2, 2}},
    {{-1, 2.5, 0}, {-1, 3, 0}, {-1, 3, 2}, {-1, 2.5, 2}},

    {{2, -2, 0},   {2, -1.5, 0},{2, -1.5, 2}, {2, -2, 2}},
    {{2, -1.5, 0}, {2, -1, 0},  {2, -1, 2},   {2, -1.5, 2}},
    {{2, -1, 0},   {2, -0.5, 0},{2, -0.5, 2}, {2, -1, 2}},
    {{2, -0.5, 0}, {2, 0, 0},   {2, 0, 2},    {2, -0.5, 2}},
    {{2, 0, 0},    {2, 0.5, 0}, {2, 0.5, 2},  {2, 0, 2}},
    {{2, 0.5, 0},  {2, 1, 0},   {2, 1, 2},    {2, 0.5, 2}},
    {{2, 1, 0},    {2, 1.5, 0}, {2, 1.5, 2},  {2, 1, 2}},
    {{2, 1.5, 0},  {2, 2, 0},   {2, 2, 2},    {2, 1.5, 2}},
    {{2, 2, 0},    {2, 2.5, 0}, {2, 2.5, 2},  {2, 2, 2}},
//    {{2, 2.5, 0},  {2, 3, 0},   {2, 3, 2},    {2, 2.5, 2}},

    {{-1  , -2, 0}, {-1  , -2, 2}, {-0.5, -2, 2}, {-0.5, -2, 0}},
    {{-0.5, -2, 0}, {-0.5, -2, 2}, { 0.0, -2, 2}, { 0.0, -2, 0}},
    {{ 0  , -2, 0}, { 0  , -2, 2}, { 0.5, -2, 2}, { 0.5, -2, 0}},
    {{ 0.5, -2, 0}, { 0.5, -2, 2}, { 1.0, -2, 2}, { 1.0, -2, 0}},
    {{ 1  , -2, 0}, { 1  , -2, 2}, { 1.5, -2, 2}, { 1.5, -2, 0}},
    {{ 1.5, -2, 0}, { 1.5, -2, 2}, { 2.0, -2, 2}, { 2.0, -2, 0}},

//    {{1, 1, 1}, {1, 1, 0}, {0, 1, 0}, {0, 1, 1}},
//    {{1, 0, 1}, {1, 1, 1}, {1, 1, 0}, {1, 0, 0}},
//    {{0, 0, 1}, {0, 1, 1}, {1, 1, 1}, {1, 0, 1}},
};

std::vector<std::vector<cv::Point2i>> box2d;
std::vector<std::vector<cv::Point2i>> box2d2d;
std::vector<int> hits;

float Rinit[] = {-0.66325486, 0.40076259, -0.63204616, 0.74838018, 0.36022136, -0.55692697, 0.0044810306, -0.84239537, -0.53884137};
//cv::Mat R = cv::Mat::eye({3, 3}, CV_32F);
cv::Mat R(3, 3, CV_32F, Rinit);
//cv::Vec3f r0 = {0,0,0};
cv::Vec3f r0 = {1.88229, 2.48768, 2.20351};
cv::Mat camera_matrix = cv::Mat::eye({3, 3}, CV_32F);
cv::Mat camera_matrix_inv = cv::Mat::eye({3, 3}, CV_32F);

cv::Point2f project(cv::Point3f point)
{
    point = (cv::Vec3f)(cv::Mat)(R.t()*(cv::Mat)(cv::Vec3f(point) - r0));
    point /= point.z;
    point = (cv::Vec3f)(cv::Mat)(camera_matrix*(cv::Mat)(cv::Vec3f(point)));
    return {point.x, point.y};
}

cv::Point3f unproject(cv::Point2f point, float z = 0)
{
    cv::Vec3f dir = (cv::Mat) (R * camera_matrix_inv * (cv::Mat)cv::Vec3f(point.x, point.y, 1));
    float r = (z-r0[2]) / dir[2];
    cv::Vec3f proj = r0 + (r * dir);
    return proj;
}

bool intersect(
        std::vector<cv::Point3f> edge,
        cv::Point3f position,
        cv::Point3f& target
) {
    cv::Point3f ex = edge[1] - edge[0];
    cv::Point3f ey = edge[2] - edge[0];
    cv::Point3f ez = ex.cross(ey);
    ey = ez.cross(ex);
    ex /= cv::norm(ex);
    ey /= cv::norm(ey);
    ez /= cv::norm(ez);

    cv::Matx33f R{
            ex.x, ey.x, ez.x,
            ex.y, ey.y, ez.y,
            ex.z, ey.z, ez.z
    };

    cv::Point3f newPosition = R.t() * (position - edge[0]);
    cv::Point3f newTarget = R.t() * (target - edge[0]);
    if (newPosition.z * newTarget.z > 0)
        return false;
    newTarget = newPosition + (newTarget - newPosition) * (-newPosition.z) / (newTarget.z - newPosition.z);

//    cv::Mat disp(100,100, CV_8SC3, cv::Scalar{255, 255, 255});

    int intersects = 0;
    for (int i = 0; i < edge.size(); i++)
    {
        cv::Point3f cur = R.t() * (edge[i % edge.size()] - edge[0]);
        cv::Point3f next = R.t() * (edge[(i + 1) % edge.size()] - edge[0]);
//        cv::line(disp, {static_cast<int>(cur.x*20 + 50), static_cast<int>(cur.y*20 + 50)}, {static_cast<int>(next.x*20 + 50), static_cast<int>(next.y*20 + 50)}, {0, 255,0});
        if ((cur.y - newTarget.y) * (next.y - newTarget.y) > 0)
            continue;
        if (next.y == cur.y) {
            continue;
        } else {
            float xint = cur.x + (next.x - cur.x) * (newTarget.y - cur.y) / (next.y - cur.y);
            if (xint > newTarget.x)
                intersects++;
        }
    }
//    std::cout << newTarget << "\n";
//
//    cv::circle(disp, {static_cast<int>(newTarget.x*20+50), static_cast<int>(newTarget.y*20 + 50)}, 2, {0,0,255}, -1);
//    cv::imshow("Debug", disp);
//    std::cout << intersects << " ------\n";
//    cv::waitKey();

    if (intersects % 2 == 0)
        return false;

    target = R * newTarget + edge[0];
    return true;
}

bool buttonDown = false;
void mouseCallback (int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN)
        buttonDown = true;
    else if (event == cv::EVENT_LBUTTONUP)
        buttonDown = false;
    if (!buttonDown)
        return;

    float mindist = cv::norm((cv::Vec2f)points[0] - cv::Vec2f{(float)x, (float)y});
    int min = 0;
    for (int i = 1; i < 4; i++)
    {
        float dist = cv::norm((cv::Vec2f)points[i] - cv::Vec2f{(float)x, (float)y});
        if (dist < mindist)
        {
            mindist = dist;
            min = i;
        }
    }
    points[min].x = x;
    points[min].y = y;

    cv::Mat rvecs(cv::Vec3f{0,0,0});
    cv::Mat tvecs(cv::Vec3f{0,0,0});

    int ret = cv::solvePnP(object_points, points, camera_matrix, cv::Mat::zeros(1, 14, CV_32F), rvecs, tvecs, true, cv::SOLVEPNP_P3P);
    r0 = tvecs;
    cv::Rodrigues(rvecs, R);
    R = R.t();
    r0 = (cv::Mat)(-R * (cv::Mat)r0);

    std::cout << R << "\n" << r0 << "\n\n";

    box2d.clear();
    for (auto& line : box)
    {
        std::vector<cv::Point2i> line2d;
        for (auto& pt : line)
        {
            line2d.push_back(project(pt));
        }
        box2d.push_back(line2d);
    }
}


int main()
{
    cv::VideoCapture video("../videos/Воровство в одном из магазинов.mp4");
    cv::Mat frame;
    cv::Mat display;
    video >> frame;
    cv::VideoWriter videoOut("../videos/JALO.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), video.get(cv::CAP_PROP_FPS), {frame.cols, frame.rows});
    cv::VideoWriter videoOut2D("../videos/JALO2D.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), video.get(cv::CAP_PROP_FPS), {500, 500});

    camera_matrix.at<float>(0,0) = frame.cols / 2;
    camera_matrix.at<float>(1,1) = frame.cols / 2;
    camera_matrix.at<float>(0, 2) = frame.cols / 2;
    camera_matrix.at<float>(1, 2) = frame.rows / 2;
    cv::invert(camera_matrix, camera_matrix_inv);

    for (auto& line : box)
    {
        std::vector<cv::Point2i> line2d;
        for (auto& pt : line)
            line2d.push_back({static_cast<int>(300 - pt.x*100), static_cast<int>(pt.y*100+200)});
        box2d2d.push_back(line2d);
    }

    for (auto& line : box)
    {
        std::vector<cv::Point2i> line2d;
        for (auto& pt : line)
        {
            line2d.push_back(project(pt));
        }
        box2d.push_back(line2d);
        hits.push_back(0);
    }

    points.clear();
    for (auto& pt : object_points)
    {
        points.push_back(project(pt));
    }

    op::Wrapper opWrapper{op::ThreadManagerMode::Asynchronous};
    opWrapper.disableMultiThreading();
    opWrapper.start();

    cv::namedWindow("Image");
    cv::setMouseCallback("Image", mouseCallback);

    while (true)
    {
        video >> frame;
        if (frame.empty())
            break;
//        cv::resize(frame, frame, {0,0}, 0.25, 0.25);

        display = frame;
        cv::Mat display2D(510, 500, CV_8UC3, {0,0,0});

        bool hitsnow[hits.size()] = {};

        auto datum = opWrapper.emplaceAndPop(frame);
        auto& keypoints = datum->front()->poseKeypoints;
        for (int i = 0; i < keypoints.getSize(0); i++) {
            cv::Point2f leftShoulder = {keypoints.at({i, 5, 0}), keypoints.at({i, 5, 1})};
            cv::Point2f rightShoulder = {keypoints.at({i, 2, 0}), keypoints.at({i, 2, 1})};

            cv::circle(display, leftShoulder, 3, {255, 0, 0}, -1);
            cv::circle(display, rightShoulder, 3, {0, 0, 255}, -1);

            cv::Point3f ls3D = unproject(leftShoulder, 0.9);
            cv::Point3f rs3D = unproject(rightShoulder, 0.9);
            cv::Point3f center = 0.5 * (ls3D + rs3D);
            cv::Point3f dir = {-(rs3D.y - ls3D.y), rs3D.x - ls3D.x, 0};
            dir /= cv::norm(dir);

            cv::circle(display2D, {static_cast<int>(300-center.x * 100), static_cast<int>(center.y*100 + 200)}, 5, {0, 0, 255}, -1);

            float intmax = 3;

            cv::Point3f far = center + intmax * dir;
            cv::Point3f far1 = far;
            int lasthit= -1;
            for (int j = 0; j< box.size(); j++) {
                auto& edge = box[j];
                if (intersect(edge, center, far1))
                    lasthit = j;
            }
            if (lasthit != -1)
                hitsnow[lasthit] = true;

//            cv::Point2f far1proj = project(far1);
//            if (far1proj.y < 0) far1proj = project(center) + (far1proj - project(center)) * (-project(center).y) / (far1proj.y - project(center).y);
//            if (far1proj.x < 0) far1proj = project(center) + (far1proj - project(center)) * (-project(center).x) / (far1proj.x - project(center).x);
//            if (far1proj.y >= display.rows) far1proj = project(center) + (far1proj - project(center)) * (display.rows - project(center).y) / (far1proj.y - project(center).y);
//            if (far1proj.x >= display.cols) far1proj = project(center) + (far1proj - project(center)) * (display.cols - project(center).x) / (far1proj.x - project(center).x);
            cv::line(display, project(center), project(far1), {0,255,255}, 2);
//            cv::line(display, project(far1), project(far), {0,0,255}, 1);

            cv::line(display2D, {static_cast<int>(300-center.x*100), static_cast<int>(center.y*100+200)}, {static_cast<int>(300-far1.x*100), static_cast<int>(far1.y*100+200)}, {0,255,255}, 1);

//            char text[20];
//            sprintf(text, "%.2f %.2f %.2f %.2f", center.x, center.y, inty0, intall);
//            cv::putText(display, text, project(center), cv::FONT_HERSHEY_SIMPLEX, 0.5, {255,255,255});
        }
//        display = datum->at(0)->cvOutputData;

        for (int j = 0; j< box.size(); j++) {
            if (hitsnow[j]) {
                hits[j]+=5;
                if (hits[j] > 255)
                    hits[j] = 255;
            } else {
                hits[j]-=2;
                if (hits[j] < 0)
                    hits[j] = 0;
            }
        }


        for (int j = 0; j< box.size(); j++) {
            auto &edge = box2d[j];
            cv::polylines(display, edge, true, cv::Scalar{0, 0, 255} * hits[j] / 255.0 + cv::Scalar{255,0,0}*(1-hits[j]/255.0), 1);
        }
        for (int j = 0; j< box.size(); j++) {
            auto &edge = box2d2d[j];
            cv::polylines(display2D, edge, true, cv::Scalar{0, 0, 255} * hits[j] / 255.0 + cv::Scalar{255,0,0}*(1-hits[j]/255.0), 2);
        }

        std::vector<cv::Scalar>colors = {{0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {255, 255, 0}};
        for (int  i = 0; i  < 4; i++)
        {
            cv::circle(display, (cv::Vec2i)(cv::Vec2f)points[i], 5, colors[i], -1);
            cv::circle(display2D, {static_cast<int>(300-object_points[i].x*100), static_cast<int>(object_points[i].y*100+200)}, 5, colors[i], -1);
        }

        cv::imshow("Image", display);
        cv::imshow("2D room", display2D);
        videoOut << display;
        videoOut2D << display2D;
        cv::waitKey(1);
    }

    return 0;
}
