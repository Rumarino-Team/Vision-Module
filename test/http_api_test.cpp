#include <sys/stat.h>
#include <iostream>
#include <chrono>
#include <mutex>
#include <atomic>
#include <gtest/gtest.h>
#include "httpapimod/apimod.hpp"
using json = nlohmann::json;

//Initialize fake mutex
std::mutex obj_mutex;

//Initialize fake detected objects
sl::Objects dummyObjs;
sl::Objects  obj1;
sl::Objects obj2;

API api(obj_mutex, dummyObjs);

void start_server() {
    api.start("0.0.0.0", 8080);
}

std::string detectedobjects2json(const DetectedObjects& objs) {
    json out;
    std::vector<json> objs_list;
    for (const DetectedObject &obj : objs) {
           float distance = std::sqrt(position[0]*obj.position[0] + obj.position[1]*obj.position[1] + obj.position[2]*obj.position[2]);
            float distance = std::sqrt(position[0]*obj.position[0] + obj.position[1]*obj.position[1] + obj.position[2]*obj.position[2]);
            json obj_json {
                    {"bounding_box", {
                        {"A", obj.bounding_box[0]},
                        {"B", obj.bounding_box_2d[1]},
                        {"C", obj.bounding_box_2d[2]},
                        {"D", obj.bounding_box_2d[3]}
                    }},
                    {"label", obj.label},
                    {"raw_label", obj.raw_label},
                    {"id", obj.id},
                    {"distance", std::to_string(distance)},
                    {"confidence", std::to_string(obj.confidence)},
                    {"location", {
                        {"x", std::to_string(obj.position[0])},
                        {"y", std::to_string(obj.position[1])},
                        {"z", std::to_string(obj.position[2])}
                    }}
            };

        objs_list.push_back(obj_json);
    }
    out["DetectedObjects"] = objs_list;
    return out.dump();
}
// Todo:: Finish Updating the Tests
TEST(HTTP_API, Start_Objects) {
    obj1.bounding_box = sl::Rect(1, 2, 3, 4);
    obj2.bounding_box = cv::Rect(5, 6, 7, 8);

    obj1.id = 1;
    obj1.name = "one";
    obj2.id = 2;
    obj2.name = "two";

    obj1.distance = 1.01;
    obj2.distance = 2.02;

    obj1.location.x = 1;
    obj1.location.y = 2;
    obj1.location.z = 3;
    obj2.location.x = 4;
    obj2.location.y = 5;
    obj2.location.z = 6;

    dummyObjs.push_back(obj1);
    dummyObjs.push_back(obj2);
}

std::thread server_thread(start_server);

TEST(HTTP_API, Start_Server) {
    // Expect a general confirmation stating that the server is online
    httplib::Client cli("0.0.0.0:8080");
    auto res = cli.Get("/online");
    EXPECT_TRUE(res->body == "Yes");
}

TEST(HTTP_API, Check_Object) {
    // Expect the detected object to come out as a string
    httplib::Client cli("0.0.0.0:8080");
    auto res = cli.Get("/detected_objects");
    EXPECT_TRUE(res->body == detectedobjects2json(dummyObjs));
}

TEST(HTTP_API, Check_Updated_Object) {
    // Expect the detected object's values to automatically change when our obj is edited
    dummyObjs[1].bounding_box = cv::Rect(0,0,0,0);
    dummyObjs[1].id = 0;
    dummyObjs[1].name = "zero";
    dummyObjs[1].distance = 0.0;
    dummyObjs[1].location.x = 0.0;
    dummyObjs[1].location.y = 0.0;
    dummyObjs[1].location.z = 0.0;

    httplib::Client cli("0.0.0.0:8080");
    auto res = cli.Get("/detected_objects");
    EXPECT_TRUE(res->body == detectedobjects2json(dummyObjs));
}

TEST(HTTP_API, Stop_Server) {
    // Expect a confirmation and the server to stop
    httplib::Client cli("0.0.0.0:8080");
    auto res = cli.Get("/stop");
    EXPECT_TRUE(res->body == "Closing Server");
}