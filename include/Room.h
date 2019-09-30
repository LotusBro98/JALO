//
// Created by alex on 13.09.2019.
//

#ifndef JALO_ROOM_H
#define JALO_ROOM_H

namespace jalo {
    class Room;
}

#include "stlParser.h"
#include "Camera.h"
#include <opencv2/opencv.hpp>

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

namespace jalo {

class Room {
public:
    struct Edge {
        std::vector<cv::Point3f> points;
        int hits;
    };

    struct Model {
        std::vector<Edge> edges;
        int hits;
    };

    explicit Room();

    void add_object(std::string name, std::string filename);
    void addCamera(Camera* camera);
    void showCameras();
    void connectToDB(std::string hostname, std::string user, std::string password);

    void capture(int skips = 1);
    void detectPeople();
    void intersectShouldersDirectionWithObjects();
    void dumpToDB();
    void loadHeatFromDB();

    int getObjectHits(std::string name);
    std::vector<Person> getPeople();

    void show2D();

    std::map<std::string, Model> objects;

private:
    std::vector<Camera*> cameras;
    std::vector<Person> people;

    sql::Driver *driver;
    sql::Connection *con;

    float seq;
    unsigned long long int time;
};

}


#endif //JALO_ROOM_H
