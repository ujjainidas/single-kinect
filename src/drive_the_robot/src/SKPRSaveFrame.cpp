#include "SKPRSaveFrame.h"
#include "SKPacket.h"
#include "SKConfig.h"
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

SKPRSaveFrame::SKPRSaveFrame(string path){
    _savePath = path;
    _performSave = false;
    _setsCaptured = 0;
    // Create main directory doesn't already exist
    mkdir(path.c_str(), 0777);

    // Set the correct start number
    while (true) {
        std::string frame_dir = path + "/imageset_" + to_string(_setsCaptured);
        if (access(frame_dir.c_str(), F_OK) == -1) {
            break;
        }
        _setsCaptured++;
    }
}

void SKPRSaveFrame::receiveFrame(SKPacket& mkp){
    if(_performSave){
        writeToFile(skp);
        _performSave = false;
    }
}

void SKPRSaveFrame::saveFrame(){
    _performSave = true;
}

/**
 * @brief Write BGRA and RGB depth images from all cameras to file
 * 
 * @param mkp Packet to write from
 */
void SKPRSaveFrame::writeToFile(SKPacket& skp){
    // Write calibration blob with imageset
    if (_setsCaptured == 0) {
        vector<uint8_t> v = skp.getSKWrapper()[0]->getRawCalibration();
        ofstream blob_file(_savePath + "/calib_blob");
        for (char c : v) {
            blob_file << c;
        }
        blob_file.close();
    }

    // Create directory for images in this packet
    string imageSetName = "imageset_" + to_string(_setsCaptured++);
    string imageSetPath = _savePath + "/" + imageSetName;
    mkdir(imageSetPath.c_str(), 0777);

    for(size_t i = 0; i < skp.getConfig()._nCameras; i++) {
        // Save both BGRA and rgb depth images from mkp
        // TODO: set up config to choose which images to save

        string imageName = "cam" + to_string(i) + "_";

        string imagePath = imageSetPath + "/" + imageName;

        cv::imwrite(imagePath + "bgra.jpg", skp[i].getRGBColor());
        cv::imwrite(imagePath + "rgb_depth.jpg", skp[i].getRGBDepth());
    }
}