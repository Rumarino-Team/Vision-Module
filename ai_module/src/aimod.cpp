#include "aimod/aimod.hpp"

AI::AI(bool record, std::string input_path, std::string output_path) {
    //Set paths
    out_path = output_path;

    //Set recording flag
    recording = record;

    //Initialize the save count, this is used for naming purposes on the output images
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
    std::string cfg = input_path + + "/" + dir_name + ".cfg";
    std::string weights = input_path + "/weights/" + dir_name + "_best.weights";
    std::string names = input_path + "/custom.names";

    //Load Darkhelp < darkhelp(config file, weights file, .names file)
    darkhelp = DarkHelp(cfg, weights, names);

    //Start the CV Video Writer
    //VideoWriter video("outcpp.avi",CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height));
    out_vid = cv::VideoWriter(output_path+"/output_vid.avi", cv::CAP_OPENCV_MJPEG, 10, cv::Size(1920, 1080));
}

DetectedObject AI::detect(Video_Frame frame) {
    //Create the resulting struct
    DetectedObject result;

    //Predict items from the frame
    DarkHelp::PredictionResults prediction = darkhelp.predict(frame.image);
    result.bounding_box = prediction.at(0).rect;
    result.obj_id = prediction.at(0).best_class;
    result.obj_name = (prediction.at(0).name).c_str();

    //Get mid point from of the predicted image and get the distance from the depth image
    //Float taken from depth map is distance in millimeters (mm)
    cv::Point2f mid_point = prediction.at(0).original_point;
    result.distance = frame.depth_map.at<float>(mid_point);

    //Saving the 3D point on the struct
    std::vector<float> pc_values = frame.point_cloud.at<std::vector<float>>(mid_point);
    result.point_3d.x = pc_values[0];
    result.point_3d.y = pc_values[1];
    result.point_3d.z = pc_values[2];
//    float x = pc_values[0]; float y = pc_values[1]; float z = pc_values[2];
//    float distance = sqrt(x*x + y*y + z*z);

    if(recording){
        //Runtime recording with CV VideoWriter, write the annotated images
        //https://stackoverflow.com/questions/22765397/create-video-from-images-using-videocapture-opencv
        cv::Mat annotated_img = darkhelp.annotate();
        out_vid.write(annotated_img);

        //Testing purposes, saving individual annotated images
        cv::imwrite(out_path+"/frameNum"+std::to_string(save_count)+".png", annotated_img, {cv::IMWRITE_PNG_COMPRESSION, 9});
        save_count++;
    }

    return result;
}

std::string AI::close() {
    if(recording){
        out_vid.release();
        return "Closing AI. Video generated at: " + out_path;
    }
    else
        return "Closing AI. No CV Video was generated.";
}

AI::~AI() {
    AI::close();
}
