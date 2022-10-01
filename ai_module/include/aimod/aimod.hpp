#ifndef __AI_MODULE__
#define __AI_MODULE__
#include <experimental/filesystem>
#include <opencv2/opencv.hpp>

// We have to include the Zed Library
// beacuse we're going to use the CustomBoxObjectData
#include <"zedmod/zedmod.hpp">

// This is the Detector of Yolov7
//https://github.com/linghu8812/tensorrt_inference Dowload here
#include<"yolov7.h">



/**

 * Better Solution:
 * Also we can just use the DetectRes data directly without  using this class. After all
 * what we're going to return in the AI class will be  a CustomBoxObjectData. Deleting this class will be optimal.
 */


/**
 * Change:
 * We are going to change  the type of the vector from DetectedObject
 * to CustomBoxObjectData.
 */
using CustomDetectedObjects = std::vector<CustomBoxObjectData>;
/**
 * Object Detection and Segmentation wrapper
 */

class Yolov7 : yolov7{
public:
void DrawResults(const std::vector<ClassRes> &results, std::vector<cv::Mat> &vec_img,
                     std::vector<std::string> image_names);

}


class AI {
    private:
        // Recording state
        bool recording;
        // The object names
        std::vector<std::string> names;
        // The output video
        cv::VideoWriter out_vid;
        // The initialized Yolov7 detector
        yolov7* Yolov7;

        /**
         * Internal method for detecting objects from an image using darknet.</>Used by the detect() methods.
         *
         * @param frame The input image
         * @param minimum_confidence Minimum object confidence
         * @return DetectedObjects where each DetectedObject only contains bounding_box, id and name
         */
        CustomBoxObjectData detect_objects(cv::Mat &frame, float minimum_confidence); //Detects the object
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
// Important Questions:
// 1. The Zed Camera is calibrated for underwater??
// 2. Does anyone know about Multithreating?? Mutex Objects  lock and unlocking?? Resolve
// 3. Does making multithreating mutex make difference compare with single threads?? || Yeah but depends

//Interesting:


// Multithreathing:
// 1. Make the TODO ->//TODO: If variable hasn't updated then keep the old json



// Steps:
// 1. change the Detector for Yolov7
// 2. Make a function that takes the detection and return a CustomBoundingBox
// 3. Make a Function in the Zed Camera that takes a CustomBoundingBox and return the Zed Objects
// 4. Make a solution for the communication. Zed_Camera -> AI -> Zed_Camera -> API with Multithreating. Need help in this.
// 5. In the API take the Object and extract all the information that we want to extract.

//Yolo Detection:
//https://github.com/linghu8812/tensorrt_inference
//1. What return the YoloDetector is the class  DetectRes that have a parameter name detection_result of type vector<Bbox>. Bbox is a struct with the following parameters:
// float prop-> probability, int class - the class of the object, float x ->  x position, float y -> y position, float w -> width 
//and float h -> hegiht.

//2. The Yolo Detector have already






// I think that the best we can do is to give to the API the Objects from the Zed module directly.
// This will allow us to free us from communications problems in the executable file.
// Instead of Zed_Camera -> AI - > ZED_Camera -> AI -> API we can do Zed_Camera -> AI -> Zed_Camera -> API
// This will mean that we will not include all the information in DetectedObjects class. Instead 
// DetectedObjects will be a placeholder for giving the Information to the Zed Module


// After reading about multithreathing all execute with a Client Server architecture using conditional variables
// for the task. The AI stream wait for the frame mutex with a conditional_variable that use wait. For aclaration
// wait is a function that wait for a notification that the code can now run. Wait unlock the pass mutex so that
// it can prevent a deadlock, a set to work the other threads.


// Solution!!
// The solution is that we are going to pass the Zed_Camera to the AI_Stream and calculate from there the Objects.
// The code has already implemented a good system for waiting for the data to appear. The API does not use a conditional variable.
// Because the only thing it does is reading the data not modifying it.


//Objects Retrieval from Zed camera:
// We must pass the 