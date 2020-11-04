#include <sys/stat.h>
#include <iostream>
#include <chrono>
#include <gtest/gtest.h>
#include "zedmod/zedmod.hpp"
//#include "tools.hpp"

bool file_exists(const char* file) {
    struct stat buffer;
    return (stat (file, &buffer) == 0);
}

std::string playback_video("media/test_input_video.svo");

TEST(ZED, Run_Video) {
    ZED_Camera cam(playback_video);
    int exec_time = 0;
    for (int i=0; i < 30; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        Video_Frame frame = cam.update();
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout << "Duration in frame " << i << " " << duration.count() << " microseconds." << std::endl;
        exec_time += duration.count();
        EXPECT_FALSE(frame.image.empty());
        EXPECT_FALSE(frame.depth_map.empty());
        EXPECT_FALSE(frame.point_cloud.empty());
    }
    std::cout << "30 frames took " << exec_time * 0.000001 << "seconds." << std::endl;
    cam.close();
}

std::string video_save("media/recording_out.svo");
ZED_Camera cam(true, video_save, Video_Quality::HD1080_30fps);

TEST(ZED, Live_video) {
    int exec_time = 0;
    for (int i=0; i < 30; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        Video_Frame frame = cam.update();
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout << "Duration in frame " << i << " " << duration.count() << " microseconds." << std::endl;
        exec_time += duration.count();
        EXPECT_FALSE(frame.image.empty());
        EXPECT_FALSE(frame.depth_map.empty());
        EXPECT_FALSE(frame.point_cloud.empty());
    }
    std::cout << "30 frames took " << exec_time * 0.000001 << " seconds." << std::endl;
    cam.close();
}

TEST(ZED, Save_Video) {
    EXPECT_TRUE(file_exists(video_save.c_str()));
}

TEST(ZED, DISABLE_Save_Images) {
    svo2img(playback_video, "media/");
}