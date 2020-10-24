#include <gtest/gtest.h>
#include "zedmod/zedmod.hpp"

TEST(ZED_Object, Run_Video) {
    ZED_Camera cam(false, "media/test_input_video.svo");
    for (int i=0; i < 10; i++) {
        Video_Frame frame = cam.update();
        EXPECT_FALSE(frame.image.empty());
        EXPECT_FALSE(frame.depth_map.empty());
    }
    cam.close();
}

//TODO: Check real video stream
//TODO: Using this video stream record a video for evaluation
//TODO: Assert that the file is really there

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}