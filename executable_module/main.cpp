#include <pthread.h>
#include <mutex>
#include <atomic>
#include <chrono>
#include <condition_variable>

#include "zedmod/zedmod.hpp"
#include "darknetmod/darknetmod.hpp"
#include "visualobjmod/visualobjmod.hpp"
#include "httpapimod/apimod.hpp"
#include "loggermod/loggermod.hpp"

using json = nlohmann::json;

// Thread variables
std::mutex frame_mutex, obj_mutex;

// Used to handle new frames
// NOTE: new_obj currently not used
std::condition_variable new_frame, new_obj;

void camera_stream(ZED_Camera &cam, Video_Frame &frame, std::atomic<bool> &running){
    // Stop the thread properly
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

void pipeline_stream(PipelineManager &pipelineManager, DetectedObjects &objs, Video_Frame &frame, std::atomic<bool> &running) {
    // Stop the thread properly
    while (running) {
        // Handle new frame
        std::unique_lock<std::mutex> frame_lock(frame_mutex);
        new_frame.wait(frame_lock);
        //Copy frame to prevent longer locking
        Video_Frame frame_copy(frame);
        frame_lock.unlock();

        // Make a new object before locking
        DetectedObjects objects = pipelineManager.detect(frame_copy);

        // Copy created object
        std::unique_lock<std::mutex> obj_lock(obj_mutex);
        objs = objects;
        new_obj.notify_one();
        obj_lock.unlock();
    }
    pipelineManager.close();
    PLOGI << "Stopped AI_STREAM thread.";
}
void print_help() {
    std::cout << "A background artificial intelligence with a custom image processing pipeline.\n\n"
                 "-cfg config, --config config\tA json file that configs the server.\n"<< std::endl;
}

int main(int argc, const char* argv[]) {
    // Zed arguments
    std::shared_ptr<ZED_Camera> cam_ptr;

    // Pipeline Arguments
    Pipeline pipeline;
    bool p_record = false;
    int p_fps = 15;
    std::string p_out;

    // API Arguments
    const char* ip = "0.0.0.0";
    int port = 8080;

    // Logger Arguments
    std::string maxseverity = "info";

    for (int i = 1; i < argc; i++) {
        std::string arg = std::string(argv[i]);
        // Help
        if (arg == "-h" || arg == "--help") {
            print_help();
            return 0;
        }

        // Config
        if (arg == "-cfg" || arg == "--config") {
            std::ifstream cfg_file;
            cfg_file.open(argv[++i]);
            if(!cfg_file) {
                // If CFG file was not found by filestream
                std::cout << "CFG Not found" << std::endl;
                return 0;
            } else {
                std::string cfg_json, line;
                while(!cfg_file.eof()) {
                    getline(cfg_file, line);
                    cfg_json += line;
                }
                // Parse json file to be able to use it comfortably
                json config = json::parse(cfg_json);
                for (json module : config["config"]) {
                    if (module["module"] == "ZED") {
                        if (module["prerecorded"]) {
                            // Prerecorded videos dont require any other type of info
                            std::string z_in = module["inpath"];
                            cam_ptr.reset(new ZED_Camera(z_in));
                        } else {
                            // Live video requires way more information to record
                            sl::RESOLUTION z_res = sl::RESOLUTION::HD1080;
                            int z_fps = module["fps"];
                            bool z_record = module["record"];
                            std::string z_out = module["outpath"];

                            if (module["resolution"] == "1080") {
                                z_res = sl::RESOLUTION::HD1080;
                            }
                            else if (module["resolution"] == "2k") {
                                z_res = sl::RESOLUTION::HD2K;
                            }
                            else if (module["resolution"] == "720") {
                                z_res = sl::RESOLUTION::HD720;
                            }
                            else if (module["resolution"] == "VGA") {
                                z_res = sl::RESOLUTION::VGA;
                            }

                            cam_ptr.reset(new ZED_Camera(z_record, z_res, z_fps, z_out));
                        }


                    }

                    else if (module["module"] == "HTTP") {
                        // We explicitly say that its a string to be able to convert to const char*
                        ip = std::string(module["ip"]).c_str();
                        port = module["port"];
                    }

                    else if (module["module"] == "pipeline") {
                        p_record = module["record"];
                        p_out = module["outpath"];
                        p_fps = module["fps"];
                        for (json pipelineModule : module["pipeline"]) {
                            // We must allocate pipeline items on the heap to prevent losing the data
                            if (pipelineModule["module"] == "darknet") {
                                std::string model = pipelineModule["model"];
                                float confidence = float(pipelineModule["confidence"]) / 100;
                                pipeline.push_back(new DarknetModule(model, confidence));
                            }
                            else if (pipelineModule["module"] == "visual obj") {
                                int red = pipelineModule["color"][0];
                                int blue = pipelineModule["color"][1];
                                int green = pipelineModule["color"][2];
                                cv::Scalar_<double>  color = cv::Scalar(blue, green, red);
                                int thickness = pipelineModule["thickness"];
                                pipeline.push_back(new VisualObjModule(color, thickness));
                            }
                        }
                    }

                    else if (module["module"] == "logger"){
                        maxseverity = module["maxseverity"];
                    }
                }

            }
        }
    }

    // Initialize LOGGER
    initPLOG(maxseverity);

    // Initialize ZED Cam
    PLOGI << "Initializing ZED Camera";
    ZED_Camera& cam = *cam_ptr;
    Video_Frame frame;

    // Initialize AI
    PLOGI << "Initializing AI";
    PipelineManager pipelineManager(pipeline, p_record, p_out, p_fps);
    DetectedObjects objs;

    // Initialize API
    PLOGI << "Initializing API";
    API api(obj_mutex, objs);

    // Use argument variables
    static std::atomic<bool> running = true;

    // Since threads copy arguments we must pass them by reference.
    std::thread camera_thread(camera_stream, std::ref(cam), std::ref(frame), std::ref(running));
    std::thread pipeline_thread(pipeline_stream, std::ref(pipelineManager), std::ref(objs), std::ref(frame), std::ref(running));

    api.start(ip, port);
    running = false;
    camera_thread.join();
    pipeline_thread.join();
}