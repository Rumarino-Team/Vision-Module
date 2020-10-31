#ifndef __ZED_MODULE__
#define __ZED_MODULE__
#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>

/**
 * To prevent using the Zed API outside the ZED_Camera, everything is converted to OpenCV format
 */
struct Video_Frame {
    // Original left camera frame
    cv::Mat image;
    // Each pixel contains a float32 determining pixel distance
    cv::Mat depth_map;
    // Each pixel contains [x,y,z,NULL]
    cv::Mat point_cloud;
};

/**
 * Camera resolution and framerate.</>
 *
 * Every item follows the same format: videoQuality_framerate
 */
enum class Video_Quality {
    HD2K_15fps,
    HD1080_15fps,
    HD1080_30fps,
    HD720_15fps,
    HD720_30fps,
    HD720_60fps,
    VGA_15fps,
    VGA_30fps,
    VGA_60fps,
    VGA_100fps
};

const std::string empty = std::string();

/**
 * Zed API wrapper
 */
class ZED_Camera {
    private:
        // Camera driver
        sl::Camera zed;
        // Recording state
        bool recording;
    public:
        /**
         * Setups up all the camera runtime variables and begins the video stream
         * NOTE: When playing a video it will continue to run even when not calling update()
         *
         * @param record Whether to record the stream or not, when playing back recording will not work
         * @param playback_video Video to be played back, when left empty it will try to open a stream to a connected Zed camera
         * @param recording_out Name of the video to be saved if recording
         * @param res Resolution to be picked up
         * @param fps Framerate
         */
        ZED_Camera(bool record, const std::string &playback_video = empty, const std::string &recording_out = empty, sl::RESOLUTION res = sl::RESOLUTION::HD1080, int fps = 30);

        /**
         * Will just call the cameras destructor and call close()
         */
        ~ZED_Camera();

        /*
         * Gets the current frame from the video stream, when recording will also save this frame
         *
         * @returns The current frame information: Left camera image, depth map and point cloud
         */
        Video_Frame update();

        /*
         * Closes the video stream and recording
         */
        void close();
};
#endif