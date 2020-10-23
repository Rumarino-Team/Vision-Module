#ifndef __AI_MODULE__
#define __AI_MODULE__
#include <DarkHelp.hpp>
#include <opencv2/opencv.hpp>
struct Object { //Built from DarkHelp::PredictionResult
    cv::Rect bounding_box;
    int obj_id;
    float distance;
};
#endif