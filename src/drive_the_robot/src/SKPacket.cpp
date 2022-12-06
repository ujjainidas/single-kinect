#include "SKPacket.h"
#include "SKConfig.h"
//#include "MKPPool.h"
#include <opencv2/imgproc.hpp>

using namespace std;

SKPacket::SKPacket(SKWrapper *wrapper) :
    _wrapper(wrapper) {
    
    // TODO pre-initialize CV mats
}

SKWrapper *SKPacket::getSKWrapper(){
    return _wrapper;
}

SKPacket::~SKPacket() {
}

k4a::capture SKPacket::getCapture() {
    return _capture;
}

void SKPacket::setCapture(k4a::capture cap) {
    _capture = cap;
    bgraColorReady = false;
    xyzPointCloudReady = false;
    rgbColorReady = false;
    rgbDepthReady = false;
    rgbColorPreviewReady = false;
} 

k4a::image SKPacket::getBGRAColorKinect() {
    if (!bgraColorReady) {
        PROFILE_START;
        k4a::image colorImageRaw = _capture.get_color_image();
        if (colorImageRaw.get_format() == K4A_IMAGE_FORMAT_COLOR_BGRA32) {
            bgraColor = colorImageRaw;
            PROFILE_END("getBGRAColorSK[copy]");
        } else {
            std::vector<uint8_t> vec(colorImageRaw.get_size());
            memcpy(vec.data(), colorImageRaw.get_buffer(), colorImageRaw.get_size());
            // TODO look into these flags
            cv::Mat *decoded = new cv::Mat(cv::imdecode(vec, cv::IMREAD_ANYDEPTH | cv::IMREAD_COLOR | cv::IMREAD_IGNORE_ORIENTATION));
            cv::cvtColor(*decoded, *decoded, cv::COLOR_BGR2BGRA);
            
            bgraColor = k4a::image::create_from_buffer(K4A_IMAGE_FORMAT_COLOR_BGRA32,
                decoded->cols, decoded->rows, decoded->cols * decoded->elemSize(), decoded->data, decoded->total() * decoded->elemSize(),
                [](void *buf, void *ctx) {delete (cv::Mat *)ctx;}, decoded);
            PROFILE_END("getBGRAColorSK[convert]");
        }
        bgraColorReady = true;
    }
    return bgraColor;
}

cv::Mat SKPacket::Depth() {
    if (!rgbDepthReady) {
        PROFILE_START;
        k4a::image depthImage = _capture.get_depth_image();
        k4a::image colorDepthImage = k4a::image::create(K4A_IMAGE_FORMAT_COLOR_BGRA32, depthImage.get_width_pixels(), depthImage.get_height_pixels(),
            depthImage.get_width_pixels() * 4 * sizeof(int8_t));
        _wrapper->transformation.color_image_to_depth_camera(depthImage, getBGRAColorKinect(), &colorDepthImage);
        
        cv::Mat cv_bgra = cv::Mat(colorDepthImage.get_height_pixels(), colorDepthImage.get_width_pixels(), CV_8UC4);
        memcpy(cv_bgra.data, colorDepthImage.get_buffer(), cv_bgra.rows * cv_bgra.cols * cv_bgra.elemSize());

        cv::cvtColor(cv_bgra, rgbDepth, cv::COLOR_BGRA2RGB);
        rgbDepthReady = true;
        PROFILE_END("getRGBDepth");
    }
    return rgbDepth;
}

cv::Mat SKPacket::getRGBColor() {
    if (!rgbColorReady) {
        PROFILE_START;
        k4a::image image = _capture.get_color_image();
        uint8_t *buffer = image.get_buffer();

        cv::Mat mat;
        if (image.get_format() == K4A_IMAGE_FORMAT_COLOR_BGRA32) {
            mat = cv::Mat(image.get_height_pixels(), image.get_width_pixels(), CV_8UC4);
            memcpy(mat.data, buffer, image.get_width_pixels() * image.get_height_pixels() * 4);
            cv::cvtColor(mat, mat, cv::COLOR_BGRA2RGB);
            PROFILE_END("getRGBColor[bgra]");
        } else {
            std::vector<uint8_t> vec(image.get_size());
            memcpy(vec.data(), buffer, image.get_size());
            mat = cv::imdecode(vec, cv::IMREAD_ANYDEPTH | cv::IMREAD_COLOR | cv::IMREAD_IGNORE_ORIENTATION);
            cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
            PROFILE_END("getRGBColor[mjpg]");
        }
        rgbColor = mat;
        rgbColorReady = true;
    }

    //cv::imshow("PREVIEW", rgbColor);
    //cv::waitKey(1);

    //cout << "HI" << endl;

    return rgbColor;
}

cv::Mat SKPacket::getRGBColorPreviewScale() {
    if (!rgbColorPreviewReady) {
        cv::resize(getRGBColor(), rgbColorPreview, cv::Size(960, 540));
        rgbColorPreviewReady = true;
    }
    return rgbColorPreview;
}

k4a::image SKPacket::getXYZDepth() {
    if (!xyzPointCloudReady) {
        k4a::image depth_image = _capture.get_depth_image();

        if (!xyzPointCloud) {
            xyzPointCloud = k4a::image::create(K4A_IMAGE_FORMAT_CUSTOM, depth_image.get_width_pixels(), depth_image.get_height_pixels(), depth_image.get_width_pixels() * 3 * sizeof(int16_t));
        }
        _wrapper->transformation.depth_image_to_point_cloud(depth_image, K4A_CALIBRATION_TYPE_DEPTH, &xyzPointCloud);
        xyzPointCloudReady = true;
    }
    return xyzPointCloud;
}

void SKPacket::allocateCVMat(int rows, int cols, int format, std::string name) {
    cv::Mat mat = cv::Mat(rows, cols, format);
    _namedMats[name] = mat;
}

cv::Mat &SKPacket::getCVMat(std::string name) {
    return _namedMats[name];
}


SKConfig &SKPacket::getConfig() {
    return _wrapper->getSKConfig();
}

k4abt::frame SKPacket::getBodyFrame() {
    return _bodyFrame;
}
void SKPacket::setBodyFrame(k4abt::frame frame) {
    _bodyFrame = frame;
}

Eigen::MatrixXd SKPacket::getAprilTagMatrix() {
    return cornerDetections["apriltag"];
}
