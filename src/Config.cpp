//
// Created by alex on 13.09.2019.
//

#include "../include/Config.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace jalo {

const char* config_filename = "vision.config";

Config &Config::get() {
    static Config config;
    return config;
}

Config::Config() {
    std::ifstream file(config_filename);
    if (file.is_open()) {

        char buf[1024];
        char buf2[1024];

        while (!file.eof()) {
            file.getline(buf, sizeof(buf), '\n');

            std::istringstream sstr(buf);
            sstr.getline(buf2, sizeof(buf2), ':');
            std::string key(buf2);
            if (!*buf2)
                continue;
            sstr.getline(buf2, sizeof(buf2), '\n');
            std::string val(buf2+1);
            data[key] = val;
        }
    }
}

void Config::save() {
    std::ofstream file(config_filename);
    for (auto & it : get().data)
        file << it.first << ": " << it.second << "\n";
}

cv::Mat Config::getMat(std::string key) {
    std::string& val = get().data[key];
    std::istringstream sstr(val);
    int rows;
    int cols;
    sstr >> rows;
    sstr.ignore(1);
    sstr >> cols;
    cv::Mat mat(rows, cols, CV_32F);
    for (auto it = mat.begin<float>(); it != mat.end<float>(); it++)
        sstr >> (*it);
    return mat;
}

void Config::setMat(std::string key, const cv::Mat &mat) {
    std::ostringstream sstr;
    sstr << mat.rows << "x" << mat.cols;
    for (auto it = mat.begin<float>(); it != mat.end<float>(); it++)
        sstr << " " << (*it);
    get().data[key] = sstr.str();
}

bool Config::haskey(std::string key) {
    return get().data.find(key) != get().data.end();
}

float Config::getFloat(std::string key, float def) {
    if (get().data.find(key) != get().data.end())
        return std::stof(get().data[key]);
    else {
        get().data[key] = std::to_string(def);
        save();
        return def;
    }
}

void Config::setFloat(std::string key, float value) {
    get().data[key] = std::to_string(value);
}

bool Config::getBool(std::string key, bool def) {
    if (get().data.find(key) != get().data.end())
        return std::stoi(get().data[key]);
    else {
        get().data[key] = std::to_string((int)def);
        save();
        return def;
    }
}

void Config::setBool(std::string key, bool value) {
    get().data[key] = std::to_string(value);
}

std::string Config::getString(std::string key, std::string def) {
    if (get().data.find(key) != get().data.end())
        return get().data[key];
    else {
        get().data[key] = def;
        save();
        return def;
    }
}

bool Config::getInt(std::string key, int def) {
    if (get().data.find(key) != get().data.end())
        return std::stoi(get().data[key]);
    else {
        get().data[key] = std::to_string(def);
        save();
        return def;
    }
}

}