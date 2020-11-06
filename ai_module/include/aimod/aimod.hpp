#ifndef __AI_MODULE__
#define __AI_MODULE__
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

class AI {
    private:
        bool recording;
        std::string dir_name;
        cv::VideoWriter out_vid;
        Detector* darknet;
public:
        AI(std::string input_path, bool record=false, std::string output_path=empty); //Loads Darkhelp
        ~AI(); //Destruct AI object
        std::vector<DetectedObject> detect(Video_Frame &frame, float minimum_confidence); //Detects object within a frame
        std::vector<DetectedObject> detect(cv::Mat &frame, float minimum_confidence);
        void close(); //Closes AI, if recording, loads a CV video
};
#endif