#include "SKWrapper.h"
#include "SKPacket.h"
#include "SKConfig.h"
#include "SKPRecipient.h"
#include <k4arecord/playback.h>
#include <cstdio>
#include <cstdlib>

using namespace std;


/**
 * @brief Write BGRA and RGB depth images from all cameras to file
 * 
 * @param skp Packet to write from
 */
SKReceive::writeToFile(SKPacket& skp){
    // Write calibration blob with imageset
    if (_setsCaptured == 0) {
        vector<uint8_t> v = mkp.getSKWrappers()[0]->getRawCalibration();
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