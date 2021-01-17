# Setup

In order to successfully run the program you will need to install the following;
1. CUDA
2. OpenCV
3. ZED SDK
4. Darknet
5. Darkhelp

## Setting up CUDA

If using POP OS:
```
$ sudo apt install system76-cuda-latest &&sudo apt install system76-cudnn-10.2
```

On any other linux distributions:
[Install CUDA 10.2](https://docs.nvidia.com/cuda/cuda-installation-guide-linux/index.html)
[Install CUDNN for CUDA 10.2](https://docs.nvidia.com/deeplearning/cudnn/install-guide/index.html)

## Setting up OpenCV

In the tools folder there is an included opencvsetup script with everything setup for you:
```
$ ./tools/opencvSetup
```

## Setting up ZED SDK

Head over to the [StereoLabs](https://www.stereolabs.com/developers/release/) website and install ZED SDK for CUDA 10.2 and follow the instructions given in their [docs](https://www.stereolabs.com/docs/installation/linux/)

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

## Compiling the project

Once all the dependencies are setup head over to the root directory and create a build directory where the program will be compiled in:
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