//
// Created by alex on 13.09.2019.
//

#include "Config.h"

#include <opencv2/opencv.hpp>
#include "Camera.h"
#include <string>
#include <iostream>
#include <dirent.h>

int main(int argc, char* argv[])
{
    jalo::Room room;

    if (argc != 2)
    {
        std::cerr << "Usage: ./shoulders  <video_path>\n";
        return -1;
    }

    std::string video_path = std::string(argv[1]);
    std::vector<int> cameras;
    cv::Mat cammat = jalo::Config::getMat("cameras");
    for (int i = 0; i < cammat.total(); i++)
        cameras.push_back(cammat.at<float>(i));

    std::string models_path = jalo::Config::getString("models_path", "../models");

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (models_path.data())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            if (std::string(ent->d_name) == "." || std::string(ent->d_name) == "..")
                continue;
            std::string path = std::string("") + ent->d_name;
            std::string name = path.substr(0, path.find_last_of("."));
            path = models_path + "/" + path;
            room.add_object(name, path);
        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("Failed to  open models dir");
        return EXIT_FAILURE;
    }

    for (int cam : cameras) {
        std::string path = video_path + "/cam" + std::to_string(cam) + ".mp4";
        try {
            room.addCamera(new jalo::Camera(cam, path.data()));
        } catch (std::exception& e) {
            std::cerr << path << ": " << e.what() << "\n";
        }
    }

    if (jalo::Config::getBool("interactive", true)) {
        room.showCameras();
        room.show2D();
        cv::waitKey();
    } else if (jalo::Config::getBool("interactive2D", true)) {
        room.show2D();
        cv::waitKey(1);
    }

    room.connectToDB(
            jalo::Config::getString("db_host", "localhost"),
            jalo::Config::getString("db_user", "alex"),
            jalo::Config::getString("db_password", "kbkbrnjy")
            );

    int seq = 0;
    int skips = jalo::Config::getInt("camera_skips", 20);
    while(true) {
        try {
            room.capture(skips);
        } catch (std::exception& e) {
            std::cerr << "\nUnable to capture more. Finishing.\n";
            return 0;
        }
        std::cout << "\rframe " << seq << "           ";
        fflush(stdout);
        seq += skips;
        room.detectPeople();
        room.intersectShouldersDirectionWithObjects();
        room.dumpToDB();
        if (jalo::Config::getBool("interactive")) {
            room.showCameras();
            room.show2D();
            if (cv::waitKey(1) != -1)
                break;
        } else if (jalo::Config::getBool("interactive2D")) {
            room.show2D();
            if (cv::waitKey(1) != -1)
                break;
        }
    }
}