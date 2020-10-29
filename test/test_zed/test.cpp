#include <sys/stat.h>
#include <iostream>
#include <gtest/gtest.h>
#include "zedmod/zedmod.hpp"

bool file_exists(const char* file) {
    struct stat buffer;
    return (stat (file, &buffer) == 0);
}

std::string playback_video("media/test_input_video.svo");

TEST(ZED, Run_Video) {
    ZED_Camera cam(false, playback_video);
    for (int i=0; i < 10; i++) {
        Video_Frame frame = cam.update();
        EXPECT_FALSE(frame.image.empty());
        EXPECT_FALSE(frame.depth_map.empty());
    }
    cam.close();
}

std::string video_save("media/recording_out.svo");
ZED_Camera cam(true, "", video_save);

TEST(ZED, Live_video) {
    for (int i=0; i < 10; i++) {
        Video_Frame frame = cam.update();
        EXPECT_FALSE(frame.image.empty());
        EXPECT_FALSE(frame.depth_map.empty());
    }
    cam.close();
}

TEST(ZED, Save_Video) {
    EXPECT_TRUE(file_exists(video_save.c_str()));
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}