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
make -j5
```

Testers are automatically compiled when using Debug build type
```bash
mkdir build-debug
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j5
```

Copy the executable over to your local folder for ease of use
```bash
sudo cp /executable_module/vision_http_server /usr/local/bin/
```

## Running the executable

```bash
# To display a help page
vision_http_server [-h, --help]
# To run with configs
vision_http_server -cfg [path_to_file]
```

### Executable configuration

 To see an example configuration check out /executable_module/config-example.json

### Using the Python library

For a general explanation of how the library works refer to the example implementation inside the library itself.

The constructor takes the configuration json path and the executable name.

```python
server = VisionStream("json/path")

server.start()
server.wait()

server.get_objects()
```

If a server is already running with that IP and Port then it will auto-connect instead or running a new instance.
