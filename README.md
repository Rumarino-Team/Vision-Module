# Setup

In order to successfully run the program you will need to install the following;
1. CUDA
2. OpenCV
3. ZED SDK
4. YAML-cpp
5. TensorRT Inference Model
6. cpp-httplib
7. nlohmann's json

## Setting up CUDA

Cuda is a parallel computing platform created for certain Nvidia GPU's that set an enviroment for
optimizing the computing process of the machine. This module will be running behind the scenes in the following modules:
Zed SDK, TensorRT and OpenCV. Without a proper machine that can run this framework the module will not run.

If using POP OS you can install it with:
```
sudo apt-get install system76-cuda-latest system76-cudnn-10.2
```

On any other linux distributions:
[Install CUDA 10.2](https://docs.nvidia.com/cuda/cuda-installation-guide-linux/index.html)
[Install CUDNN for CUDA 10.2](https://docs.nvidia.com/deeplearning/cudnn/install-guide/index.html)

## Setting up OpenCV
OpenCV is the framework that is used for modifying the Images.
In the tools folder there is an included opencvsetup script with everything setup for you:
```
./tools/opencvSetup
```
You will only need to execute the file in the bash and it will dowload the dependencies.

## Setting up ZED SDK

This is the core of the module. This module will let us connect with the camera and extract
all the information that we will need.

Head over to the [StereoLabs](https://www.stereolabs.com/developers/release/) website 
and install ZED SDK for CUDA 10.2 and follow the instructions given in 
their [docs](https://www.stereolabs.com/docs/installation/linux/)

## YAML-Cpp
This dependencie is neccesary for reading the YAML file that will be passed
to the TensorRT Inference model.
First you can Dowload the repo from here.
```bash
git clone https://github.com/jbeder/yaml-cpp.git
```
Then compile it with cmake.
```bash
mkdir build && cd build
cmake ..
make -j
```

## Setting up TensorRT Yolo Inference model

This is the Inference model that we are gonna used for making the detections.
We will using the Yolov7 Detector from this repo that has already make the wrapper of the onnx
model for us. Please revise the [example](https://github.com/linghu8812/tensorrt_inference/tree/master/example) project from the repo. Don't worry if the example is using
the yolov5 model the code for both model are the same(What change is the onnx model previous trained).

**Important**: The model also need the TensorRT package for it to work but if you have installed all previous
 dependencies correcty when installing the Zed SDK it must have already install this package.


First clone this [TensorRT repo](https://github.com/linghu8812/tensorrt_inference)
```bash
git clone https://github.com/linghu8812/tensorrt_inference.git
```

Then compile and copy the files over to local

```bash
cd tensorrt_inference/project
mkdir build && cd build
cmake ..
make -j
mkdir libs build
cp ../bin/libyolov7.so ./libs/
```

## Setting up the API dependencies

Install [cpp-httplib](https://github.com/yhirose/cpp-httplib)
```bash
git clone https://github.com/yhirose/cpp-httplib.git
cd cpp-httplib
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DHTTPLIB_COMPILE=on -DBUILD_SHARED_LIBS=on ..
sudo cmake --build . --target install
```

Install [nlohmann's json](https://github.com/nlohmann/json)
```bash
git clone https://github.com/nlohmann/json.git
cd json
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
sudo make install -j5
```

## Compiling the project

Once all the dependencies are setup head over to the root directory and create a build 
directory where the program will be compiled in:
```bash
mkdir build-release
cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Testers are automatically compiled when using Debug build type
```bash
mkdir build-debug
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

Copy the executable over to your local folder for ease of use
```bash
sudo cp /executable_module/vision_http_server /usr/local/bin/
```

## Running the executable

The executable supports various arguments that modify its behavior.

```bash
# To display a help page
vision_http_server [-h, --help]
```

### Argument descriptions

If you already tried to look at the help page you'll notice that the executable has plenty 
of different arguments, in the following sections these will be explained with more clarity.

#### ZED Camera parameters
The following parameters are all optional, if no arguments are specified the ZED feed will 
be live at 1080p resolution and 30 frames per second.

The following arguments are for the ZED live feed.
```bash
# Tell the ZED camera to start recording the live feed and save it in the specified file
[-zr, --zed_record] ${video_ouput}

# Specify the feed resolution
[-res, --resolution] ${resolution}

# Specify the feed frame rate
[-zfps, --zed_fps] ${fps}
```

The following are for pre-recorded feeds
```bash
# Point to the pre-recorded svo file
[-zp, --zed_play] ${SVO_file}
```

#### Yolov7 parameters
The only parameter that is required is the yaml file flag. The rest are optional.

```bash
# Required parameter that points to the pre-trained model folder
[-m, --yolo_model] ${model_path}

# Enable recording the model's output and output
[-mr, --model_record] ${video_output}

# Specify the recording's target framerate
[-mfps, --model_fps] ${fps}

# Target confidence percentage for the model's detection
[-c, --confidence] ${confidence}
```

#### Server parameters
The server by default is initialized at 0.0.0.0:8080, but you can change this
if you please.

```bash
# The ip in which the server can be reached
-ip ${ip}

# The port in which the server will be listening at
[-p, --port] ${port}
```

#### Using the Python library
For a general explanation of how the library works refer to the example implementation inside the library itself.

The kwargs are the same as the verbose versions of the executable args without the "--" prefix.
