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
    cfg = input_path + dir_name + ".cfg";
    weights = input_path + "weights/" + dir_name + "_best.weights";
    names = input_path + "custom.names";
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

    //TODO: logic to get distance
    //get mid point from bounding_box and get float32 from the depth image
    cv::Point2f mid_point = prediction.at(0).original_point;

    //TODO: figure out what the line below returns and how to work with it
    //frame.depth_map.at<cv::float16_t>(mid_point);

    if(recording){
        cv::Mat annotated_img = darkhelp.annotate();
        cv::imwrite(out_path+"frameNum"+std::to_string(save_count)+".png", annotated_img, {cv::IMWRITE_PNG_COMPRESSION, 9});
        save_count++;
    }

    return result;
}

void AI::close() {

}