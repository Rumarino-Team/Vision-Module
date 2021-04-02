#include "pipelinemod/pipelinemod.hpp"

PipelineModule::PipelineModule(const char *name) : name(name) {}

PipelineErrors PipelineModule::detect(Video_Frame &frame, DetectedObjects &objs) {
    return NotImplemented;
}

PipelineManager::PipelineManager(Pipeline &pipeline, bool record, const std::string& output_path,
                                 int fps) : pipeline(pipeline), recording(record) {
    if(recording) {
        //Start the CV Video Writer
        PLOGI << "Starting VideoWriter to record outputs";
        out_vid = cv::VideoWriter(output_path, cv::VideoWriter::fourcc('M','P','E','G'), fps, cv::Size(1920, 1080));
    }
}

DetectedObjects PipelineManager::detect(Video_Frame &frame) {
    DetectedObjects objs;

    //At the moment the frame.image gets edited via each pass through
    PLOGD << "Detecting object inside frame";
    for (auto module : pipeline) {
        module->detect(frame, objs);
    }

    if(recording) {
        // Must do the cvt color to change color space
        PLOGD << "Changing frame color space in order to save it on the video output";
        cv::Mat record_img;
        cv::cvtColor(frame.image, record_img, cv::COLOR_BGRA2BGR);
        out_vid.write(record_img);
    }

    PLOGD << "Returning array of detected objects";
    return objs;
}

void PipelineManager::close() {
    if(recording){
        PLOGI << "Tidying up recorded video";
        out_vid.release();
    }
    PLOGI << "PipelineManager properly closed.";
}
