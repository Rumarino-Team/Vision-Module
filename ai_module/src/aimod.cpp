#include "aimod/aimod.hpp"

AI::AI(std::string input_path, bool record, std::string output_path, int fps) {
    //Set recording flag
    recording = record;

    PLOGI << "AI: Finding paths for cfg, weights and .names files...";
    std::string cfg, weights, names_path;

    //Identify the files inside the folder
    for (const auto & entry : std::experimental::filesystem::directory_iterator(input_path)) {
        if (entry.path().has_extension()) {
            if (entry.path().extension().string() == ".cfg") {
                cfg = entry.path().string();
                PLOGI << "AI: Found path for the cfg file... " + cfg;
            }
            else if (entry.path().extension().string() == ".names") {
                names_path = entry.path().string();
                PLOGI << "AI: Found path for the .names file... " + names_path;
            }
        }
        else if (entry.path().stem().string() == "weights") {
            for (const auto & all_weights : std::experimental::filesystem::directory_iterator(entry.path())) {
                std::string weight_str = all_weights.path().stem();
                //TODO: for now we only look for the best weights, there must be a better way
                if (weight_str.substr(weight_str.length() - 5) == "_best") {
                    weights = all_weights.path().string();
                    PLOGI << "AI: Found path for weights file... " + weights;
                    break;
                }
            }
        }
    }
    PLOGI << "AI: Finished Finding paths for cfg, weights and .names files.";

    PLOGI <<"AI: Getting the names inside the .names file.";
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
    PLOGI << "AI: Loaded all names inside .names file.";

    PLOGI << "AI: Loading darknet...";
    // Load darknet
    darknet = new Detector(cfg, weights);
    PLOGI << "AI: Finished loading darknet.";

    if(recording) {
        //Start the CV Video Writer
        PLOGI << "AI: Loading Video Writer... Output path: " + output_path;
        out_vid = cv::VideoWriter(output_path, cv::VideoWriter::fourcc('M','P','E','G'), fps, cv::Size(1920, 1080));
        PLOGI << "AI: Finished loading Video Writer.";
    }
}

DetectedObjects AI::detect_objects(cv::Mat &frame, float minimum_confidence) {
    //Create the resulting struct
    DetectedObjects results;

    // Image where detected structures are written in
    cv::Mat annotated_img;
    if(recording){
        PLOGI << "AI: Converting one image to another color space...";
        cv::cvtColor(frame, annotated_img, cv::COLOR_BGRA2BGR);
        PLOGI << "AI: Finished converting one image to another color space.";
    }


    //Predict items from the frame
    PLOGI << "AI: Getting predictions from darknet.";
    auto predictions = darknet->detect(frame, minimum_confidence);

    for(auto &prediction : predictions) {
        DetectedObject result;

        PLOGI << "AI: Saving result's bounding box.";
        result.bounding_box = cv::Rect(prediction.x, prediction.y, prediction.w, prediction.h);
        PLOGI << "AI: Saving result's id.";
        result.id = prediction.obj_id;
        // Get the name of the detected object; leave it empty if not found
        PLOGI << "AI: Saving result's name.";
        if (prediction.obj_id < names.size()) {
            result.name = names[prediction.obj_id].c_str();
        }
        else {
            result.name = "";
        }

        if(recording){
            cv::rectangle(annotated_img, result.bounding_box, cv::Scalar(0,255,0), 4, 8, 0);
            PLOGI << "AI: Drawing the result's bounding box on the frame.";
        }

        PLOGI << "AI: Pushing back result on current frame to results array.";
        results.push_back(result);
    }

    if(recording){
        PLOGI << "AI: Recording the annotated image to recording video.";
        out_vid.write(annotated_img);
    }

    PLOGI << "AI: Returning result.";
    return results;
}

DetectedObjects AI::detect(Video_Frame &frame, float minimum_confidence) {
    DetectedObjects results = this->detect_objects(frame.image, minimum_confidence);

    //At the moment we have to iterate the same array twice for compatibility with the only image method
    for (auto &result : results) {
        //Get mid point from of the predicted image and get the distance from the depth image
        //Float taken from depth map is distance in millimeters (mm)
        cv::Point2f mid_point = cv::Point2f(result.bounding_box.x + result.bounding_box.width/2, result.bounding_box.y + result.bounding_box.height/2);
        PLOGI << "AI: Getting result's distance";
        result.distance = frame.depth_map.at<float>(mid_point);

        //Saving the 3D point on the struct
        PLOGI << "AI: Getting result's 3D point.x";
        result.location.x = frame.point_cloud.at<cv::Vec4f>(mid_point)[0];
        PLOGI << "AI: Getting result's 3D point.y";
        result.location.y = frame.point_cloud.at<cv::Vec4f>(mid_point)[1];
        PLOGI << "AI: Getting result's 3D point.z";
        result.location.z = frame.point_cloud.at<cv::Vec4f>(mid_point)[2];
    }

    return results;
}

DetectedObjects AI::detect(cv::Mat &frame, float minimum_confidence) {
    PLOGI << "AI: Detecting objects with a minimum confidence of: " << minimum_confidence;
    return this->detect_objects(frame, minimum_confidence);
}

void AI::close() {
    if(recording){
        PLOGI << "AI: Ending the video recording.";
        out_vid.release();
    }
    PLOGI << "AI: Closing AI.";
}

AI::~AI() {
    AI::close();
}