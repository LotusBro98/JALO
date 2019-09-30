//
// Created by alex on 24.09.2019.
//


#include "Config.h"

#include <opencv2/opencv.hpp>
#include "Camera.h"

int main(int argc, char* argv[])
{
    jalo::Room room;

    room.add_object("1", "../models/shop1/1.stl");
    room.add_object("2", "../models/shop1/2.stl");
    room.add_object("3", "../models/shop1/3.stl");
    room.add_object("4", "../models/shop1/4.stl");
    room.add_object("5", "../models/shop1/5.stl");
    room.add_object("6", "../models/shop1/6.stl");
    room.add_object("7", "../models/shop1/7.stl");
    room.add_object("8", "../models/shop1/8.stl");
    room.add_object("9", "../models/shop1/9.stl");
    room.add_object("9", "../models/shop1/9.stl");
    room.add_object("10", "../models/shop1/10.stl");
    room.add_object("11", "../models/shop1/11.stl");
    room.add_object("12", "../models/shop1/12.stl");
    room.add_object("13", "../models/shop1/13.stl");
    room.add_object("14", "../models/shop1/14.stl");
    room.add_object("15", "../models/shop1/15.stl");
    room.add_object("16", "../models/shop1/16.stl");
    room.add_object("17", "../models/shop1/17.stl");
    room.add_object("18", "../models/shop1/18.stl");
    room.add_object("19", "../models/shop1/19.stl");
    room.add_object("20", "../models/shop1/20.stl");
    room.add_object("21", "../models/shop1/21.stl");
    room.add_object("22", "../models/shop1/22.stl");
    room.add_object("23", "../models/shop1/23.stl");
    room.add_object("24", "../models/shop1/24.stl");
    room.add_object("25", "../models/shop1/25.stl");
    room.add_object("26", "../models/shop1/26.stl");
    room.add_object("27", "../models/shop1/27.stl");
    room.add_object("28", "../models/shop1/28.stl");

    room.connectToDB(
            jalo::Config::getString("db_host", "localhost"),
            jalo::Config::getString("db_user", "alex"),
            jalo::Config::getString("db_password", "kbkbrnjy")
    );

    room.loadHeatFromDB();

    room.show2D();
    cv::waitKey();
}