#ifndef SKPRPUREIMAGE_H
#define SKPRPUREIMAGE_H

#include "SKPRecipient.h"

class SKPRPureImage : public SKPRecipient {
public:
    void receiveFrame(SKPacket &pkt);
    void setRecipient(SKPRecipient *skpr);

protected:
    SKPRecipient *_skpr;
};

#endif