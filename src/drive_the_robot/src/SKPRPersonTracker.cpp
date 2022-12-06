#include "SKPRPersonTracker.h"
#include "SKWrapper.h"
#include "SKConfig.h"
#include "SKPacket.h"
#include "SKPRecipient.h"

#include <cstdlib>

using namespace k4abt;

SKPRPersonTracker::SKPRPersonTracker(SKWrapper &skw) : _skw(skw), _tracker_config(K4ABT_TRACKER_CONFIG_DEFAULT) {

    k4a_calibration_t calibration = _skw.getCalibration();
    _tracker = k4abt::tracker::create(calibration, _tracker_config);
}

SKPRPersonTracker::~SKPRPersonTracker() {
    //delete _tracker;
}

void SKPRPersonTracker::addRecipient(SKPRecipient *r) {
    _r.push_back(r);
}

void SKPRPersonTracker::receiveFrame(SKPacket &skp) {
    //PROFILE_START;

    std::chrono::milliseconds wait = std::chrono::milliseconds(K4A_WAIT_INFINITE);

    _tracker.enqueue_capture(skp.getCapture(), wait);

    k4abt::frame bf;
    _tracker.pop_result(&bf, wait);
    skp.setBodyFrame(bf);

    //PROFILE_END("MKPPersonTracker.receiveFrame");

    for(size_t i = 0; i < _r.size(); i++) {
        _r[i]->receiveFrame(skp);
    }
}