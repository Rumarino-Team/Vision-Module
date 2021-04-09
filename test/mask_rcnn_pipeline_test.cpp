#include <iostream>
#include <chrono>
#include <mutex>
#include <atomic>
#include <pthread.h>
#include <condition_variable>
#include <gtest/gtest.h>
#include "zedmod/zedmod.hpp"
#include "darknetmod/darknetmod.hpp"
#include "maskrcnnmod/maskrcnnmod.hpp"
#include "visualobjmod/visualobjmod.hpp"
#include "tools.hpp"

std::string in_video_2 = "media/test_input_video.svo";

std::string yolo_model_input_path = "media/RUBBER-DUCKY";
std::string mask_rcnn_input_path = "media/MASK-RCNN";

// TEST(Darknet, File_Checker) {
//     std::vector <std::string> tokens;
//     std::stringstream charToString(yolo_model_input_path);
//     std::string intermediate;
//     while(getline(charToString, intermediate, '/'))
//         if(!intermediate.empty() && intermediate != " ")
//             tokens.push_back(intermediate);
//     std::string dir_name = tokens.back();

//     std::string yolo_cfg = yolo_model_input_path + "/" + dir_name + ".cfg";
//     std::string yolo_weights = yolo_model_input_path + "/weights/" + dir_name + "_best.weights";
//     std::string yolo_names = yolo_model_input_path + "/custom.names";

//     EXPECT_TRUE(file_exists(cfg.c_str()));
//     EXPECT_TRUE(file_exists(weights.c_str()));
//     EXPECT_TRUE(file_exists(names.c_str()));
// }

TEST(MaskRCNN, File_Checker) {
    std::vector <std::string> tokens;
    std::stringstream charToString(mask_rcnn_input_path);
    std::string intermediate;
    while(getline(charToString, intermediate, '/'))
        if(!intermediate.empty() && intermediate != " ")
            tokens.push_back(intermediate);
    std::string dir_name = tokens.back();

    std::string mask_rcnn_graph = mask_rcnn_input_path + "/" + dir_name + ".pbtxt";
    std::string mask_rcnn_weights = mask_rcnn_input_path + "/" + dir_name + ".pb";;
    std::string mask_rcnn_names = mask_rcnn_input_path + "/" + dir_name + ".names";;
    std::string mask_rcnn_colors = mask_rcnn_input_path + "/" + dir_name + ".colors";

    EXPECT_TRUE(file_exists(mask_rcnn_graph.c_str()));
    EXPECT_TRUE(file_exists(mask_rcnn_weights.c_str()));
    EXPECT_TRUE(file_exists(mask_rcnn_names.c_str()));
    EXPECT_TRUE(file_exists(mask_rcnn_colors.c_str()));
}

TEST(Pipeline, MaskRCNN_Masking) {
    // Create the darknet pipeline module
    DarknetModule darkMod(yolo_model_input_path, 0.60);
    MaskRCNNModule maskRCNNMod(mask_rcnn_input_path, 0.5, 0.3);


    // Create the pipeline manager
    Pipeline pipeline = {&darkMod, &maskRCNNMod};
    PipelineManager ai(pipeline, false);

    cv::VideoCapture cap("media/test_video.mp4");
    EXPECT_TRUE(cap.isOpened());

    while(true) {
        Video_Frame frame;
        cv::Mat img;
        if (cap.read(img)) {
            frame.image = img;
            auto obj = ai.detect(frame);
        } else {
            break;
        }
    }
}

// Declare thread functions
// The multithreading here is simple and technically linear
// But since where only testing the AI part this is ok
// Multithreading is in response to ZED and Darknet not being able to run in the same thread
Video_Frame threaded_frame_2;
std::mutex frame_mutex_2;
std::condition_variable new_frame_2;
std::condition_variable new_detect_2;


void camera_stream_2(ZED_Camera *cam, std::atomic<bool> &running) {
    while(running) {
        auto start = std::chrono::high_resolution_clock::now();

        std::unique_lock<std::mutex> frame_lock(frame_mutex_2);
        threaded_frame_2 = cam->update();
        frame_lock.unlock();
        new_frame_2.notify_one();

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        int zed_time_per_frame = duration.count();
        std::cout << "Camera stream is going at " << 1000000/zed_time_per_frame << " fps." << std::endl;
    }
}

TEST(Pipeline, Mask_RCNN_Multi_Threading) {
    ZED_Camera cam(in_video_2);
    std::string out_video = "media/ai_output.avi";

    Pipeline pipeline;

    // Create the darknet pipeline module
    float confidence = 0.60;
    pipeline.push_back(new DarknetModule(yolo_model_input_path, confidence));

    // // Create the image writing pipeline module
    // pipeline.push_back(new VisualObjModule());

    // Add MaskRCNN to pipeline
    float confidence_threshold = 0.5;
    float mask_threshold = 0.3;
    pipeline.push_back(new MaskRCNNModule(mask_rcnn_input_path, confidence_threshold, mask_threshold));

    PipelineManager ai(pipeline, true, out_video);

    static std::atomic<bool> running = true;
    std::thread camera_thread(camera_stream_2, &cam, std::ref(running));

    for (int i=0; i < 500; i++) {

        auto start = std::chrono::high_resolution_clock::now();

        std::unique_lock<std::mutex> frame_lock(frame_mutex_2);
        new_frame_2.wait(frame_lock);
        DetectedObjects threaded_obj = ai.detect(threaded_frame_2);
        frame_lock.unlock();

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        int ai_time_per_frame = duration.count();
        std::cout << "AI detection is going at " << 1000000/ai_time_per_frame << " fps." << std::endl;

        std::cout << "Objects detected at frame " << i << " \n{" <<std::endl;
        for (DetectedObject &obj : threaded_obj) {
            std::cout << "\tBounding box: ("<<obj.bounding_box.x<<", "<<obj.bounding_box.y<<") w: "<<obj.bounding_box.width<<" h: " <<obj.bounding_box.height<< std::endl;
            std::cout << "\tObject ID: "<< obj.id << std::endl;
            std::cout << "\tObject Name: " << obj.name << std::endl;
            std::cout << "\tDistance: " << obj.distance << std::endl;
            std::cout << "\tLocation: (" << obj.location.x << ", " << obj.location.y << ", " << obj.location.z << " )\n" << std::endl;
            //EXPECT_TRUE();
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