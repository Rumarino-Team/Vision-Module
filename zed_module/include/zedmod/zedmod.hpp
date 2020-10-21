#ifndef __ZED_MODULE__
#define __ZED_MODULE__
#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>
struct Video_Frame {
    //https://github.com/stereolabs/zed-opencv/blob/master/cpp/src/main.cpp provides a sl::Mat to cv::Mat
    //We want to use the cv::Mat to further avoid using ZED outside this module
    cv::Mat image;
    cv::Mat depth_map; //sl::Mat -> cv::Mat will contain 32bit floats
};

class ZED_Camera {
    private:
        Camera zed;
    public:
        ZED_Camera(bool record); //Inits zed, has ability to record
        Video_Frame update(); //Gets new frame
        char* close(); //Closes camera object and returns save path if recording
};
#endif