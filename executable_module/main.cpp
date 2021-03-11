#include <pthread.h>
#include <mutex>
#include <atomic>
#include <chrono>
#include <condition_variable>

#include "zedmod/zedmod.hpp"
#include "aimod/aimod.hpp"
#include "httpapimod/apimod.hpp"

// Thread variables
std::mutex frame_mutex, obj_mutex;

// Used to handle new frames
// NOTE: new_obj currently not used
std::condition_variable new_frame, new_obj;

void camera_stream(ZED_Camera &cam, Video_Frame &frame, std::atomic<bool> &running){
    // Stop the thread properly
    PLOGI << "Stopping CAMERA_STREAM thread properly...";
    while (running) {
        std::unique_lock<std::mutex> lock(frame_mutex);
        frame = cam.update();
        new_frame.notify_one();
        lock.unlock();
        // Thread too fast and causing blocking
        std::this_thread::sleep_for(std::chrono::microseconds (1));
    }
    cam.close();
    PLOGI << "Stopped CAMERA_STREAM thread.";
}

void ai_stream(AI &ai, float confidence, DetectedObjects &objs, Video_Frame &frame, std::atomic<bool> &running) {
    // Stop the thread properly
    PLOGI << "Stopping AI_STREAM thread properly...";
    while (running) {
        // Handle new frame
        std::unique_lock<std::mutex> frame_lock(frame_mutex);
        new_frame.wait(frame_lock);
        //Copy frame to prevent longer locking
        Video_Frame frame_copy(frame);
        frame_lock.unlock();

        // Handle new object
        std::unique_lock<std::mutex> obj_lock(obj_mutex);
        objs = ai.detect(frame_copy, confidence);
        new_obj.notify_one();
        obj_lock.unlock();
    }
    ai.close();
    PLOGI << "Stopped AI_STREAM thread.";
}

void print_help() {
    std::cout << "---First define ZED parameters---" << std::endl;
    std::cout << "[OPTIONAL] -zr   --zed_record" << std::endl;
    std::cout << "\tUsage -zr /path/video_out.avi" << std::endl;
    std::cout << "[OPTIONAL] -res  --resolution" << std::endl;
    std::cout << "\t-res 1080" << std::endl;
    std::cout << "[OPTIONAL] -zfps --zed_fps" << std::endl;
    std::cout << "\t-zfps 60" << std::endl;
    std::cout << "----------------or---------------\n" << std::endl;
    std::cout << "[OPTIONAL] -zp   --zed_play" << std::endl;
    std::cout << "\t-zp /path/video_in.avi" << std::endl;
    std::cout << "--Now define darknet parameters--" << std::endl;
    std::cout << "[REQUIRED] -m    --yolo_model" << std::endl;
    std::cout << "\t-m /path/model" << std::endl;
    std::cout << "[OPTIONAL] -mr   --model_record" << std::endl;
    std::cout << "\t-mr /path/video_out.avi" << std::endl;
    std::cout << "[OPTIONAL] -mfps --model_fps" << std::endl;
    std::cout << "\t-mfps 60" << std::endl;
    std::cout << "[OPTIONAL] -c    --confidence" << std::endl;
    std::cout << "\t-c 60" << std::endl;
    std::cout << "\n--Now define server params--" << std::endl;
    std::cout << "[OPTIONAL] -ip" << std::endl;
    std::cout << "\t-ip 0.0.0.0" << std::endl;
    std::cout << "[OPTIONAL] -p    --port" << std::endl;
    std::cout << "\t-p 8080" << std::endl;
    std::cout << "\n--Finally define logger params--" << std::endl;
    std::cout << "[OPTIONAL] -d    --debug" << std::endl;
}

int main(int argc, const char* argv[]) {
    // Zed arguments
    bool live_zed = true;
    bool z_record = false;
    sl::RESOLUTION z_res = sl::RESOLUTION::HD1080;
    int z_fps = 30;
    std::string z_out, z_in;
    // AI arguments
    std::string model, m_out;
    bool m_record = false;
    int m_fps = 15;
    int confidence_percent = 60;
    // API arguments
    const char* ip = "0.0.0.0";
    int port = 8080;
    // Logger arguments
    // INFO Logger by default
    std::string maxSeverity = "info";

    for (int i = 1; i < argc; i++) {
        std::string arg = std::string(argv[i]);
        if (arg == "-h" || arg == "--help") {
            print_help();
            return 0;
        }
        else if (arg == "-zr" || arg == "--zed_record") {
            live_zed = true;
            z_out = argv[++i];}
        else if (arg == "-res" || arg == "--resolution") {
            std::string res = argv[++i];
            if (res == "1080") {
                z_res = sl::RESOLUTION::HD1080;
            }
            else if (res == "2k") {
                z_res = sl::RESOLUTION::HD2K;
            }
            else if (res == "720") {
                z_res = sl::RESOLUTION::HD720;
            }
            else if (res == "VGA") {
                z_res = sl::RESOLUTION::VGA;
            }
        }
        else if (arg == "-zfps" || arg == "--zed_fps") {
            z_fps = std::stoi(std::string(argv[++i]));
        }
        else if (arg == "-zp" || arg == "--zed_play") {
            live_zed = false;
            z_in = argv[++i];
        }
        else if (arg == "-m" || arg == "--yolo_model") {
            model = argv[++i];
        }
        else if (arg == "-mr" || arg == "--model_record") {
            m_record = true;
            m_out = argv[++i];
        }
        else if (arg == "-mfps" || arg == "--model_fps") {
            m_fps = std::stoi(std::string(argv[++i]));
        }
        else if (arg == "-c" || arg == "--confidence") {
            confidence_percent = std::stoi(std::string(argv[++i]));
        }
        else if (arg == "-ip") {
            ip = argv[++i];
        }
        else if (arg == "-p" || arg == "--port") {
            port = std::stoi(std::string(argv[++i]));
        }
        else if (arg == "-d" || arg == "--debug"){
            maxSeverity = "debug";
        }
    }


    // Initialize Logger
    initPLOG(maxSeverity);
    // Check what params were given prior to initializing the PLOG
    PLOGD << "Debug mode has been turned on for LOGGING.";
    if(maxSeverity == "debug"){ // Informational LOG for DEBUG
        if(!z_out.empty()){
            PLOGI << "ARG given for zed recording true and out at " << z_out;
        }
        if (z_res == sl::RESOLUTION::HD1080) {
            PLOGI << "ARG given for setting ZED RESOLUTION to HD1080";
        }
        else if (z_res == sl::RESOLUTION::HD2K) {
            PLOGI << "ARG given for setting ZED RESOLUTION to HD2k";
        }
        else if (z_res == sl::RESOLUTION::HD720) {
            PLOGI << "ARG given for setting ZED RESOLUTION to HD720";
        }
        else if (z_res == sl::RESOLUTION::VGA) {
            PLOGI << "ARG given for setting ZED RESOLUTION to VGA";
        }
        if(z_fps != 30){
            PLOGI << "ZED fps set to " << z_fps;
        }
        if(!z_in.empty()){
            PLOGI << "ARG given for zed playback at " << z_in;
        }
        if(!model.empty()){
            PLOGI << "ARG given for AI model is \"" << model << "\"";
        }
        if(!m_out.empty()){
            PLOGI << "ARG given for AI model recording is TRUE with out at " << m_out;
        }
        if(m_fps != 15){
            PLOGI << "ARG given for AI model fps is " << m_fps;
        }
        if(confidence_percent != 60){
            PLOGI << "ARG given for AI confidence percentage is " << confidence_percent << "%";
        }
        if(std::strcmp(ip, "0.0.0.0") != 0){
            PLOGI << "ARG given for IP is [" << ip << "]";
        }
        if(port != 8080){
            PLOGI << "ARG given for PORT is [" << port << "]";
        }
    }


    // Initialize ZED Cam
    PLOGI << "Initializing ZED Cam";
    std::shared_ptr<ZED_Camera> cam_ptr;
    if (live_zed) {
        cam_ptr.reset(new ZED_Camera(z_record, z_res, z_fps, z_out));
    } else {
        cam_ptr.reset(new ZED_Camera(z_in));
    }

    ZED_Camera& cam = *cam_ptr;
    Video_Frame frame;

    // Initialize AI
    PLOGI << "Initializing AI";
    AI ai(model, m_record, m_out, m_fps);
    DetectedObjects objs;
    float conf = float(confidence_percent) / 100;

    // Initialize API
    PLOGI << "Initializing API";
    API api(obj_mutex, objs);

    // Use argument variables
    static std::atomic<bool> running = true;

    // Since threads copy arguments we must pass them by reference.
    std::thread camera_thread(camera_stream, std::ref(cam), std::ref(frame), std::ref(running));
    std::thread ai_thread(ai_stream, std::ref(ai), conf, std::ref(objs), std::ref(frame), std::ref(running));

    api.start(ip, port);
    running = false;
    camera_thread.join();
    ai_thread.join();
}