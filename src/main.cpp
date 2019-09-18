//
// Created by alex on 13.09.2019.
//

#include "Config.h"

#include <opencv2/opencv.hpp>
#include "Camera.h"

int main()
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

    room.addCamera(new jalo::Camera(5, "../videos/1565609313_23_87/cam5.mp4"));
    room.addCamera(new jalo::Camera(6, "../videos/1565609313_23_87/cam6.mp4"));
    room.addCamera(new jalo::Camera(7, "../videos/1565609313_23_87/cam7.mp4"));
//    room.addCamera(new jalo::Camera(8, "../videos/1565609313_23_87/cam8.mp4"));
    room.addCamera(new jalo::Camera(9, "../videos/1565609313_23_87/cam9.mp4"));
//    room.addCamera(new jalo::Camera(10, "../videos/1565609313_23_87/cam10.mp4"));
    room.addCamera(new jalo::Camera(12, "../videos/1565609313_23_87/cam12.mp4"));
    room.addCamera(new jalo::Camera(14, "../videos/1565609313_23_87/cam14.mp4"));
    room.addCamera(new jalo::Camera(15, "../videos/1565609313_23_87/cam15.mp4"));
//    room.addCamera(new jalo::Camera(16, "../videos/1565609313_23_87/cam16.mp4"));
//    room.addCamera(new jalo::Camera(17, "../videos/1565609313_23_87/cam17.mp4"));
    room.addCamera(new jalo::Camera(18, "../videos/1565609313_23_87/cam18.mp4"));
//    room.addCamera(new jalo::Camera(19, "../videos/1565609313_23_87/cam19.mp4"));
//    room.addCamera(new jalo::Camera(20, "../videos/1565609313_23_87/cam20.mp4"));
    room.addCamera(new jalo::Camera(21, "../videos/1565609313_23_87/cam21.mp4"));
//    room.addCamera(new jalo::Camera(22, "../videos/1565609313_23_87/cam22.mp4"));
//    room.addCamera(new jalo::Camera(23, "../videos/1565609313_23_87/cam23.mp4"));
//    room.addCamera(new jalo::Camera(24, "../videos/1565609313_23_87/cam24.mp4"));
//    room.addCamera(new jalo::Camera(26, "../videos/1565609313_23_87/cam26.mp4"));
    room.addCamera(new jalo::Camera(27, "../videos/1565609313_23_87/cam27.mp4"));
//    room.addCamera(new jalo::Camera(28, "../videos/1565609313_23_87/cam28.mp4"));
//    room.addCamera(new jalo::Camera(29, "../videos/1565609313_23_87/cam29.mp4"));
//    room.addCamera(new jalo::Camera(30, "../videos/1565609313_23_87/cam30.mp4"));
//    room.addCamera(new jalo::Camera(31, "../videos/1565609313_23_87/cam31.mp4"));
    room.addCamera(new jalo::Camera(32, "../videos/1565609313_23_87/cam32.mp4"));
//    room.addCamera(new jalo::Camera(33, "../videos/1565609313_23_87/cam33.mp4"));
//    room.addCamera(new jalo::Camera(34, "../videos/1565609313_23_87/cam34.mp4"));
//    room.addCamera(new jalo::Camera(35, "../videos/1565609313_23_87/cam35.mp4"));
//    room.addCamera(new jalo::Camera(36, "../videos/1565609313_23_87/cam36.mp4"));
//    room.addCamera(new jalo::Camera(37, "../videos/1565609313_23_87/cam37.mp4"));
//    room.addCamera(new jalo::Camera(38, "../videos/1565609313_23_87/cam38.mp4"));
//    room.addCamera(new jalo::Camera(39, "../videos/1565609313_23_87/cam39.mp4"));
    room.addCamera(new jalo::Camera(40, "../videos/1565609313_23_87/cam40.mp4"));


    room.showCameras();
    room.show2D();
    cv::waitKey();

    while(true)
    {
        room.capture();
        room.detectPeople();
        room.intersectShouldersDirectionWithObjects();
        room.showCameras();
        room.show2D();
        if (cv::waitKey(1) != -1)
            break;
    }
}