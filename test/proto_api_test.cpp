#include <gtest/gtest.h>
#include "protobufapimod/apimod.hpp"


DetectedObjects dummies;
DetectedObject obj1;
DetectedObject obj2;
std::string output;
//proto proto_tester = proto(output);

TEST(PROTO_API, getting_objects) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    obj1.bounding_box = cv::Rect(1, 2, 3, 4);
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

    dummies.push_back(obj1);
    dummies.push_back(obj2);
}

//TEST(PROTO_API, check_frame) {
//    prototype.set_frame(&image1, &dummies[0]);
//    ASSERT_STREQ(image1.name, "one");
//    EXPECT_TRUE(image1.rect.x == 1);
//}
//
//TEST(PROTO_API, send_frames) {
//    prototype.send(dummies, "mypipe");
//}



