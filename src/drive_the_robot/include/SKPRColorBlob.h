#ifndef SKPRCOLORBLOB_H
#define SKPRCOLORBLOB_H

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "SKPRecipient.h"
#include "SKPacket.h"

class SKPRColorBlob : public SKPRecipient {
public:

    SKPRColorBlob(ros::NodeHandle &n); 
    void receiveFrame(SKPacket &pkt);
    void setRecipient(SKPRecipient *skpr);

protected:
    ros::Publisher _publisher;
    SKPRecipient *_skpr;

};

#endif