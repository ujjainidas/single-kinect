#include "ros/ros.h"
#include "std_msgs/String.h"
#include "SKPRColorBlob.h"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

#define RED_CHANNEL 0
#define GREEN_CHANNEL 1
#define BLUE_CHANNEL 2

#define NUM_COLS 1920.0
#define MIDPOINT (NUM_COLS / 2.0)

SKPRColorBlob::SKPRColorBlob(ros::NodeHandle &n) :
    _publisher(n.advertise<std_msgs::String>("node_camera_pub", 1000)) {
    ROS_INFO_STREAM("SKPRColorBlob");
}

void SKPRColorBlob::receiveFrame(SKPacket &pkt) {
    cv::Mat rgb = pkt.getRGBColor();

    cv::Mat minColor = cv::Mat(rgb.rows, rgb.cols, CV_8UC1, cv::Scalar(0));
    cv::Mat maxColor = cv::Mat(rgb.rows, rgb.cols, CV_8UC1, cv::Scalar(255));

    vector<cv::Mat> chans;
    cv::split(rgb, chans);

    //subtract red channel from blue channel
    cv::Mat greenMinusRed;
    cv::subtract(chans[GREEN_CHANNEL], chans[RED_CHANNEL], greenMinusRed);
    cv::Mat greenMinusBlue;
    cv::subtract(chans[BLUE_CHANNEL], chans[GREEN_CHANNEL], greenMinusBlue);
    cv::Mat redMBlueMGreen;
    cv::subtract(greenMinusRed, greenMinusBlue, redMBlueMGreen);

    /*
    void bitwise_and(InputArray src1, InputArray src2, 
        OutputArray dst, InputArray mask = noArray())
    */

    //set BGR threshold
    cv::Mat threshGR;
    cv::threshold(greenMinusRed, threshGR, 80, 255, cv::THRESH_BINARY);
    cv::Mat threshGB;
    cv::threshold(greenMinusBlue, threshGB, 100, 255, cv::THRESH_BINARY);
    cv::Mat threshRBG;
    cv::threshold(redMBlueMGreen, threshRBG, 10, 255, cv::THRESH_BINARY);

    //find contours
    vector<cv::Mat> contoursGR;
    vector<cv::Vec4i> hierarchyGR;
    cv::findContours(threshRBG, contoursGR, hierarchyGR, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

    //find max contour
    int maxSizeIndex = 0;
    int maxSize = 0;
    for(int i = 0; i < contoursGR.size(); i++) {
        int size = cv::contourArea(contoursGR[i]);
        if(size > maxSize) {
            maxSizeIndex = i;
            maxSize = size;
        }
    }

    int centerX = MIDPOINT;

    if(contoursGR.size() > 0) {
        cv::Moments M = cv::moments(contoursGR[maxSizeIndex]);
        if(M.m00 != 0) {
            centerX = M.m10 / M.m00;
        }
    }

    cv::Mat grContourMask = cv::Mat::zeros(rgb.rows, rgb.cols, CV_8UC1);
    cv::drawContours(grContourMask, contoursGR, maxSizeIndex, cv::Scalar(255), cv::LineTypes::FILLED, 8, hierarchyGR);

    // //find contours
    // vector<cv::Mat> contoursGB;
    // vector<cv::Vec4i> hierarchyGB;
    // cv::findContours(threshGB, contoursGB, hierarchyGB, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

    // //find max contour
    // maxSizeIndex = 0;
    // maxSize = 0;
    // for(int i = 0; i < contoursGB.size(); i++) {
    //     int size = cv::contourArea(contoursGB[i]);
    //     if(size > maxSize) {
    //         maxSizeIndex = i;
    //         maxSize = size;
    //     }
    // }

    // cv::Mat gbContourMask = cv::Mat::zeros(rgb.rows, rgb.cols, CV_8UC1);
    // cv::drawContours(gbContourMask, contoursGB, maxSizeIndex, cv::Scalar(255), cv::LineTypes::FILLED, 8, hierarchyGB);

    // cv::Mat gbImage = cv::Mat::zeros(rgb.rows, rgb.cols, CV_8UC1);
    // rgb.copyTo(gbImage, gbContourMask);


    vector<cv::Mat> mergeChans;
    //mergeChans.push_back(greenMinusRed);
    mergeChans.push_back(grContourMask);
    mergeChans.push_back(grContourMask);
    mergeChans.push_back(grContourMask);
    //mergeChans.push_back(gbContourMask);
    //mergeChans.push_back(chans[1]);
    //mergeChans.push_back(gbContourMask);

    cv::Mat output = cv::Mat::zeros(rgb.rows, rgb.cols, CV_8UC3);
    cv::merge(mergeChans, output);

    int thickness = 2;

    // display a line along the x-axis of the line
    cv::Point line_location_start(centerX, 0), line_location_end(centerX, output.rows-1);
    cv::line(output, line_location_start, line_location_end, cv::Scalar(255, 0, 0), LINE_4);

    //display a line along the midpoint of the camera/robot
    cv::Point midpoint_start(MIDPOINT, 0), midpoint_end(MIDPOINT, output.rows-1);
    cv::line(output, midpoint_start, midpoint_end, cv::Scalar(0, 255, 0), LINE_4);

    pkt.allocateCVMat(output.rows, output.cols, CV_8UC3, "blue blobs");
    output.copyTo(pkt.getCVMat("blue blobs"));

    // calculate the error of the line
    float error = ((float)centerX - MIDPOINT)/NUM_COLS;

    ROS_INFO_STREAM(error);

    std_msgs::String msg;

    std::stringstream ss;
    ss << error;
    msg.data = ss.str();
    _publisher.publish(msg);

    _skpr->receiveFrame(pkt);
}

void SKPRColorBlob::setRecipient(SKPRecipient *skpr) {
    _skpr = skpr;
}