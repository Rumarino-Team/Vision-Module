#include "maskrcnnmod/maskrcnnmod.hpp"

#include <utility>

// Had to use it since tutorial uses it and otherwise I have no idea where some functions and variables come from
using namespace cv;
using namespace dnn;
// using namespace std;

MaskRCNNModule::MaskRCNNModule(const std::string& input_path, float minimum_confidence, bool calculate_location,
                               MaskRCNNFunction func, bool mask_image, float minimum_mask) : PipelineModule("MaskRCNN"),
                               confidence_threshold(minimum_confidence), get_location(calculate_location),
                               mask_function(std::move(func)), mask_image(mask_image), mask_threshold(minimum_mask) {
    std::string names_path, colors_path, model_graph, weights;


    //Identify the files inside the folder
    for (const auto & entry : std::experimental::filesystem::directory_iterator(input_path)) {
        if (entry.path().has_extension()) {
            if (entry.path().extension().string() == ".colors") {
                colors_path = entry.path().string();
            }
            else if (entry.path().extension().string() == ".names") {
                names_path = entry.path().string();
            }
            else if (entry.path().extension().string() == ".pbtxt") {
                model_graph = entry.path().string();
            }

            // This is not in repo. Must be downloaded from http://download.tensorflow.org/models/object_detection/mask_rcnn_inception_v2_coco_2018_01_28.tar.gz
            // The file we want is frozen_inference_graph.pb and you have to put it in test/media/MASK-RCNN/MASK-RCNN.pb
            else if (entry.path().extension().string() == ".pb") {
                weights = entry.path().string();
            }
        }
    }

    //Get the names inside the file
    std::ifstream names_file;
    names_file.open(names_path.c_str());
    if(!names_file) {
        std::cout << "[AI] [ERROR] No .names file found";
    }
    else {
        std::string name;
        while(!names_file.eof()) {
            getline(names_file, name);
            names.push_back(name);
        }
    }

    //Get the names inside the file
    std::ifstream colors_file;
    colors_file.open(colors_path.c_str());
    if(!colors_file) {
        std::cout << "[AI] [ERROR] No .txt file for colors found";
    }
    else {
        std::string line;
        if(!colors_file.eof()) {
            getline(colors_file, line);
            char* pEnd;
            double r, g, b;
            r = strtod (line.c_str(), &pEnd);
            g = strtod (pEnd, NULL);
            b = strtod (pEnd, NULL);
            Scalar color = Scalar(r, g, b, 255.0);
            colors.push_back(Scalar(r, g, b, 255.0));
        }
    }

    // Load the network
    maskrcnn = new cv::dnn::Net(readNetFromTensorflow(weights, model_graph));

    // Set opencv dnn to use GPU
    maskrcnn->setPreferableBackend(DNN_BACKEND_CUDA);
    maskrcnn->setPreferableTarget(DNN_TARGET_CUDA);
    // CPU
//    maskrcnn->setPreferableBackend(DNN_BACKEND_OPENCV);
//    maskrcnn->setPreferableTarget(DNN_TARGET_CPU);
}

PipelineErrors MaskRCNNModule::detect(Video_Frame &frame, DetectedObjects &objs) {

    cv::Mat blob;

    // Crop frame.image using our cv:Rect bounding box and copy that reference to bounding_box mat
    // frame.image(object.bounding_box).copyTo(bounding_box);

    // Create a 4D blob from a frame because the network receives blobs as input
    int height = frame.image.cols;
    int width = frame.image.rows;
    blobFromImage(frame.image, blob, 1.0, Size(height, width), Scalar(), true, false);
    //blobFromImage(frame.image, blob, 1.0, Size(frame.image.cols, frame.image.rows), Scalar(), true, false);

    maskrcnn->setInput(blob);

    // Runs the forward pass to get output from the output layers
    std::vector<String> outNames(2);
    outNames[0] = "detection_out_final";
    outNames[1] = "detection_masks";
    std::vector<Mat> outs;
    maskrcnn->forward(outs, outNames);

    // Now we mask the objects in this object
    cv::Mat outDetections = outs[0];
    cv::Mat outMasks = outs[1];

    // Output size of masks is NxCxHxW where
    // N - number of detected boxes
    // C - number of classes (excluding background)
    // HxW - segmentation shape

    const int numDetections = outDetections.size[2];
    const int numClasses = outMasks.size[1];

    outDetections = outDetections.reshape(1, outDetections.total() / 7);

    for (int i = 0; i < numDetections; ++i) {

        float score = outDetections.at<float>(i, 2);

        if (score >= confidence_threshold) {
            DetectedObject result;

            // Extract the bounding box
            int classId = static_cast<int>(outDetections.at<float>(i, 1));
            int left = static_cast<int>(height * outDetections.at<float>(i, 3));
            int top = static_cast<int>(width * outDetections.at<float>(i, 4));
            int right = static_cast<int>(height * outDetections.at<float>(i, 5));
            int bottom = static_cast<int>(width * outDetections.at<float>(i, 6));

            left = max(0, min(left, height - 1));
            top = max(0, min(top, width - 1));
            right = max(0, min(right, height - 1));
            bottom = max(0, min(bottom, width - 1));
            Rect box = Rect(left, top, right - left + 1, bottom - top + 1);

            // Makes sure ROI is valid
            if (box.width > 0 && box.height > 0 && box.x >= 0 && box.y >= 0 && box.x + box.width <= height && box.y + box.height <= width) {

                // Setup object
                result.bounding_box = box;
                result.id = classId;
                //Get the label for the class name and its confidence
                // score is confidence, must store this in structure
                if (!names.empty())
                {
                    CV_Assert(classId < (int)names.size());
                    result.name = names[classId].c_str();
                }

                if (get_location || mask_image) {
                    // Extract the mask for the object
                    cv::Mat objectMask(outMasks.size[2], outMasks.size[3], CV_32F, outMasks.ptr<float>(i,classId));
                    // Resize image
                    resize(objectMask, objectMask, Size(box.width, box.height));


                    if (mask_image) {
                        // Generate random color per instance
                        // TODO: make colors directly linked to item ID
                        int colorInd = std::rand() % colors.size();
                        //Scalar color = colors[colorInd];
                        Scalar color(0,0,0);
                        // Threshold, color and apply it on the image

                        Mat mask = (objectMask > mask_threshold);
                        Mat coloredRoi = (0.3 * color + 0.7 * frame.image(box));
                        coloredRoi.convertTo(coloredRoi, CV_8UC3);

                        // Draw the contours on the image
                        std::vector<cv::Mat> contours;
                        cv::Mat hierarchy;
                        mask.convertTo(mask, CV_8U);
                        findContours(mask, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
                        drawContours(coloredRoi, contours, -1, color, 5, LINE_8, hierarchy, 100);
                        coloredRoi.copyTo(frame.image(box), mask);
                    }

                    if (get_location) {
                        // Get recommended point relative to the mask
                        cv::Point2i relativeLoc = mask_function(objectMask);
                        // Get real location based on the relative point
                        cv::Point2i realLoc(box.x + relativeLoc.x, box.y + relativeLoc.y);
                        // Get 3D location
                        result.location = frame.point_cloud.at<cv::Point3f>(realLoc);
                        // Get distance
                        result.distance = frame.depth_map.at<float>(realLoc);
                    }
                }

                objs.push_back(result);
            }
        }
    }

    return None;
}

MaskRCNNModule::~MaskRCNNModule() {
    delete maskrcnn;
}

// Mask functions
cv::Point2i getCenter(cv::Mat& mask) {
    int divisions = 1;
    int max_divisions = 5;
    int length = mask.rows;
    int height = mask.cols;

    while(true) {
        for (int i = 1; i <= divisions; i++) {
            // Get center of region
            float p1 = ((float)i-1.0f)/(float)divisions;
            float p2 = (float)i/(float)divisions;

            int x1 = length*p1;
            x1 = (x1 < 0) ? 0 : x1;
            int x2 = length*p2;
            x2 = (x2 > length) ? length : x2;
            int y1 = height*p1;
            y1 = (y1 < 0) ? 0 : y1;
            int y2 = height*p2;
            y2 = (y2 > height) ? height : y2;


            int x = x1 + ((x2 - x1)/2);
            int y = y1 + ((y2 - y1)/2);

            cv::Point2i center(x, y);

            if (mask.at<float>(center) > 0.8) {
                return center;
            }
        }
        divisions++;
        if (divisions > max_divisions) {
            return cv::Point2i(length/2, height/2);
        }
    }
}
