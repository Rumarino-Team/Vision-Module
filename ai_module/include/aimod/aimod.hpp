#ifndef __AI_MODULE__
#define __AI_MODULE__
#include <DarkHelp.hpp>
#include <opencv2/opencv.hpp>
#include "zedmod/zedmod.hpp"
struct DetectedObject { //Built from DarkHelp::PredictionResult
    cv::Rect bounding_box;
    int obj_id;
    const char* obj_name;
    float distance;
};

class AI {
    private:
        bool recording;
        std::string in_path;
        std::string out_path;
        std::string dir_name;
        std::string cfg;
        std::string weights;
        std::string names;
        int save_count;
    public:
        AI(bool record, std::string input_path, std::string output_path); //Loads Darkhelp
        DetectedObject detect(Video_Frame frame); //Detects object within a frame
        std::string close(); //Closes AI, if recording, loads a CV video
};
#endif