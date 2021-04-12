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

/**
 * Will subdivide the image into regions to find the center
 *
 * @param mask The image mask
 * @return A point for the object center
 */
cv::Point2i getCenter(cv::Mat& mask);

class MaskRCNNModule : public PipelineModule {
public:
    typedef std::function<cv::Point2i(cv::Mat&)> MaskRCNNFunction;
    /**
     * Reads the model and initializes the model.
     *
     * @param input_path The path to folder with the model, model graph and names and colors file
     * @param confidence_threshold The confidence threshold
     * @param calculate_location Use the mask information to calculate object distance and 3D location
     * @param func Function used to calculate the objects location
     * @param mask_image Write the mask on top of the image
     * @param mask_threshold The mask threshold
     */
    MaskRCNNModule(const std::string& input_path, float minimum_confidence, bool calculate_location = true,
                   MaskRCNNFunction func = getCenter, bool mask_image = true, float minimum_mask = 0.3);

    ~MaskRCNNModule();

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
    cv::dnn::Net*  maskrcnn;
    // Mask function
    MaskRCNNFunction mask_function;
    // Model confidence and mask threshold
    float confidence_threshold, mask_threshold;
    // Calculate object location and draw mask on image
    bool get_location, mask_image;
};
#endif