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

TEST(File_Checker, MaskRCNN) {
    std::vector <std::string> tokens;
    std::stringstream charToString(mask_rcnn_input_path);
    std::string intermediate;
    while(getline(charToString, intermediate, '/'))
        if(!intermediate.empty() && intermediate != " ")
            tokens.push_back(intermediate);
    std::string dir_name = tokens.back();

    std::string mask_rcnn_graph = mask_rcnn_input_path + "/" + dir_name + ".pbtxt";
    std::string mask_rcnn_weights = mask_rcnn_input_path + "/" + dir_name + ".pb";
    std::string mask_rcnn_names = mask_rcnn_input_path + "/" + dir_name + ".names";
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

    std::string out_video = "media/rcnn_output.avi";

    // Create the pipeline manager
    Pipeline pipeline = {&darkMod, &maskRCNNMod};
    PipelineManager ai(pipeline, true, out_video);

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

TEST(Pipeline, MaskRCNN_Module) {
    ZED_Camera cam(in_video_2);
    std::string out_video = "media/rcnn_multithreaded_output.avi";

    // Add MaskRCNN to pipeline
    float confidence_threshold = 0.5;
    MaskRCNNModule maskRCNNMod(mask_rcnn_input_path, confidence_threshold);

    Pipeline pipeline = {&maskRCNNMod};
    pipeline.push_back(new VisualObjModule());

    PipelineManager ai(pipeline, true, out_video);

    for (int i=0; i < 30; i++) {

        auto start = std::chrono::high_resolution_clock::now();

        Video_Frame frame = cam.update();
        DetectedObjects threaded_obj = ai.detect(frame);
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
        }
        std::cout << "}" << std::endl;
    }
    // Close the objects
    ai.close();
    cam.close();
}