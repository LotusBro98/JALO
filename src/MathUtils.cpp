//
// Created by alex on 13.09.2019.
//

#include "../include/MathUtils.h"

namespace jalo {

cv::Point2f distortPoint(const cv::Point2f &src,
                         const cv::Mat &cameraMatrix, const cv::Mat &distorsionMatrix) {
    float fx = cameraMatrix.at<float>(0, 0);
    float fy = cameraMatrix.at<float>(1, 1);
    float ux = cameraMatrix.at<float>(0, 2);
    float uy = cameraMatrix.at<float>(1, 2);

    float k1 = distorsionMatrix.at<float>(0, 0);
    float k2 = distorsionMatrix.at<float>(0, 1);
    float p1 = distorsionMatrix.at<float>(0, 2);
    float p2 = distorsionMatrix.at<float>(0, 3);
    float k3 = distorsionMatrix.at<float>(0, 4);
    //BOOST_FOREACH(const cv::Point2f &p, src)

    const cv::Point2f &p = src;
    float x = p.x;
    float y = p.y;
    float xCorrected, yCorrected;
    //Step 1 : correct distorsion

    float r2 = x * x + y * y;
    //radial distorsion
    xCorrected = x * (1. + k1 * r2 + k2 * r2 * r2 + k3 * r2 * r2 * r2);
    yCorrected = y * (1. + k1 * r2 + k2 * r2 * r2 + k3 * r2 * r2 * r2);

    //tangential distorsion
    //The "Learning OpenCV" book is wrong here !!!
    //False equations from the "Learning OpenCv" book
    //xCorrected = xCorrected + (2. * p1 * y + p2 * (r2 + 2. * x * x));
    //yCorrected = yCorrected + (p1 * (r2 + 2. * y * y) + 2. * p2 * x);
    //Correct formulae found at : http://www.vision.caltech.edu/bouguetj/calib_doc/htmls/parameters.html
    xCorrected = xCorrected + (2. * p1 * x * y + p2 * (r2 + 2. * x * x));
    yCorrected = yCorrected + (p1 * (r2 + 2. * y * y) + 2. * p2 * x * y);

    //Step 2 : ideal coordinates => actual coordinates

    xCorrected = xCorrected * fx + ux;
    yCorrected = yCorrected * fy + uy;

    return cv::Point2f(xCorrected, yCorrected);
}

cv::Point2f undistortPoint(const cv::Point2f &src, const cv::Mat &cameraMatrix, const cv::Mat &distorsionMatrix) {
    std::vector<cv::Point2f> points = {src};
    cv::undistortPoints(points, points, cameraMatrix, distorsionMatrix);
    return points[0];
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

}