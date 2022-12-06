#ifndef SKP_RECIPIENT_H
#define SKP_RECIPIENT_H

class SKPacket;

class SKPRecipient {
public:
    virtual void receiveFrame(SKPacket &skp) = 0;
};

#endif