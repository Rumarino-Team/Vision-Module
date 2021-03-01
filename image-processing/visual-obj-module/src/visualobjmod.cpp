#include "visualobjmod/visualobjmod.hpp"

#include <utility>

VisualObjModule::VisualObjModule(cv::Scalar_<double>  color, int thickness, bool boundingBox, bool boxCoords,
                                 bool objectName, bool distance, bool location) :
                                 PipelineModule("Visual Object Writer"), color(std::move(color)),
                                 thickness(thickness), boundingBox(boundingBox), boxCoords(boxCoords),
                                 objectName(objectName), distance(distance), location(location) {}

PipelineErrors VisualObjModule::detect(Video_Frame &frame, DetectedObjects &objs) {

    //  Model Name
    //  _____________
    // |             | (x, y) w: w:
    // |             | distance
    // |             | (x, y, z)
    // |             |
    //  -------------
    int spacing = 20;
    for (const DetectedObject& obj : objs) {
        if (boundingBox) {
            cv::rectangle(frame.image, obj.bounding_box, color, thickness, 8, 0);
        }
        if (objectName) {
            cv::putText(frame.image, obj.name, cv::Point(obj.bounding_box.x, obj.bounding_box.y-thickness),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 2);
        }
        if (boxCoords) {
            cv::putText(frame.image, "("+std::to_string(obj.bounding_box.x)+", "+std::to_string(obj.bounding_box.y)+")",
                        cv::Point(obj.bounding_box.x+obj.bounding_box.width+thickness, obj.bounding_box.y+spacing),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 2);

            cv::putText(frame.image,"w: "+std::to_string(obj.bounding_box.width)+" h: "+std::to_string(obj.bounding_box.height),
                        cv::Point(obj.bounding_box.x+obj.bounding_box.width+thickness, obj.bounding_box.y+spacing*2),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 2);
        }
        if (distance) {
            cv::putText(frame.image,"loc: "+std::to_string(obj.distance),
                        cv::Point(obj.bounding_box.x+obj.bounding_box.width+thickness, obj.bounding_box.y+spacing*3),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 2);
        }
        if (location) {
            cv::putText(frame.image,"("+std::to_string(obj.location.x)+", "+std::to_string(obj.location.y)+", "+std::to_string(obj.location.z)+")",
                        cv::Point(obj.bounding_box.x+obj.bounding_box.width+thickness, obj.bounding_box.y+spacing*4),
                        cv::FONT_HERSHEY_COMPLEX_SMALL, 1.0, color, 2);
        }
    }
    return None;
}
