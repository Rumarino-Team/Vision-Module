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
        sl::Camera zed;
        bool recording;
    public:
        ZED_Camera(bool record, const char* playback_video, const char* recording_out, sl::RESOLUTION res, int fps);
        Video_Frame update(); //Gets new frame
        void close(); //Closes camera object
};
#endif