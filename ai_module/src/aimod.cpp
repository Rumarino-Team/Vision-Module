#include "aimod/aimod.hpp"
//TODO: Implement DrawInference method. To Do that we have to do an Override to the function by creating a class
// that inherit the Yolov7 Model. This is just for eliminating the cv::imwrite function.

     Yolov7::DrawResults(const std::vector<DetectRes> &detections, std::vector<cv::Mat> vec_img,
                            std::vector<std::string> image_name=std::vector<std::string>()) {
        org_img = vec_img;
        std::vector<Bbox> rects = detections.det_results;
        if (channel_order == "BGR")
            cv::cvtColor(org_img, org_img, cv::COLOR_BGR2RGB);
        for(const auto &rect : rects) {
            char t[256];
            sprintf(t, "%.2f", rect.prob);
            std::string name = class_labels[rect.classes] + "-" + t;
            cv::putText(org_img, name, cv::Point(rect.x - rect.w / 2, rect.y - rect.h / 2 - 5),
                    cv::FONT_HERSHEY_COMPLEX, 0.7, class_colors[rect.classes], 2);
            cv::Rect rst(rect.x - rect.w / 2, rect.y - rect.h / 2, rect.w, rect.h);
            cv::rectangle(org_img, rst, class_colors[rect.classes], 2, cv::LINE_8, 0);
        }
        return org_img;
        }
    


AI::AI(std::string input_yaml, bool record, std::string output_path, int fps) {
    //Set recording flag
    recording = record;
    
    // Check if the directory is a yaml document.
    if(std::filesystem::path(input_yaml) != "yaml"){
        std::cout << "File extension is not yaml." std::endl;
    }
    // Load yolov7
    Yolov7 yolov7 = new Yolov7(input_yaml);

    if(recording) {
        //Start the CV Video Writer
        out_vid = cv::VideoWriter(output_path, cv::VideoWriter::fourcc('M','P','E','G'), fps, cv::Size(1920, 1080));
    }
}
// All function that return DetectedObjects now will return CustomobjectData
sl::CustomBoxObjectData AI::detect_objects(std::vector<cv::Mat> &frames) {
    //
    sl::CustomBoxObjectData results;

    // Image where detected structures are written in
    cv::Mat annotated_img;
    if(recording)
        cv::cvtColor(frame, annotated_img, cv::COLOR_BGRA2BGR);

    //Predict items from the frames
    std::vector<DetectRes> predictions = yolov7->InferenceImages(frames);
    for(Bbox &prediction : predictions.det_results) {
        cv::Rect bounding_box = cv::Rect(prediction.x, prediction.y, prediction.w, prediction.h);

        //Create the CustomBoxObjectData
        sl::CustomBoxObjectData tmp;

        //Setting the Bounding box info
        std::vector<sl::uint2> bbox_out(4);
        bbox_out[0] = sl::uint2(prediction.x, prediction.y);
        bbox_out[1] = sl::uint2(prediction.x + predition.width, prediction.y);
        bbox_out[2] = sl::uint2(prediction.x + prediction.width, prediction.y + prediction.height);
        bbox_out[3] = sl::uint2(prediction.x, prediction.y + prediction.height);


        // Fill the detections into the correct SDK format
        tmp.unique_object_id = sl::generate_unique_id();
        tmp.probability = prediction.conf;
        tmp.label = (int) prediction.class_id;
        tmp.bounding_box_2d = bbox_out;
        tmp.is_grounded = true; // objects are moving on the floor plane and tracked in 2D only
        objects_in.push_back(tmp);


    // Add the new frame annotated to the Video.   
    if(recording){
        annotated_img = yolov7->DrawResults();
        out_vid.write(annotated_img);
    }

    return results;
}

DetectedObjects AI::detect(cv::Mat &frame) {
    // Inference need a Vector type.
    auto vect = std::vector<cv::Mat>;
    vect.push_back(frame);
    return this->detect_objects(vect);
}

void AI::close() {
    if(recording){
        out_vid.release();
    }
}

AI::~AI() {
    AI::close();
}