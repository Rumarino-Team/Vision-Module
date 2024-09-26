#ifndef __ZED_MODULE__
#define __ZED_MODULE__
#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>

/**
 * To prevent using the Zed API outside the ZED_Camera, everything is converted to OpenCV format
 */
struct Video_Frame {
    /**
     * Initialize empty frame
     */
    Video_Frame();
    /**
     * Initialize frame by copying the images
     *
     * @param image
     */
    Video_Frame(cv::Mat &image);
    /**
     * Initialize frame by copying another frame
     *
     * @param frame
     */
    Video_Frame(Video_Frame &frame);
    /**
     * Copy over the Frame's values to this one.
     *
     * @param frame
     */
    void copy(Video_Frame &frame);
    // Original left camera frame
    cv::Mat image;
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

        
        // Actually initiates the object
        void init(bool record, const std::string &playback_video, const std::string &recording_out, sl::RESOLUTION res, int fps);
    public:

        /**
         * Setups up all the camera runtime variables and begins the live video stream.</>Uses this module's video quality setter.
         *
         * @param record Whether to record the stream or not
         * @param recording_out Name of the video to be saved if recording
         * @param quality Video resolution along with framerate
         */
        ZED_Camera(bool record, const std::string &recording_out = empty, Video_Quality quality = Video_Quality::HD1080_30fps);

        /**
         * Setups up all the camera runtime variables and begins the live video stream.</>Uses zed's video quality setter.
         *
         * @param record Whether to record the stream or not
         * @param res Video resolution
         * @param fps Video framerate
         * @param recording_out Name of the video to be saved if recording
         *
         * @overload
         */
        ZED_Camera(bool record, sl::RESOLUTION res, int fps, const std::string &recording_out = empty);

        /**
         * Setups up all the camera runtime variables and begins the pre-recorded video stream.
         *
         * @param playback_video Video to serve as a stream
         *
         * @overload
         */
        ZED_Camera(const std::string &playback_video);



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
        *This method return the Objects vector of the ObjectData type from the Zed SDK in the
        * Object detection AI module.
        */
        sl::Objects Zed_Inference(sl::CustomBoxObjectData objs);

        /*
         * Closes the video stream and recording
         */
        void close();
};

/**
 * Turns the given svo video into a collection of images.
 *
 * @param playback_video The video to be played back
 * @param image_save_path Where the stripped images will be stored
 */
void svo2img(const std::string &playback_video, const std::string &image_save_path);
#endif