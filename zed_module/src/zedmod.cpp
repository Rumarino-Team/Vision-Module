#include "zedmod/zedmod.hpp"

/**
 * Convert Zed images to OpenCV images
 *
 * @param input Zed image to be converted
 * @return OpenCV image virtually identical to the Zed one
 */
cv::Mat zedMat2cvMat(sl::Mat input) {
    // Mapping between MAT_TYPE and CV_TYPE
    int cv_type = -1;
    switch (input.getDataType()) {
        case sl::MAT_TYPE::F32_C1: cv_type = CV_32FC1; break;
        case sl::MAT_TYPE::F32_C2: cv_type = CV_32FC2; break;
        case sl::MAT_TYPE::F32_C3: cv_type = CV_32FC3; break;
        case sl::MAT_TYPE::F32_C4: cv_type = CV_32FC4; break;
        case sl::MAT_TYPE::U8_C1: cv_type = CV_8UC1; break;
        case sl::MAT_TYPE::U8_C2: cv_type = CV_8UC2; break;
        case sl::MAT_TYPE::U8_C3: cv_type = CV_8UC3; break;
        case sl::MAT_TYPE::U8_C4: cv_type = CV_8UC4; break;
        default: break;
    }

    // Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
    // cv::Mat and sl::Mat will share a single memory structure
    return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(sl::MEM::CPU));
}

/**
 * Returns a verbose version of the given Zed ERROR_CODE
 *
 * @param error Zed error code
 * @return A verbose string of the given error code
 */
const char* ErrorToString(sl::ERROR_CODE error) {
    // There is no pretty way of doing this...
    switch (error) {
        case sl::ERROR_CODE::FAILURE: return "Unsuccessful behavior.";
        case sl::ERROR_CODE::NO_GPU_COMPATIBLE : return "No GPU found or CUDA capability of the device is not supported.";
        case sl::ERROR_CODE::NOT_ENOUGH_GPU_MEMORY : return "Not enough GPU memory for this depth mode, try a different mode (such as PERFORMANCE), or increase the minimum depth value (see InitParameters::depth_minimum_distance).";
        case sl::ERROR_CODE::CAMERA_NOT_DETECTED : return "The ZED camera is not plugged or detected.";
        case sl::ERROR_CODE::SENSORS_NOT_AVAILABLE : return "A ZED-M or ZED2 camera is detected but the sensors (imu,barometer...) cannot be opened. Only for ZED-M or ZED2 devices.";
        case sl::ERROR_CODE::INVALID_RESOLUTION : return "Invalid resolution parameter.";
        case sl::ERROR_CODE::LOW_USB_BANDWIDTH : return "Using multiple ZED or a USB 2.0 port (bandwidth issue).";
        case sl::ERROR_CODE::CALIBRATION_FILE_NOT_AVAILABLE : return "ZED calibration file is not found on the host machine. Use ZED Explorer or ZED Calibration to get one.";
        case sl::ERROR_CODE::INVALID_CALIBRATION_FILE : return "ZED calibration file is not valid, try to download the factory one or recalibrate your camera using 'ZED Calibration'.";
        case sl::ERROR_CODE::INVALID_SVO_FILE : return "The provided SVO file is not valid.";
        case sl::ERROR_CODE::SVO_RECORDING_ERROR : return "An recorder related error occurred (not enough free storage, invalid file).";
        case sl::ERROR_CODE::SVO_UNSUPPORTED_COMPRESSION : return "NVIDIA based compression cannot be loaded.";
        case sl::ERROR_CODE::END_OF_SVOFILE_REACHED : return "SVO end of file has been reached, and no frame will be available until the SVO position is reset.";
        case sl::ERROR_CODE::INVALID_COORDINATE_SYSTEM : return "The requested coordinate system is not available.";
        case sl::ERROR_CODE::INVALID_FIRMWARE : return "The firmware of the ZED is out of date. Update to the latest version.";
        case sl::ERROR_CODE::INVALID_FUNCTION_PARAMETERS : return "An invalid parameter has been set for the function.";
        case sl::ERROR_CODE::CUDA_ERROR : return "A CUDA error has been detected in the process. Activate verbose in sl::Camera::open for more info.";
        case sl::ERROR_CODE::CAMERA_NOT_INITIALIZED : return "ZED SDK is not initialized. Probably a missing call to sl::Camera::open.";
        case sl::ERROR_CODE::NVIDIA_DRIVER_OUT_OF_DATE : return "Your NVIDIA driver is too old and not compatible with your current CUDA version.";
        case sl::ERROR_CODE::INVALID_FUNCTION_CALL : return "The call of the function is not valid in the current context. Could be a missing call of sl::Camera::open.";
        case sl::ERROR_CODE::CORRUPTED_SDK_INSTALLATION : return "The SDK wasn't able to load its dependencies or somes assets are missing, the installer should be launched.";
        case sl::ERROR_CODE::INCOMPATIBLE_SDK_VERSION : return "The installed SDK is incompatible SDK used to compile the program.";
        case sl::ERROR_CODE::INVALID_AREA_FILE : return "The given area file does not exist, check the path.";
        case sl::ERROR_CODE::INCOMPATIBLE_AREA_FILE : return "The area file does not contain enought data to be used or the sl::DEPTH_MODE used during the creation of the area file is different from the one currently set.";
        case sl::ERROR_CODE::CAMERA_FAILED_TO_SETUP : return "Failed to open the camera at the proper resolution. Try another resolution or make sure that the UVC driver is properly installed.";
        case sl::ERROR_CODE::CAMERA_DETECTION_ISSUE : return "Your ZED can not be opened, try replugging it to another USB port or flipping the USB-C connector.";
        case sl::ERROR_CODE::CANNOT_START_CAMERA_STREAM : return "Cannot start camera stream. Make sure your camera is not already used by another process or blocked by firewall or antivirus.";
        case sl::ERROR_CODE::NO_GPU_DETECTED : return "No GPU found, CUDA is unable to list it. Can be a driver/reboot issue.";
        case sl::ERROR_CODE::PLANE_NOT_FOUND : return "Plane not found, either no plane is detected in the scene, at the location or corresponding to the floor, or the floor plane doesn't match the prior given.";
        case sl::ERROR_CODE::MODULE_NOT_COMPATIBLE_WITH_CAMERA : return "The Object detection module is only compatible with the ZED 2.";
        case sl::ERROR_CODE::MOTION_SENSORS_REQUIRED : return "The module needs the sensors to be enabled (see InitParameters::disable_sensors).";
    }
}

ZED_Camera::ZED_Camera(bool record, const std::string &recording_out, Video_Quality quality) {
    sl::RESOLUTION res;
    int fps;

    switch (quality) {
        case Video_Quality::HD2K_15fps: res = sl::RESOLUTION::HD2K; fps=15; break;
        case Video_Quality::HD1080_15fps: res = sl::RESOLUTION::HD1080; fps=15; break;
        case Video_Quality::HD1080_30fps: res = sl::RESOLUTION::HD1080; fps=30; break;
        case Video_Quality::HD720_15fps: res = sl::RESOLUTION::HD720; fps=15; break;
        case Video_Quality::HD720_30fps: res = sl::RESOLUTION::HD720; fps=30; break;
        case Video_Quality::HD720_60fps: res = sl::RESOLUTION::HD720; fps=60; break;
        case Video_Quality::VGA_15fps: res = sl::RESOLUTION::VGA; fps=15; break;
        case Video_Quality::VGA_30fps: res = sl::RESOLUTION::VGA; fps=30; break;
        case Video_Quality::VGA_60fps: res = sl::RESOLUTION::VGA; fps=60; break;
        case Video_Quality::VGA_100fps: res = sl::RESOLUTION::VGA; fps=100; break;
    }

    this->init(record, std::string(), recording_out, res, fps);
}

ZED_Camera::ZED_Camera(bool record, sl::RESOLUTION res, int fps, const std::string &recording_out) {
    this->init(record, std::string(), recording_out, res, fps);
}

ZED_Camera::ZED_Camera(const std::string &playback_video) {
    this->init(false, playback_video, std::string(), sl::RESOLUTION::HD1080, 30);
}

void ZED_Camera::init(bool record, const std::string &playback_video, const std::string &recording_out, sl::RESOLUTION res, int fps) {
    zed = sl::Camera();

    // Conf params
    sl::InitParameters init_params;
    init_params.sdk_verbose = true; // Use for the debug flag.
    // Init depth sensing
    init_params.depth_mode = sl::DEPTH_MODE::ULTRA;
    init_params.coordinate_units = sl::UNIT::METER;
    init_params.depth_stabilization = true;

    // Choose either opening a camera stream or video stream
    if (playback_video.empty()) {
        init_params.camera_resolution = res;
        init_params.camera_fps = fps;

    } else {
        init_params.input.setFromSVOFile(playback_video.c_str());
        init_params.svo_real_time_mode = true; // This will allow the video to play realtime
    }

    // Check if it opens, it not print error
    try {
        sl::ERROR_CODE err = zed.open(init_params);
        if (err != sl::ERROR_CODE::SUCCESS) {
            throw err;
        }
    }
    catch (sl::ERROR_CODE error) {
        std::cout << "An exception occurred." << ErrorToString(error) << '\n';
    }

    // If recording enabled activate it
    recording = record;
    if (recording) {
        sl::RecordingParameters record_params;
        record_params.video_filename = recording_out.c_str();
        record_params.target_framerate = fps;
        record_params.compression_mode = sl::SVO_COMPRESSION_MODE::H264;
        zed.enableRecording(record_params);
    }
}

ZED_Camera::~ZED_Camera() { this->close(); }

Video_Frame ZED_Camera::update() {
    Video_Frame new_frame;
    sl::Mat left_image, depth_map, point_cloud;
    if (zed.grab() == sl::ERROR_CODE::SUCCESS) {
        // Since the following functions just copy over to the given variable
        // We must first initialize a sl::Mat to then convert over to  cv::Mat
        zed.retrieveImage(left_image, sl::VIEW::LEFT);
        zed.retrieveMeasure(depth_map, sl::MEASURE::DEPTH);
        zed.retrieveMeasure(point_cloud, sl::MEASURE::XYZ);
        new_frame.image = zedMat2cvMat(left_image);
        new_frame.depth_map = zedMat2cvMat(depth_map);
        new_frame.point_cloud = zedMat2cvMat(point_cloud);
    }
    else if (zed.grab() == sl::ERROR_CODE::END_OF_SVOFILE_REACHED) {
        std::cout << "SVO end has been reached. Looping back to first frame" << std::endl;
        zed.setSVOPosition(0);
        new_frame = this->update(); // Avoid returning an empty object
    }

    return new_frame;
}

void ZED_Camera::close() {
    if (recording) {
        zed.disableRecording();
    }
    zed.close();
    zed.~Camera();
}
