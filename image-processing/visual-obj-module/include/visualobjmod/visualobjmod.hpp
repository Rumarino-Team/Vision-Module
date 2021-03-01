#ifndef __VISUAL_OBJ_MODULE__
#define __VISUAL_OBJ_MODULE__
#include "pipelinemod/pipelinemod.hpp"

class VisualObjModule : public PipelineModule {
public:
    /**
     * Writes detected object information on the image.
     *
     * @param boundingBox Writes the object location in the image.
     * @param objectName Writes the object name.
     * @param distance Writes the distance in relation to the camera.
     * @param location Writes the 3D location in relation to the camera.
     */
    VisualObjModule(cv::Scalar_<double>  color = cv::Scalar(0,255,0), int thickness=4,
                    bool boundingBox=true, bool boxCoords = true, bool objectName=true, bool distance=true, bool location=true);

    /**
     * Detects the objects in the image using Darknet
     *
     * @param frame The video image  frame
     * @param objs Objects array
     * @return Possible error is returned
     */
    PipelineErrors detect(Video_Frame &frame, DetectedObjects &objs) override;

private:
    cv::Scalar_<double> color;
    int thickness;
    bool boundingBox, boxCoords, objectName, distance, location;
};
#endif