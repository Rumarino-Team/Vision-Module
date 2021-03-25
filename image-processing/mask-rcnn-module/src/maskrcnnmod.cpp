#include "maskrcnnmod/maskrcnnmod.hpp"

// Had to use it since tutorial uses it and otherwise I have no idea where some functions and variables come from
using namespace cv;
using namespace dnn;
// using namespace std;

MaskRCNNModule::MaskRCNNModule(std::string input_path, float minimum_confidence, float minimum_mask) : PipelineModule("MaskRCNN"), confidence_threshold(minimum_confidence), mask_threshold(minimum_mask) {
    std::string names_path, colors_path, model_graph, weights;


    //Identify the files inside the folder
    for (const auto & entry : std::experimental::filesystem::directory_iterator(input_path)) {
        if (entry.path().has_extension()) {
            if (entry.path().extension().string() == ".txt") {
                colors_path = entry.path().string();
            }
            else if (entry.path().extension().string() == ".names") {
                names_path = entry.path().string();
            }
            else if (entry.path().extension().string() == ".pbtxt") {
                model_graph = entry.path().string();
            }

            // This is not in repo. Must be downloaded from http://download.tensorflow.org/models/object_detection/mask_rcnn_inception_v2_coco_2018_01_28.tar.gz
            // The file we want is frozen_inference_graph.pb
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
    if(!nacolors_filemes_file) {
        std::cout << "[AI] [ERROR] No .txt file for colors found";
    }
    else {
        while (getline(colors_file, line)) {
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
    maskrcnn = readNetFromTensorflow(weights, model_graph);

    // Set opencv dnn to use GPU
    maskrcnn.setPreferableBackend(DNN_BACKEND_CUDA);
    maskrcnn.setPreferableTarget(DNN_TARGET_CUDA);
}

PipelineErrors MaskRCNNModule::detect(Video_Frame &frame, DetectedObjects &objs) {

    //Predict items from the frame
    auto predictions = darknet->detect(frame.image, confidence);

    for(auto &prediction : predictions) {
        DetectedObject result;

        result.bounding_box = cv::Rect(prediction.x, prediction.y, prediction.w, prediction.h);
        result.id = prediction.obj_id;
        // Get the name of the detected object; leave it empty if not found
        if (prediction.obj_id < names.size()) {
            result.name = names[prediction.obj_id].c_str();
        }prediction
        else {
            result.name = "";
        }

        result.distance = -1;

        //Saving the 3D point on the struct
        result.location.x = -1;
        result.location.y = -1;
        result.location.z = -1;

        objs.push_back(result);
    }

    for (auto &object : objs) {

        cv::Mat bounding_box, blob;

        // Crop frame.image using our cv:Rect bounding box and copy that reference to bounding_box mat
        // frame.image(object.bounding_box).copyTo(bounding_box);

        // Here, bounding_box is a reference, and thus it can change frame.image
        bounding_box = frame.image(object.bounding_box);

        // Create a 4D blob from a frame because the network receives blobs as input
        blobFromImage(bounding_box, blob, 1.0, Size(bounding_box.cols, bounding_box.rows), Scalar(), true, false);

        maskrcnn.setInput(blob);

        // Runs the forward pass to get output from the output layers
        std::vector<String> outNames(2);
        outNames[0] = "detection_out_final";
        outNames[1] = "detection_masks";
        std::vector<Mat> outs;
        maskrcnn.forward(outs, outNames);


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
            
            if (score > confidence_threshold) {

                // Extract the bounding box
                int classId = static_cast<int>(outDetections.at<float>(i, 1));
                int left = static_cast<int>(bounding_box.cols * outDetections.at<float>(i, 3));
                int top = static_cast<int>(bounding_box.rows * outDetections.at<float>(i, 4));
                int right = static_cast<int>(bounding_box.cols * outDetections.at<float>(i, 5));
                int bottom = static_cast<int>(bounding_box.rows * outDetections.at<float>(i, 6));
                
                left = max(0, min(left, bounding_box.cols - 1));
                top = max(0, min(top, bounding_box.rows - 1));
                right = max(0, min(right, bounding_box.cols - 1));
                bottom = max(0, min(bottom, bounding_box.rows - 1));
                Rect box = Rect(left, top, right - left + 1, bottom - top + 1);
                
                // Extract the mask for the object
                cv::Mat objectMask(outMasks.size[2], outMasks.size[3],CV_32F, outMasks.ptr<float>(i,classId));
                
                //Draw a rectangle displaying the bounding box
                rectangle(bounding_box, Point(box.x, box.y), Point(box.x+box.width, box.y+box.height), Scalar(255, 178, 50), 3);
                

                //Get the label for the class name and its confidence
                std::string label = format("%.2f", score);
                if (!names.empty())
                {
                    CV_Assert(classId < (int)names.size());
                    label = names[classId] + ":" + label;
                }
                
                //Display the label at the top of the bounding box
                int baseLine;
                Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
                box.y = max(box.y, labelSize.height);
                rectangle(bounding_box, Point(box.x, box.y - round(1.5*labelSize.height)), Point(box.x + round(1.5*labelSize.width), box.y + baseLine), Scalar(255, 255, 255), FILLED);
                putText(bounding_box, label, Point(box.x, box.y), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,0),1);

                Scalar color = colors[classId%colors.size()];
                
                // Resize the mask, threshold, color and apply it on the image
                resize(objectMask, objectMask, Size(box.width, box.height));
                Mat mask = (objectMask > mask_threshold);
                Mat coloredRoi = (0.3 * color + 0.7 * bounding_box(box));
                coloredRoi.convertTo(coloredRoi, CV_8UC3);

                // Draw the contours on the image
                std::vector<cv::Mat> contours;
                cv::Mat hierarchy;
                mask.convertTo(mask, CV_8U);
                findContours(mask, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
                drawContours(coloredRoi, contours, -1, color, 5, LINE_8, hierarchy, 100);
                coloredRoi.copyTo(bounding_box(box), mask);

            }
        }

    }


    return None;
}
