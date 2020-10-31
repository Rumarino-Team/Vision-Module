#ifndef __ZED_MODULE__
#define __ZED_MODULE__
#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>
struct Video_Frame {
    //https://github.com/stereolabs/zed-opencv/blob/master/cpp/src/main.cpp provides a sl::Mat to cv::Mat
    //We want to use the cv::Mat to further avoid using ZED outside this module
    cv::Mat image;
    cv::Mat depth_map; //sl::Mat -> cv::Mat will contain 32bit floats
    cv::Mat point_cloud; //Each pixel contains [x,y,z,NULL]
};

const std::string empty = std::string();

class ZED_Camera {
    private:
        sl::Camera zed;
        bool recording;
    public:
        ZED_Camera(bool record, const std::string &playback_video = empty, const std::string &recording_out = empty, sl::RESOLUTION res = sl::RESOLUTION::HD1080, int fps = 30);
        ~ZED_Camera();
        Video_Frame update(); //Gets new frame
        void close(); //Closes camera object
};
#endif