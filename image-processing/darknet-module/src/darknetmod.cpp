#include "darknetmod/darknetmod.hpp"

DarknetModule::DarknetModule(std::string input_path, float minimum_confidence) : PipelineModule("Darknet"), confidence(minimum_confidence) {
    std::string cfg, weights, names_path;

    //Identify the files inside the folder
    PLOGD << "Finding .cfg, .names, and .weights files...";
    for (const auto & entry : std::experimental::filesystem::directory_iterator(input_path)) {
        if (entry.path().has_extension()) {
            if (entry.path().extension().string() == ".cfg") {
                cfg = entry.path().string();
                PLOGD << "Found .cfg file at " + cfg;
            }
            else if (entry.path().extension().string() == ".names") {
                names_path = entry.path().string();
                PLOGD << "Found .names file at " + names_path;
            }
        }
        else if (entry.path().stem().string() == "weights") {
            for (const auto & all_weights : std::experimental::filesystem::directory_iterator(entry.path())) {
                std::string weight_str = all_weights.path().stem();
                //TODO: for now we only look for the best weights, there must be a better way
                if (weight_str.substr(weight_str.length() - 5) == "_best") {
                    weights = all_weights.path().string();
                    PLOGD << "Found .weights file at " + weights;
                    break;
                }
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
        names_file.close();
    }

    // Load darknet
    darknet = new Detector(cfg, weights);
    PLOGI << "Finished initializing darknet AI module";
}

PipelineErrors DarknetModule::detect(Video_Frame &frame, DetectedObjects &objs) {

    //Predict items from the frame
    auto predictions = darknet->detect(frame.image, confidence);

    PLOGD << "Detecting objects from predictions...";
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

        result.distance = -1;

        //Saving the 3D point on the struct
        result.location.x = -1;
        result.location.y = -1;
        result.location.z = -1;

        objs.push_back(result);
    }
    PLOGD << "Finished detecting objects.";

    return None;
}
