#ifndef SK_PACKET_H
#define SK_PACKET_H

#include <Eigen/Eigen>
#include <k4a/k4a.h>
#include <k4abt.hpp>

#include <opencv2/opencv.hpp>

#include <map>
#include <string>

#include <cstdlib>
#include "SKWrapper.h"
#include "Profiling.h"

class SKConfig;
class SKWrapper;

class SKPacket {
public:
    SKPacket(SKWrapper *wrapper);
    ~SKPacket();

    /* standard MKP functionality */
    k4a::capture getCapture();
    void setCapture(k4a::capture capture);
    cv::Mat getRGBColor();
    cv::Mat getRGBColorPreviewScale();
    cv::Mat getRGBDepth();
    cv::Mat Depth();
    void getBodyFrameValues();
    k4a::image getXYZDepth();
    Eigen::MatrixXd getAprilTagMatrix();

    void allocateCVMat(int rows, int cols, int format, std::string name);
    cv::Mat &getCVMat(std::string name);

    k4abt::frame getBodyFrame();
    void setBodyFrame(k4abt::frame frame);

    /* accessors */
    SKConfig &getConfig();
    SKWrapper *getSKWrapper();

    std::map<std::string, Eigen::MatrixXd> cornerDetections;

    std::vector<k4abt_body_t> solvedBodies;
    std::map<size_t, std::map<uint32_t, uint32_t>> _localToGlobalBodyIds;

protected:
    SKWrapper *_wrapper;
    std::map<std::string, cv::Mat> _namedMats;

private:
    k4a::image getBGRAColorKinect();

    k4a::capture _capture;
    k4a::image bgraColor;
    bool bgraColorReady = false;
    k4a::image xyzPointCloud;
    bool xyzPointCloudReady = false;
    k4abt::frame _bodyFrame;
    std::vector<k4abt_skeleton_t> _skeletons;

    cv::Mat rgbColor;
    bool rgbColorReady = false;
    cv::Mat rgbDepth;
    bool rgbDepthReady = false;
    cv::Mat rgbColorPreview;
    bool rgbColorPreviewReady = false;
};

#endif