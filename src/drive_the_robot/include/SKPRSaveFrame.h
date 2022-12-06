#ifndef SKPR_SAVE_FRAME_H
#define SKPR_SAVE_FRAME_H

#include "SKPRecipient.h"
#include <string>

class SKPRSaveFrame : public SKPRecipient {
public:
    SKPRSaveFrame(std::string path);
    void receiveFrame(SKPacket &skp);
    void saveFrame();
protected:
    void writeToFile(SKPacket& skp);
    std::string _savePath;
    bool _performSave;
    int _setsCaptured;
};

#endif