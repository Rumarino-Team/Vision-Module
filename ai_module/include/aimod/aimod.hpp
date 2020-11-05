#ifndef __AI_MODULE__
#define __AI_MODULE__
#include <yolo_v2_class.hpp>
#include <opencv2/opencv.hpp>
#include "zedmod/zedmod.hpp"
struct DetectedObject { //Built from DarkHelp::PredictionResult
    cv::Rect bounding_box;
    int obj_id;
    const char* obj_name;
    float distance;
    cv::Point3f point_3d;
};

class AI {
    private:
        bool recording;
        std::string out_path;
        std::string dir_name;
        cv::VideoWriter out_vid;
        Detector* darknet;
public:
        AI(bool record, std::string input_path, std::string output_path); //Loads Darkhelp
        ~AI(); //Destruct AI object
        std::vector<DetectedObject> detect(Video_Frame frame, float minimum_confidence); //Detects object within a frame
        void close(); //Closes AI, if recording, loads a CV video
};
#endif