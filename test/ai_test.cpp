#include <iostream>
#include <chrono>
#include <mutex>
#include <atomic>
#include <pthread.h>
#include <condition_variable>
#include <gtest/gtest.h>
#include "zedmod/zedmod.hpp"
#include "aimod/aimod.hpp"
//#include "tools.hpp"

bool file_exists_ai(const char* file) {
    struct stat buffer;
    return (stat (file, &buffer) == 0);
}

std::string in_video = "media/test_input_video.svo";

std::string input_path = "media/RUBBER-DUCKY";

TEST(AI, File_Checker) {
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

TEST(AI, Video_Detection) {
    AI ai(input_path);

    cv::VideoCapture cap("media/test_video.mp4");
    EXPECT_TRUE(cap.isOpened());

    while(true) {
        cv::Mat img;
        if (cap.read(img)) {
            auto obj = ai.detect(img, 0.6);
        } else {
            break;
        }
    }

}

// Declare thread functions
// The multithreading here is simple and technically linear
// But since where only testing the AI part this is ok
// Multithreading is in response to ZED and Darknet not being able to run in the same thread
Video_Frame threaded_frame;
std::mutex frame_mutex;
std::condition_variable new_frame;
std::condition_variable new_detect;


void camera_stream(ZED_Camera *cam, std::atomic<bool> &running) {
    while(running) {
        auto start = std::chrono::high_resolution_clock::now();

        std::unique_lock<std::mutex> frame_lock(frame_mutex);
        threaded_frame = cam->update();
        frame_lock.unlock();
        new_frame.notify_one();

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        int zed_time_per_frame = duration.count();
        std::cout << "Camera stream is going at " << 1000000/zed_time_per_frame << " fps." << std::endl;
    }
}

TEST(AI, Multi_Threading) {
    ZED_Camera cam(in_video);
    AI ai(input_path, true, "media/ai_output.avi");
    float minimum_confidence = 0.60;

    static std::atomic<bool> running { true };
    std::thread camera_thread(camera_stream, &cam, std::ref(running));

    for (int i=0; i < 30; i++) {

        auto start = std::chrono::high_resolution_clock::now();

        std::unique_lock<std::mutex> frame_lock(frame_mutex);
        new_frame.wait(frame_lock);
        std::vector<DetectedObject> threaded_obj = ai.detect(threaded_frame, minimum_confidence);
        frame_lock.unlock();

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        int ai_time_per_frame = duration.count();
        std::cout << "AI detection is going at " << 1000000/ai_time_per_frame << " fps." << std::endl;

        std::cout << "Objects detected at frame " << i << " \n{" <<std::endl;
        for (DetectedObject obj : threaded_obj) {
            std::cout << "\tBounding box: ("<<obj.bounding_box.x<<", "<<obj.bounding_box.y<<") w: "<<obj.bounding_box.width<<" h: " <<obj.bounding_box.height<< std::endl;
            std::cout << "\tObject ID: "<< obj.id << std::endl;
            std::cout << "\tObject Name: " << obj.name << std::endl;
            std::cout << "\tDistance: " << obj.distance << std::endl;
            std::cout << "\tLocation: (" << obj.location.x << ", " << obj.location.y << ", " << obj.location.z << " )\n" << std::endl;
            //EXPECT_TRUE(); avoid getting nan in distance, empty string names and other irregularities like those
        }
        std::cout << "}" << std::endl;
    }
    //Close the threads
    running = false;
    camera_thread.join();
    // Close the objects
    ai.close();
    cam.close();
}

