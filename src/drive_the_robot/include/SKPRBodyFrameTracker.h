#ifndef SKPRBODYFRAMETRACKER_H
#define SKPRBODYFRAMETRACKER_H

#include "SKPRecipient.h"

class SKPRBodyFrameTracker : public SKPRecipient {
public:
    void receiveFrame(SKPacket &pkt);
    void setRecipient(SKPRecipient *skpr);

protected:
    SKPRecipient *_skpr;
};

#endif