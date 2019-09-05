//
// Created by alex on 30.08.2019.
//

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <openpose/headers.hpp>


auto points = std::vector<cv::Point2f>{{0, 0}, {0, 1}, {1, 0}, {1, 1}};
auto object_points = std::vector<cv::Point3f>{{0, 0, 0}, {0, 1, 0}, {1, 0, 0}, {1, 1, 0}};

auto box = std::vector<std::vector<cv::Point3f>>{
    {{0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 0, 0}},
    {{0, 0, 0}, {0, 1, 0}, {0, 1, 1}, {0, 0, 1}},
    {{1, 1, 1}, {1, 1, 0}, {0, 1, 0}, {0, 1, 1}},
    {{1, 0, 1}, {1, 1, 1}, {1, 1, 0}, {1, 0, 0}},
    {{0, 0, 0}, {0, 0, 1}, {1, 0, 1}, {1, 0, 0}},
    {{0, 0, 1}, {0, 1, 1}, {1, 1, 1}, {1, 0, 1}},
};

std::vector<std::vector<cv::Point2i>> box2d;

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


    camera_matrix.at<float>(0,0) = frame.cols / 2;
    camera_matrix.at<float>(1,1) = frame.cols / 2;
    camera_matrix.at<float>(0, 2) = frame.cols / 2;
    camera_matrix.at<float>(1, 2) = frame.rows / 2;
    cv::invert(camera_matrix, camera_matrix_inv);

    for (auto& line : box)
    {
        std::vector<cv::Point2i> line2d;
        for (auto& pt : line)
        {
            line2d.push_back(project(pt));
        }
        box2d.push_back(line2d);
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

        auto datum = opWrapper.emplaceAndPop(frame);
        auto& keypoints = datum->front()->poseKeypoints;
        for (int i = 0; i < keypoints.getSize(0); i++) {
            cv::Point2f leftShoulder = {keypoints.at({i, 5, 0}), keypoints.at({i, 5, 1})};
            cv::Point2f rightShoulder = {keypoints.at({i, 2, 0}), keypoints.at({i, 2, 1})};

            cv::circle(display, leftShoulder, 5, {255, 0, 0}, -1);
            cv::circle(display, rightShoulder, 5, {0, 0, 255}, -1);

            cv::Point3f ls3D = unproject(leftShoulder, 1);
            cv::Point3f rs3D = unproject(rightShoulder, 1);
            cv::Point3f center = 0.5 * (ls3D + rs3D);
            cv::Point3f dir = {-(rs3D.y - ls3D.y), rs3D.x - ls3D.x, 0};
            dir /= cv::norm(dir);

            float intx0 = (0 - center.x) / dir.x;
            float intx1 = (1 - center.x) / dir.x;
            float inty0 = (0 - center.y) / dir.y;
            float inty1 = (1 - center.y) / dir.y;

            cv::Point3f intx0pt = center + intx0 * dir;
            cv::Point3f intx1pt = center + intx1 * dir;
            cv::Point3f inty0pt = center + inty0 * dir;
            cv::Point3f inty1pt = center + inty1 * dir;

            bool intx0hit = intx0 > 0 && intx0pt.y > 0 && intx0pt.y < 1;
            bool intx1hit = intx1 > 0 && intx1pt.y > 0 && intx1pt.y < 1;
            bool inty0hit = inty0 > 0 && inty0pt.x > 0 && inty0pt.x < 1;
            bool inty1hit = inty1 > 0 && inty1pt.x > 0 && inty1pt.x < 1;

            std::vector<float> ints = {
                    intx0hit ? intx0 : 100500,
                    intx1hit ? intx1 : 100500,
                    inty0hit ? inty0 : 100500,
                    inty1hit ? inty1 : 100500,
            };
            float intall = *std::min_element(ints.begin(), ints.end());
            float intmax = 1.5;
            if (intall == 100500)
                intall = intmax;

            if (intall > intmax)
                intall = intmax;

            cv::Point3f far = center + intall * dir;
            cv::Point3f far1 = center + intmax * dir;

            cv::line(display, project(center), project(far), {0,255,255}, 2);
            cv::line(display, project(far), project(far1), {0,0,255}, 2);

//            char text[20];
//            sprintf(text, "%.2f %.2f %.2f %.2f", center.x, center.y, inty0, intall);
//            cv::putText(display, text, project(center), cv::FONT_HERSHEY_SIMPLEX, 0.5, {255,255,255});
        }
//        display = datum->at(0)->cvOutputData;

        std::vector<cv::Scalar>colors = {{0, 0, 255}, {0, 255, 0}, {255, 0, 0}, {255, 255, 0}};
        for (int  i = 0; i  < 4; i++)
        {
            cv::circle(display, (cv::Vec2i)(cv::Vec2f)points[i], 5, colors[i], -1);
        }

        cv::polylines(display, box2d, true, {0, 255, 0});

        cv::imshow("Image", display);
        videoOut << display;
        cv::waitKey(20);
    }

    return 0;
}
