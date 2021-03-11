#include "aimod/aimod.hpp"

AI::AI(std::string input_path, bool record, std::string output_path, int fps) {
    //Set recording flag
    recording = record;

    PLOGD << "Finding paths for cfg, weights and .names files...";
    std::string cfg, weights, names_path;

    //Identify the files inside the folder
    for (const auto & entry : std::experimental::filesystem::directory_iterator(input_path)) {
        if (entry.path().has_extension()) {
            if (entry.path().extension().string() == ".cfg") {
                cfg = entry.path().string();
                PLOGD << "Found path for the cfg file... " + cfg;
            }
            else if (entry.path().extension().string() == ".names") {
                names_path = entry.path().string();
                PLOGD << "Found path for the .names file... " + names_path;
            }
        }
        else if (entry.path().stem().string() == "weights") {
            for (const auto & all_weights : std::experimental::filesystem::directory_iterator(entry.path())) {
                std::string weight_str = all_weights.path().stem();
                //TODO: for now we only look for the best weights, there must be a better way
                if (weight_str.substr(weight_str.length() - 5) == "_best") {
                    weights = all_weights.path().string();
                    PLOGD << "Found path for weights file... " + weights;
                    break;
                }
            }
        }
    }
    PLOGI << "Finished Finding paths for cfg, weights and .names files.";

    PLOGD <<"Getting the names inside the .names file.";
    //Get the names inside the file
    std::ifstream names_file;
    names_file.open(names_path.c_str());
    if(!names_file) {
        PLOGI << "[ERROR] No .names file found";
    }
    else {
        std::string name;
        while(!names_file.eof()) {
            getline(names_file, name);
            names.push_back(name);
        }
    }
    PLOGD << "Loaded all names inside .names file.";

    PLOGD << "Loading darknet...";
    // Load darknet
    darknet = new Detector(cfg, weights);
    PLOGI << "Finished loading darknet.";

    if(recording) {
        //Start the CV Video Writer
        PLOGD << "Loading Video Writer... Output path: " + output_path;
        out_vid = cv::VideoWriter(output_path, cv::VideoWriter::fourcc('M','P','E','G'), fps, cv::Size(1920, 1080));
        PLOGI << "Finished loading Video Writer.";
    }
}

DetectedObjects AI::detect_objects(cv::Mat &frame, float minimum_confidence) {
    //Create the resulting struct
    DetectedObjects results;

    // Image where detected structures are written in
    cv::Mat annotated_img;
    if(recording){
        PLOGD << "Converting one image to another color space...";
        cv::cvtColor(frame, annotated_img, cv::COLOR_BGRA2BGR);
        PLOGD << "Finished converting one image to another color space.";
    }


    //Predict items from the frame
    PLOGD << "Getting predictions from darknet.";
    auto predictions = darknet->detect(frame, minimum_confidence);

    for(auto &prediction : predictions) {
        DetectedObject result;

        result.bounding_box = cv::Rect(prediction.x, prediction.y, prediction.w, prediction.h);
        result.id = prediction.obj_id;
        // Get the name of the detected object; leave it empty if not found
        if (prediction.obj_id < names.size()) {
            result.name = names[prediction.obj_id].c_str();
        }
        else {
            result.name = "";
        }

        if(recording){
            cv::rectangle(annotated_img, result.bounding_box, cv::Scalar(0,255,0), 4, 8, 0);
        }

        results.push_back(result);
    }
    PLOGD << "Finished creating results array from predictions.";

    if(recording){
        PLOGD << "Recording the annotated image to recording video.";
        out_vid.write(annotated_img);
    }

    PLOGI << "Returning result.";
    return results;
}

DetectedObjects AI::detect(Video_Frame &frame, float minimum_confidence) {
    DetectedObjects results = this->detect_objects(frame.image, minimum_confidence);

    //At the moment we have to iterate the same array twice for compatibility with the only image method
    PLOGD << "Saving distance and 3D point to results...";
    for (auto &result : results) {
        //Get mid point from of the predicted image and get the distance from the depth image
        //Float taken from depth map is distance in millimeters (mm)
        cv::Point2f mid_point = cv::Point2f(result.bounding_box.x + result.bounding_box.width/2, result.bounding_box.y + result.bounding_box.height/2);
        result.distance = frame.depth_map.at<float>(mid_point);

        //Saving the 3D point on the struct
        result.location.x = frame.point_cloud.at<cv::Vec4f>(mid_point)[0];
        result.location.y = frame.point_cloud.at<cv::Vec4f>(mid_point)[1];
        result.location.z = frame.point_cloud.at<cv::Vec4f>(mid_point)[2];
    }
    PLOGD << "Finished saving distance and 3D point to results.";

    return results;
}

DetectedObjects AI::detect(cv::Mat &frame, float minimum_confidence) {
    PLOGD << "Detecting objects with a minimum confidence of: " << minimum_confidence;
    return this->detect_objects(frame, minimum_confidence);
}

void AI::close() {
    if(recording){
        PLOGI << "Ending the video recording.";
        out_vid.release();
    }
    PLOGI << "Closing AI.";
}

AI::~AI() {
    AI::close();
}