#include "SKPRPureImage.h"
#include "SKPacket.h"
#include "SKPRecipient.h"

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

#define BLUE_CHANNEL 0
#define GREEN_CHANNEL 1
#define RED_CHANNEL 2

const Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");
std::string sep = "\n----------------------------------------\n";


int radius  = 5;
cv::Scalar line_color(0,255,255);
int thickness = 5;


void SKPRPureImage::receiveFrame(SKPacket &pkt) {
    cv::Mat rgb = pkt.getRGBColor();
    Eigen::MatrixXd currMatrix = pkt.getAprilTagMatrix();

    // Draw corners
    for (int corner; corner < currMatrix.cols(); corner++) {
        cv::Point center(currMatrix.coeff(0,corner),  currMatrix.coeff(1,corner));
        cv::circle(rgb, center, radius, line_color, thickness);
    }

    // Draw lines
    if (currMatrix.cols() > 0) {
        cv::Point p1(currMatrix.coeff(0,0), currMatrix.coeff(1,0));
        cv::Point p2(currMatrix.coeff(0,1), currMatrix.coeff(1,1));
        cv::Point p3(currMatrix.coeff(0,2), currMatrix.coeff(1,2));
        cv::Point p4(currMatrix.coeff(0,3), currMatrix.coeff(1,3));

        cv::line(rgb, p1, p3, line_color, thickness, cv::LINE_AA);
        cv::line(rgb, p2, p4, line_color, thickness, cv::LINE_AA);
    }

    pkt.allocateCVMat(rgb.rows, rgb.cols, CV_8UC3, "pure");

    rgb.copyTo(pkt.getCVMat("pure"));

    _skpr->receiveFrame(pkt);
}

void SKPRPureImage::setRecipient(SKPRecipient *skpr) {
    _skpr = skpr;
}