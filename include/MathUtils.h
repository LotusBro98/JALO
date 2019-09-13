//
// Created by alex on 13.09.2019.
//

#ifndef JALO_MATHUTILS_H
#define JALO_MATHUTILS_H

#include <opencv2/opencv.hpp>

namespace jalo {

cv::Point2f distortPoint(const cv::Point2f & src,
                         const cv::Mat & cameraMatrix, const cv::Mat & distorsionMatrix);

cv::Point2f undistortPoint(const cv::Point2f & src,
                         const cv::Mat & cameraMatrix, const cv::Mat & distorsionMatrix);

bool intersect(
        std::vector<cv::Point3f> edge,
        cv::Point3f position,
        cv::Point3f& target
);

}


#endif //JALO_MATHUTILS_H
