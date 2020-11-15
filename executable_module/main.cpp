#include <pthread.h>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "zedmod/zedmod.hpp"
#include "aimod/aimod.hpp"
#include "apimod/apimod.hpp"

void camera_stream(ZED_Camera &cam, Video_Frame &frame, std::mutex &frame_mutex, std::condition_variable &new_frame, std::atomic<bool> &running){
    // Stop the thread properly
    while (running) {
        std::unique_lock<std::mutex> lock(frame_mutex);
        frame = cam.update();
        lock.unlock();
        new_frame.notify_one();
    }
    cam.close();
}

void ai_stream(AI &ai, std::vector<DetectedObject> &objs, float confidence, Video_Frame &frame, std::mutex &obj_mutex, std::mutex &frame_mutex, std::condition_variable &new_obj, std::condition_variable &new_frame, std::atomic<bool> &running) {
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

}