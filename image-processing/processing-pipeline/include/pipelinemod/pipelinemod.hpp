#ifndef __PIPELINE_MODULE__
#define __PIPELINE_MODULE__
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

enum PipelineErrors {
    None,
    NotImplemented,
};

class PipelineModule {
public:
    /**
     * Default module constructor
     *
     * @param name The module name
     */
    explicit PipelineModule(const char* name);

    /**
     * Detector or image enhancer
     *
     * @param frame The video image  frame
     * @param objs Objects array
     * @return Possible error is returned
     */
    virtual PipelineErrors detect(Video_Frame &frame, DetectedObjects &objs);
protected:
    const char* name;
};

/**
 * Is just an array containing multiple pipelines
 */
using Pipeline = std::vector<PipelineModule*>;

class PipelineManager {
public:
    /**
     * Initialized the video recorded and stores the pipeline modules
     *
     * @param pipeline A vector array of the modules in order of usage
     * @param record Whether to record or not
     * @param output_path Where the recorded video will be stores
     * @param fps The recorded video framerate
     */
    explicit PipelineManager(Pipeline &pipeline, bool record,
                             const std::string& output_path=empty, int fps=15);

    /**
     * Runs the pipeline order and saves the frame
     *
     * @param frame The frame to analyze
     * @return Detected objects
     */
    DetectedObjects detect(Video_Frame &frame);

    /**
     * Closes the recording stream
     */
    void close();
private:
    bool recording;
    cv::VideoWriter out_vid;
    Pipeline pipeline;
};

#endif