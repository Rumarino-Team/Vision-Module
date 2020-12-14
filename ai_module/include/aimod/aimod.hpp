#ifndef __AI_MODULE__
#define __AI_MODULE__
#include <experimental/filesystem>
#include <yolo_v2_class.hpp>
#include <opencv2/opencv.hpp>
#include "zedmod/zedmod.hpp"

/**
 * Contains all object information
 */
struct DetectedObject {
    // The location of the object in relation to the image
    cv::Rect bounding_box;
    // The object id according to the object detector
    int id;
    // The object name that matches with the given id
    const char* name;
    // The distance from the camera
    float distance;
    // The relative 3D location from the camera
    cv::Point3f location;
};

/**
 * Is just an array containing many DetectedObject objects
 */
using DetectedObjects = std::vector<DetectedObject>;

/**
 * Object Detection and Segmentation wrapper
 */
class AI {
    private:
        // Recording state
        bool recording;
        // The object names
        std::vector<std::string> names;
        // The output video
        cv::VideoWriter out_vid;
        // The initialized darknet detector
        Detector* darknet;

        /**
         * Internal method for detecting objects from an image using darknet.</>Used by the detect() methods.
         *
         * @param frame The input image
         * @param minimum_confidence Minimum object confidence
         * @return DetectedObjects where each DetectedObject only contains bounding_box, id and name
         */
        DetectedObjects detect_objects(cv::Mat &frame, float minimum_confidence); //Detects the object
public:
        /**
         * Reads all the YOLO files and initializes the model.</> It also starts the recording.
         *
         * @param input_path The pre-trained YOLO model folder
         * @param record Whether to record the stream or not
         * @param output_path Name of the video to be saved if recording
         * @param fps The framerate at which the video playback would be at.
         */
        AI(std::string input_path, bool record=false, std::string output_path=empty, int fps=15);

        /**
         * Calls the destructor and the close() method
         */
        ~AI();

        /**
         * Detects objects and gives their approximate distance and 3D location.
         *
         * @param frame The input frame
         * @param minimum_confidence Minimum object confidence
         * @return Detected objects and with location in relation to the image and 3D space in relation to the camera, object distance, name and id.
         */
        DetectedObjects detect(Video_Frame &frame, float minimum_confidence);

        /**
         * Detects objects.
         *
         * @param frame The input image
         * @param minimum_confidence Minimum object confidence
         * @return DetectedObjects where each DetectedObject only contains bounding_box, id and name
         */
        DetectedObjects detect(cv::Mat &frame, float minimum_confidence);

        /*
         * Closes the AI, and if recording, closes the camera stream.
         */
        void close();
};
#endif