#ifndef __AI_MODULE__
#define __AI_MODULE__
#include <experimental/filesystem>
#include <yolo_v2_class.hpp>
#include <opencv2/opencv.hpp>
#include "zedmod/zedmod.hpp"

struct DetectedObject { //Built from DarkHelp::PredictionResult
    cv::Rect bounding_box;
    int id;
    const char* name;
    float distance;
    cv::Point3f location;
};

using DetectedObjects = std::vector<DetectedObject>;

class AI {
    private:
        bool recording;
        std::vector<std::string>names;
        cv::VideoWriter out_vid;
        Detector* darknet;
public:
        AI(std::string input_path, bool record=false, std::string output_path=empty); //Loads Darkhelp
        ~AI(); //Destruct AI object
        DetectedObjects detect(Video_Frame &frame, float minimum_confidence); //Detects object within a frame
        DetectedObjects detect(cv::Mat &frame, float minimum_confidence);
        void close(); //Closes AI, if recording, loads a CV video
};
#endif