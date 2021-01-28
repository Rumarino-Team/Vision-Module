# Setup

In order to successfully run the program you will need to install the following;
1. CUDA
2. OpenCV
3. ZED SDK
4. Darknet
5. cpp-httplib
6. nlohmann's json

## Setting up CUDA

If using POP OS:
```
sudo apt-get install system76-cuda-latest system76-cudnn-10.2
```

On any other linux distributions:
[Install CUDA 10.2](https://docs.nvidia.com/cuda/cuda-installation-guide-linux/index.html)
[Install CUDNN for CUDA 10.2](https://docs.nvidia.com/deeplearning/cudnn/install-guide/index.html)

## Setting up OpenCV

In the tools folder there is an included opencvsetup script with everything setup for you:
```
./tools/opencvSetup
```

## Setting up ZED SDK

Head over to the [StereoLabs](https://www.stereolabs.com/developers/release/) website 
and install ZED SDK for CUDA 10.2 and follow the instructions given in 
their [docs](https://www.stereolabs.com/docs/installation/linux/)

## Setting up Darknet

First clone [This Darknet repo](https://github.com/FloppyDisck/darknet)
```bash
git clone https://github.com/FloppyDisck/darknet.git
```

Then compile and copy the files over to local
```bash
make -j5
sudo cp libdarknet.so /usr/local/lib
sudo cp include/yolo_v2_class.hpp /usr/local/include
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

#### Darknet parameters
The only parameter that is required is the yolo_model flag. The rest are optional.

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
