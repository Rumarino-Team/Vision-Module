#include "aimod/aimod.hpp"

AI::AI(bool record, std::string input_path, std::string output_path) {
    //Set path, a path should NOT end with a "/"
    out_path = output_path;

    //Set recording flag
    recording = record;

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
    //darkhelp = DarkHelp(cfg, weights, names);
    darknet = new Detector(cfg, weights);

    //Start the CV Video Writer
    //VideoWriter video("outcpp.avi",CV_FOURCC('M','J','P','G'),10, Size(frame_width,frame_height));
    out_vid = cv::VideoWriter(output_path, cv::CAP_OPENCV_MJPEG, 10, cv::Size(1920, 1080));
}

std::vector<DetectedObject> AI::detect(Video_Frame frame, float minimum_confidence) {
    //Create the resulting struct
    std::vector<DetectedObject> results;

    //Predict items from the frame
    auto predictions = darknet->detect(frame.image, minimum_confidence);

    for(auto &prediction : predictions) {
        DetectedObject result;

        result.bounding_box = cv::Rect(prediction.x, prediction.y, prediction.w, prediction.h);
        result.obj_id = prediction.obj_id;
        result.obj_name = "Empty";

        //Get mid point from of the predicted image and get the distance from the depth image
        //Float taken from depth map is distance in millimeters (mm)
        cv::Point2f mid_point = cv::Point2f(prediction.x + prediction.w/2, prediction.y + prediction.h/2);
        result.distance = frame.depth_map.at<float>(mid_point);

        //Saving the 3D point on the struct
        std::vector<float> pc_values = frame.point_cloud.at<std::vector<float>>(mid_point);
        result.point_3d.x = pc_values[0];
        result.point_3d.y = pc_values[1];
        result.point_3d.z = pc_values[2];
        //float x = pc_values[0]; float y = pc_values[1]; float z = pc_values[2];
        //float distance = sqrt(x*x + y*y + z*z);

        results.push_back(result);
    }

    if(recording){
        cv::Mat annotated_img = frame.image;
        out_vid.write(annotated_img);
    }

    return results;
}

void AI::close() {
    if(recording){
        out_vid.release();
    }
}

AI::~AI() {
    AI::close();
}