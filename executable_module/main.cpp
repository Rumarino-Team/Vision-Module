#include <pthread.h>
#include <mutex>
#include <atomic>
#include <chrono>
#include <condition_variable>

//Server imports
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#include "zedmod/zedmod.hpp"
#include "aimod/aimod.hpp"
#include "httpapimod/apimod.hpp"

// Thread variables
std::mutex frame_mutex, obj_mutex;

// Used to handle new frames
// NOTE: new_obj currently not used
std::condition_variable new_frame, new_obj;

//Api global variable and type
int volatile sockfd;
typedef struct obj_t {
    int x;
    int y;
    int w; 
    int h;
    int id;
    const char* name;
    float distance;
}__attribute__((packed)) obj;

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
        new_obj.notify_one();
        obj_lock.unlock();
    }
    ai.close();
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
    std::cout << "\n--Finally define server params--" << std::endl;
    std::cout << "[OPTIONAL] -ip" << std::endl;
    std::cout << "\t-ip 0.0.0.0" << std::endl;
    std::cout << "[OPTIONAL] -p    --port" << std::endl;
    std::cout << "\t-p 8080" << std::endl;
}

obj toObj(DetectedObject o) {
    int x = o.bounding_box.x;
    int y = o.bounding_box.y;
    int width = o.bounding_box.width;
    int height = o.bounding_box.height;

    obj data = {x, y, width, height, o.id, o.name, o.distance};

    return data;
}

int getRoute (int sock, int debugLevel)
{
    //Setting variables to read socket
    int n;
    // char buffer[1024];
    // bzero(buffer,1024);

    int route;

    //Read socket
    n = read(sock, &route, sizeof(route));

    if (n < 0) {
        if (debugLevel >= 1) {
            //Printf for now
            printf("ERROR: reading from socket\n");
        }
    }

    //Here we do api routing

    //Print api call
    if (debugLevel >= 2) {
        //Printf for now
        printf("INFO: API Call -> %d\n", route);
    }

    //Convert buffer to int for routing
    return route;

    // //Handy Dandy Api Routing Function
    // execAPI(route, sock, );
}


void socket_api_stream(DetectedObjects &objs, std::atomic<bool> &running, int debugLevel) {
    //Set up server first
    
    // Socket boilerplate
    int newsockfd, portno, pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    

    //Hard code port for now
    portno = 8080;


    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
       if (debugLevel >= 1) {
           //Printf for now
           printf("ERROR: error on binding\n");
       }
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    pid = getpid();
    if (pid > 0) {
        // signal(SIGINT, SIGINT_handler);

        //This is to avoid zombie processes in the server
        signal(SIGCHLD, SIG_IGN);
    }

    if (debugLevel >= 2) {
        printf("INFO: Server Started!\n");
        printf("INFO: Port -> %d\n", portno);
        fflush(stdout);
    }

    while (running) {

        // Block until socket is written to
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        
        if (newsockfd < 0) {
            if (debugLevel >= 1) {
                //Printf for now
                printf("ERROR: error on accept\n");
            }
        }

        //Fork new child
        pid = fork();
        
        if (pid < 0) {
            if (debugLevel >= 1) {
                //Printf for now
                printf("ERROR: error on fork\n");
            }
        }
        
        //Let the child handle the request
        if (pid == 0) {
            close(sockfd);
            int route = getRoute(newsockfd, debugLevel);


            //We got the route number, so do magic now...

            //Lock obj so it doesn't change whist sending it to client
            std::unique_lock<std::mutex> obj_lock(obj_mutex);
            
            //Indicates size of struct to send
            int header;
            int numObjs = sizeof(objs) / sizeof(DetectedObject);
            obj objects[numObjs];

            if (numObjs > 0) {
                for (int i = 0; i < numObjs; i++) {
                    objects[i] = toObj(objs[i]);
                }
            }

            // obj *objectPointer;
            // objectPointer = objects;
            // int header = sizeof(objectPointer);
            header = sizeof(objects);

            if (debugLevel >= 2) {
                printf("INFO: Size of struct to send -> %ld\n", header);
            }

            //Send struct size to client for parity checking
            if (write(newsockfd, &header, sizeof(header)) < 0) {
                if (debugLevel >= 1) {
                    printf("ERROR: error writing to socket\n");
                }
            }

            //Send thee data
            if (write(newsockfd, &objects, sizeof(objects)) < 0) {
                if (debugLevel >= 1) {
                    printf("ERROR: error writing to socket\n");
                }
            }

            if (debugLevel >= 2) {
                printf("INFO: data sent\n");
            }

            obj_lock.unlock();

            exit(0);
        }

        //Parent closes connenction to new socket. Only the child should deal with the new socket
        else close(newsockfd);
        
    }

    //After running, we must close parent socket file descriptor
    close(sockfd);
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
    
    // API Arguments
    const char* ip = "0.0.0.0";
    int port = 8080;
    int debugLevel = 2;

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
        else if (arg == "-ip" || arg == "--ip") {
            ip = argv[++i];
        }
        else if (arg == "-p" || arg == "--port") {
            port = std::stoi(std::string(argv[++i]));
        }
    }

    if(model.empty()) {
        std::cout << "No YOLO model specified!" << std::endl;
        return 0;
    }

    // Initialize ZED Cam
    std::shared_ptr<ZED_Camera> cam_ptr;
    if (live_zed) {
        cam_ptr.reset(new ZED_Camera(z_record, z_res, z_fps, z_out));
    } else {
        cam_ptr.reset(new ZED_Camera(z_in));
    }

    // ZED_Camera& cam = *cam_ptr;
    ZED_Camera cam(false);
    Video_Frame frame;

    // Initialize AI
    // AI ai(model, m_record, m_out, m_fps);
    AI ai("../test/media/RUBBER-DUCKY", true, "ai_output.avi", 15);
    DetectedObjects objs;
    float conf = float(confidence_percent) / 100;

    // Initialize API
    // API api(obj_mutex, objs);

    // Use argument variables
    static std::atomic<bool> running = true;

    // Since threads copy arguments we must pass them by reference.
    std::thread camera_thread(camera_stream, std::ref(cam), std::ref(frame), std::ref(running));
    std::thread ai_thread(ai_stream, std::ref(ai), conf, std::ref(objs), std::ref(frame), std::ref(running));
    std::thread socket_api_thread(socket_api_stream, std::ref(objs), std::ref(running), std::ref(debugLevel));


    //TO DO: Make socket api be the one that stops the executable
    // api.start(ip, port);
    // running = false;

    camera_thread.join();
    ai_thread.join();
    socket_api_thread.join();
}