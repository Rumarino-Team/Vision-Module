#ifndef __MASKRCNN_MODULE__
#define __MASKRCNN_MODULE__
#include <experimental/filesystem>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string.h>

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "pipelinemod/pipelinemod.hpp"

class MaskRCNNModule : public PipelineModule {
public:
    /**
     * Reads the model and initializes the model.
     *
     * @param input_path The path to folder with the model, model graph and names and colors file
     * @param confidence_threshold The confidence threshold
     * @param mask_threshold The mask threshold
     */
    MaskRCNNModule(std::string input_path, float minimum_confidence, float minimum_mask);

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
    // The colors
    std::vector<cv::Scalar> colors;
    // Network
    cv::dnn::Net maskrcnn;
    // Model confidence
    float confidence_threshold;
    // Model mask threshold
    float mask_threshold;
};
#endif