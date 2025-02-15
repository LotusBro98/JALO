//
// Created by alex on 13.09.2019.
//

#ifndef JALO_CONFIG_H
#define JALO_CONFIG_H

#include <string>
#include <map>

#include <opencv2/opencv.hpp>

namespace jalo {

class Config {
public:

    static cv::Mat getMat(std::string key);
    static std::string getString(std::string key, std::string def = "");
    static float getFloat(std::string key, float def = 0);
    static bool getBool(std::string key, bool def = false);
    static int getInt(std::string key, int def = 0);

    static void save();

    static bool haskey(std::string key);

    static void setMat(std::string key, const cv::Mat& mat);
    static void setString(std::string key, const std::string& value);
    static void setFloat(std::string key, float value);
    static void setBool(std::string key, bool value);

private:
    static Config &get();

    std::map<std::string, std::string> data;

    Config();
};

}

#endif //JALO_CONFIG_H
