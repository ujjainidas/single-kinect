#ifndef SKP_PERSON_TRACKER_H
#define SKP_PERSON_TRACKER_H

#include "SKPRecipient.h"

#include <k4abt.hpp>

class SKPRecipient;
class SKWrapper;

class SKPRPersonTracker : public SKPRecipient {
public:
    SKPRPersonTracker(SKWrapper &skw);
    ~SKPRPersonTracker();

    void addRecipient(SKPRecipient *r);

    void receiveFrame(SKPacket &skp);
    
protected:
    SKWrapper &_skw;
    std::vector<SKPRecipient *> _r;
    
    k4abt_tracker_configuration_t _tracker_config;
    k4abt::tracker _tracker;
};

#endif