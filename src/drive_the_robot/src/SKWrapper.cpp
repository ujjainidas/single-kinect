#include "SKWrapper.h"
#include "SKPacket.h"
#include "SKConfig.h"
#include "SKPRecipient.h"
#include <k4arecord/playback.h>
#include <cstdio>
#include <cstdlib>

using namespace std;

SKWrapper::SKWrapper(SKConfig &skconfig, uint8_t deviceIndex) :
    _skconfig(skconfig), _device(NULL), _config(skconfig._config) {

    _device = k4a::device::open(deviceIndex);
    _sensor_calibration = _device.get_calibration(_config.depth_mode, _config.color_resolution);
    
    // Standalone
    _config.wired_sync_mode = K4A_WIRED_SYNC_MODE_STANDALONE;

    // Recipient specific config
    //JWH NEED THIS _kfr.getDevice(_device, _config);
    //JWH NEED THIS _kfr.getCalibration(sensor_calibration);
    transformation = k4a::transformation(_sensor_calibration);
    start();
}

SKWrapper::~SKWrapper() {
    _device.stop_cameras();
    _device.close();
}

void SKWrapper::setupSync(int32_t color_exposure_usec, int32_t powerline_freq){
    _device.set_color_control(K4A_COLOR_CONTROL_EXPOSURE_TIME_ABSOLUTE, K4A_COLOR_CONTROL_MODE_MANUAL, color_exposure_usec);
    // _device.set_color_control(K4A_COLOR_CONTROL_POWERLINE_FREQUENCY,K4A_COLOR_CONTROL_MODE_MANUAL,powerline_freq);
}

k4a_device_configuration_t SKWrapper::getConfig(){
    return _config;
}

k4a::calibration SKWrapper::getCalibration(){
    return _sensor_calibration;
}


std::vector<uint8_t> SKWrapper::getRawCalibration(){
    return _device.get_raw_calibration();
}

k4a::record SKWrapper::getRecordingHandle(const std::string& path){
    return k4a::record::create(path.c_str(), _device, _config);
}

void SKWrapper::start() {
    _device.start_cameras(&_config);
}

void SKWrapper::capture(k4a::capture *cap) {
    _device.get_capture(cap, std::chrono::milliseconds(K4A_WAIT_INFINITE));
}

void SKWrapper::doOnce() {
    // cout << "SKWrapper::doOnce()" << endl;
    SKPacket pkt(this);
    k4a::capture cap = pkt.getCapture();
    capture(&cap);
    pkt.setCapture(cap);
    if(_skpr) {
        _skpr->receiveFrame(pkt);
    }

    /*

    
    cv::imshow("PREVIEW", blueImage);
    cv::waitKey(1);
    */
}

void SKWrapper::setRecipient(SKPRecipient *skpr) {
    _skpr = skpr;
}

SKConfig &SKWrapper::getSKConfig() {
    return _skconfig;
}


