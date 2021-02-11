
#include <pybind11/pybind11.h>
// #include "apimod/apimod.hpp"
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

typedef struct obj {
    int x;
    int y;
    int w; 
    int h;
    int obj_id;
    const char* obj_name;
    float distance;
}__attribute__((packed)) obj;

namespace py = pybind11;

PYBIND11_MODULE(apimod, mod) {
    mod.def(
        "testImport",
        [](std::string msg) {
            return msg;
        },
        "Used to know if import worked correctly",
        py::arg("msg")
    );
    mod.def(
        "getObjects",
        [](std::string host, std::string port) {
            
            int sockfd, portno, n;
            struct sockaddr_in serv_addr;
            struct hostent *server;

            portno = atoi(port.c_str());
            sockfd = socket(AF_INET, SOCK_STREAM, 0);

            server = gethostbyname(host.c_str());

            bzero((char *) &serv_addr, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            bcopy((char *)server->h_addr, 
                (char *)&serv_addr.sin_addr.s_addr,
                server->h_length);
            serv_addr.sin_port = htons(portno);


            int getObjectsRoute = 2;

            n = write(sockfd, &getObjectsRoute, sizeof(getObjectsRoute));
            
            int header;
            n = read(sockfd, &header, sizeof(header));
            
            obj objects[header / sizeof(obj)];            

            n = read(sockfd, &objects, sizeof(obj));

            close(sockfd);
            
            //Initialize python list        
            py::list convertedObjs;

            int length = sizeof(objects) / sizeof(obj);

            //Iterate objects called in vector v
            for (int i = 0; i < length; i++) {

                //Typecast all attributes to python types
                py::int_ x(objects[i].x);
                py::int_ y(objects[i].y);
                py::int_ w(objects[i].w);
                py::int_ h(objects[i].h);
                py::int_ id(objects[i].obj_id);
                std::string name(objects[i].obj_name);
                py::float_ distance(objects[i].distance);
                
                //Initialize temporary list for object attributes
                py::list object;

                //Append python type typecasted attributes to temp list
                object.append(x);
                object.append(y);
                object.append(w);
                object.append(h);
                object.append(id);
                object.append(name);
                object.append(distance);

                //Append temp list to objects list
                convertedObjs.append(object);
            }

            //Return our glorious objects array
            return convertedObjs;

        },
        "Used to know if import worked correctly",
        py::arg("host"),
        py::arg("port")
    );
//     mod.def(
//     "getObjectsTest",
//     [](std::string ai_path, float confidence, std::string video_path) {

//         //Initialize the all mighty AI
//         AI ai(ai_path);

//         //Initialize zed camera and get current frame
//         cv::VideoCapture cap(video_path);
//         int totalFrames = cap.get(cv::CAP_PROP_FRAME_COUNT);
//         int min = 1;

//         //Get random frame within total number of frames
//         int range = totalFrames - min + 1;
//         int randomFrame = rand() % range + min;

//         //Set cv capture to random frame
//         cap.set(cv::CAP_PROP_POS_FRAMES, randomFrame);

//         //Initialize and obtain frame data from video capture
//         cv::Mat img;
//         cap.read(img);

//         //Call the object detection AI
//         std::vector<DetectedObject> v = ai.detect(img, confidence);

//         //Initialize python list        
//         py::list objects;

//         //Iterate objects called in vector v
//         for (DetectedObject &obj : v) {

//             //Typecast all attributes to python types
//             py::int_ x(obj.bounding_box.x);
//             py::int_ y(obj.bounding_box.y);
//             py::int_ w(obj.bounding_box.width);
//             py::int_ h(obj.bounding_box.height);
//             py::int_ id(obj.id);
//             std::string name(obj.name);
//             py::float_ distance(obj.distance);
            
//             //Initialize temporary list for object attributes
//             py::list object;

//             //Append python type typecasted attributes to temp list
//             object.append(x);
//             object.append(y);
//             object.append(w);
//             object.append(h);
//             object.append(id);
//             object.append(name);
//             object.append(distance);

//             //Append temp list to objects list
//             objects.append(object);
//         }

//         //Return our glorious objects array
//         return objects;
//     },
//     "Returns objects from random frame in video file",
//     py::arg("ai_path"),
//     py::arg("confidence"),
//     py::arg("video_path")

//   );
//   mod.def(
//     "getObjects",
//     [](std::string ai_path, float confidence) {
        
//         //Initialize the all mighty AI
//         // AI ai(ai_path);
//         // std::string x = "SS";
//         // AI ss(x);

//         //Initialize zed camera and get current frame
//         // ZED_Camera cam(false);
//         // cv::Mat frame = cam.update().image;

//         // //Call the object detection AI
//         // std::vector<DetectedObject> v = ai.detect(frame, confidence);

//         // //Close Zed camera
//         // cam.close();

//         // //Initialize python list        
//         // py::list objects;

//         // //Iterate objects called in vector v
//         // for (DetectedObject &obj : v) {

//         //     //Typecast all attributes to python types
//         //     py::int_ x(obj.bounding_box.x);
//         //     py::int_ y(obj.bounding_box.y);
//         //     py::int_ w(obj.bounding_box.width);
//         //     py::int_ h(obj.bounding_box.height);
//         //     py::int_ id(obj.id);
//         //     std::string name(obj.name);
//         //     py::float_ distance(obj.distance);
            
//         //     //Initialize temporary list for object attributes
//         //     py::list object;

//         //     //Append python type typecasted attributes to temp list
//         //     object.append(x);
//         //     object.append(y);
//         //     object.append(w);
//         //     object.append(h);
//         //     object.append(id);
//         //     object.append(name);
//         //     object.append(distance);

//         //     //Append temp list to objects list
//         //     objects.append(object);
//         // }

//         // //Return our glorious objects array
//         // return objects;
//         return "s";
//     },
//     "Obtains objects from the AI Module",
//     py::arg("ai_path"),
//     py::arg("confidence")
//   );
}
