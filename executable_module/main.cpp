#include <pthread.h>
#include <mutex>
#include <atomic>
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
    while (running) {
        Video_Frame frame_update = cam.update();
        std::unique_lock<std::mutex> lock(frame_mutex);
        frame.copy(frame_update);
        lock.unlock();
        new_frame.notify_one();
    }
    cam.close();
}

void ai_stream(AI &ai, float confidence, DetectedObjects &objs, Video_Frame &frame, std::atomic<bool> &running) {
    // Stop the thread properly
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
        obj_lock.unlock();
        new_obj.notify_one();
    }
    ai.close();
}

int main(int argc, char* argv[]) {
    //TODO: use arguments to initialize the ai and zed
    ZED_Camera cam(false);
    Video_Frame frame;

    AI ai("../test/media/RUBBER-DUCKY", true, "ai_output.avi");
    DetectedObjects objs;
    float conf = 0.66;
    // Use argument variables
    static std::atomic<bool> running = true;

    //Initialize API
    API api(obj_mutex, objs);

    // Since threads copy arguments we must pass them by reference.
    std::thread camera_thread(camera_stream, std::ref(cam), std::ref(frame), std::ref(running));
    std::thread ai_thread(ai_stream, std::ref(ai), conf, std::ref(objs), std::ref(frame), std::ref(running));

    while(true) {
        // API cases go here
        api.start("0.0.0.0", 8080);

        running = false;
        camera_thread.join();
        ai_thread.join();
        break;
    }
}