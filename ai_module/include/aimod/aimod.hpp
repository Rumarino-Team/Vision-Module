#ifndef __AI_MODULE__
#define __AI_MODULE__
#include <filesystem>
#include <opencv2/opencv.hpp>

// We have to include the Zed module
#include <"zedmod/zedmod.hpp">

// This is the file from the Detector of Yolov7
#include<"yolov7.h">


// This variable is used for storing the CustomDetectedObjects that
// is transfer to the zed.ingestinference(). This vector is updated
// every grab of the camera.
using CustomDetectedObjects = std::vector<CustomBoxObjectData>;


// We created a new Class that inherits from the yolov7 detector to redifined
// the method DrawResults.
class Yolov7 : yolov7{
public:
void DrawResults(const std::vector<ClassRes> &results, std::vector<cv::Mat> &vec_img,
                     std::vector<std::string> image_names);

}


class AI {
    private:
        // Recording state
        bool recording;
        // The output video
        cv::VideoWriter out_vid;
        // The initialized Yolov7 detector
        yolov7* Yolov7;

        /**
         * Internal method for detecting objects from an image using yolov7.
         *
         * @param frame The input image
         * @return DetectedObjects where each DetectedObject only contains bounding_box, id and name
         */
        CustomBoxObjectData detect_objects(cv::Mat &frame); //Detects the object
public:
        /**
         * Reads all the YOLO files and initializes the model.</> It also starts the recording.
         *
         * @param input_yaml The yaml config
         * @param record Whether to record the stream or not
         * @param output_path Name of the video to be saved if recording
         * @param fps The framerate at which the video playback would be at.
         */
        AI(std::string input_yaml, bool record=false, std::string output_path=empty, int fps=15);

        /**
         * Calls the destructor and the close() method
         */
        ~AI();
        /**
         * Detects objects.
         *
         * @param frame The input image
         * @return CustomBoxObjectData where each DetectedObject only contains bounding_box, id and name
         */
        CustomBoxObjectData detect(Video_Frame &frame);

        /*
         * Closes the AI, and if recording, closes the camera stream.
         */
        void close();
};
#endif