#include "aimod/aimod.hpp"

AI::AI(bool record, std::string input_path, std::string output_path) {
    //Set input and output paths
    in_path = input_path;
    out_path = output_path;

    //Set recording boolean
    recording = record;

    //Initialize the save count, this is used for naming purposes on the outputs
    save_count = 0;

    //Set name of the directory from input path
    //Vector of string to save tokens
    std::vector <std::string> tokens;
    //stringstream class charToString
    std::stringstream charToString(input_path);
    std::string intermediate;
    //Tokenizing w.r.t. slash '/'
    while(getline(charToString, intermediate, '/'))
        if(!intermediate.empty() && intermediate != " ")
            tokens.push_back(intermediate);
    //The Folder Name should be the last token
    dir_name = tokens.back();

    //Set path for each necessary file
    cfg = input_path + + "/" + dir_name + ".cfg";
    weights = input_path + "/weights/" + dir_name + "_best.weights";
    names = input_path + "/custom.names";
}

DetectedObject AI::detect(Video_Frame frame) {
    //Create the resulting struct
    DetectedObject result;

    //Load Darkhelp < darkhelp(config file, weights file, .names file)
    DarkHelp darkhelp(cfg, weights, names);

    //Predict items from the frame
    DarkHelp::PredictionResults prediction = darkhelp.predict(frame.image);
    result.bounding_box = prediction.at(0).rect;
    result.obj_id = prediction.at(0).best_class;
    result.obj_name = (prediction.at(0).name).c_str();

    //Get mid point from of the predicted image and get the distance from the depth image
    //Float taken from depth map is distance in millimeters (mm)
    cv::Point2f mid_point = prediction.at(0).original_point;
    result.distance = frame.depth_map.at<float>(mid_point);

    if(recording){
        //TODO: change to runtime recording with CV VideoCapture
        //https://stackoverflow.com/questions/22765397/create-video-from-images-using-videocapture-opencv
        cv::Mat annotated_img = darkhelp.annotate();
        cv::imwrite(out_path+"/frameNum"+std::to_string(save_count)+".png", annotated_img, {cv::IMWRITE_PNG_COMPRESSION, 9});
        save_count++;
    }

    return result;
}

std::string AI::close() {
    if(recording){
        //TODO: close CV VideoCapture and return path
    }
    else
        return "Closing AI. No CV Video was generated.";
}