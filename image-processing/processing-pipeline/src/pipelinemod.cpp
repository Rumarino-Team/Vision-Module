#include "pipelinemod/pipelinemod.hpp"

PipelineModule::PipelineModule(const char *name) : name(name) {}

PipelineErrors PipelineModule::detect(Video_Frame &frame, DetectedObjects &objs) {
    return NotImplemented;
}

PipelineManager::PipelineManager(Pipeline &pipeline, bool record, const std::string& output_path,
                                 int fps) : pipeline(pipeline), recording(record) {
    if(recording) {
        //Start the CV Video Writer
        out_vid = cv::VideoWriter(output_path, cv::VideoWriter::fourcc('M','P','E','G'), fps, cv::Size(1920, 1080));
    }
}

DetectedObjects PipelineManager::detect(Video_Frame &frame) {
    DetectedObjects objs;
    //At the moment the frame.image gets edited via each pass through
    cv::cvtColor(frame.image, frame.image, cv::COLOR_BGRA2BGR);

    for (auto module : pipeline) {
        module->detect(frame, objs);
    }

    if(recording) {
        // Must do the cvt color to change color space
        out_vid.write(frame.image);
    }

    return objs;
}

void PipelineManager::close() {
    if(recording){
        out_vid.release();
    }
}
