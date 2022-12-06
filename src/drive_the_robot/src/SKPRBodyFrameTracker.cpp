#include "SKPRBodyFrameTracker.h"
#include "SKPacket.h"
#include "SKPRecipient.h"

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;

#define BLUE_CHANNEL 0
#define GREEN_CHANNEL 1
#define RED_CHANNEL 2



void SKPRBodyFrameTracker::receiveFrame(SKPacket &pkt) {

    if (_skpr) {
        _skpr->receiveFrame(pkt);
    }
}

void SKPRBodyFrameTracker::setRecipient(SKPRecipient *skpr) {
    _skpr = skpr;
}