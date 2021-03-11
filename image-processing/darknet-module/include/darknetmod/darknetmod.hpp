#ifndef __DARKNET_MODULE__
#define __DARKNET_MODULE__
#include <experimental/filesystem>
#include <yolo_v2_class.hpp>
#include "pipelinemod/pipelinemod.hpp"


class DarknetModule : public PipelineModule {
public:
    /**
     * Reads all the YOLO files and initializes the model.
     *
     * @param input_path The pre-trained YOLO model folder
     */
    DarknetModule(std::string input_path, float minimum_confidence);

    /**
     * Detects the objects in the image using Darknet
     *
     * @param frame The video image  frame
     * @param objs Objects array
     * @return Possible error is returned
     */
     PipelineErrors detect(Video_Frame &frame, DetectedObjects &objs) override;
private:
    // The object names
    std::vector<std::string> names;
    // The initialized darknet detector
    Detector* darknet;
    // Model confidence
    float confidence;
};
#endif