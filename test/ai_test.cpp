#include <iostream>
#include <chrono>
#include <gtest/gtest.h>
#include "zedmod/zedmod.hpp"
#include "aimod/aimod.hpp"
//#include "tools.hpp"

bool file_exists_ai(const char* file) {
    struct stat buffer;
    return (stat (file, &buffer) == 0);
}

std::string in_video("media/test_input_video.svo");
//std::string playback_video_ai = "media/test_input_video.svo";

//CHANGE THE INPUT PATH TO WHERE YOU HAVE THE RUBBER-DUCKY FOLDER!
std::string input_path = "/home/hectormiranda/Downloads/RUBBER-DUCKY";
std::string out_path = "media/ai_output";

TEST(AI, File_Checker){
    std::vector <std::string> tokens;
    std::stringstream charToString(input_path);
    std::string intermediate;
    while(getline(charToString, intermediate, '/'))
        if(!intermediate.empty() && intermediate != " ")
            tokens.push_back(intermediate);
    std::string dir_name = tokens.back();

    std::string cfg = input_path + + "/" + dir_name + ".cfg";
    std::string weights = input_path + "/weights/" + dir_name + "_best.weights";
    std::string names = input_path + "/custom.names";

    EXPECT_TRUE(file_exists_ai(cfg.c_str()));
    EXPECT_TRUE(file_exists_ai(weights.c_str()));
    EXPECT_TRUE(file_exists_ai(names.c_str()));
}

TEST(AI, Run_Video) {
    ZED_Camera cam(in_video);
    AI ai(true, input_path, out_path);
    float minimum_confidence = 0.60;

    int exec_time = 0;
    for (int i=0; i < 30; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        Video_Frame frame = cam.update();
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        std::cout << "Duration in frame " << i << " " << duration.count() << " microseconds." << std::endl;
        exec_time += duration.count();

        //The following can NOT be empty, otherwise the AI will go loco
        ASSERT_FALSE(frame.image.empty());
        ASSERT_FALSE(frame.depth_map.empty());
        ASSERT_FALSE(frame.point_cloud.empty());

        //ai.detect(frame, minimum_confidence);
    }
    std::cout << "30 frames took " << exec_time * 0.000001 << "seconds." << std::endl;
    //ai.close();
    cam.close();
}

