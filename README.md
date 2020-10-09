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

First clone [AleceyAB's Darknet repo](https://github.com/AlexeyAB/darknet)
```
$ git clone https://github.com/AlexeyAB/darknet.git
```

Inside open the included Makefile and change GPU, CUDNN and OPENCV to 1 for better GPU performance. Also set LIBSO flag to 1 since we'll need a library to be built in order to use the program in out repo.

Once this is done you must go over to line 66 and replace the NVCC variable with
``` 
NVCC=/usr/local/cuda/bin/nvcc
```

Then just do
```
$ make
```
and Darknet should compile.

## Setting up Darkhelp

Darkhelp is a C++ wrapper for Darknet that also changes its image handling type to officially use OpenCV.
To install follow [these instructions](https://www.ccoderun.ca/darkhelp/api/Building.html#Linux).

## Compiling the project

Once all the dependencies are setup head over to the root directory and create a build directory where the program will be compiled in:
```
$ mkdir build
$ cd build
$ cmake ..
```