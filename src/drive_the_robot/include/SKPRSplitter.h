#ifndef MKPR_SPLITTER_H
#define MKPR_SPLITTER_H

#include "SKPRecipient.h"

#include <vector>

class SKPRSplitter : public SKPRecipient {
public:
    void addRecipient(SKPRecipient *r);
    void receiveFrame(SKPacket &skp);

protected:
    std::vector<SKPRecipient *> _v;
};

#endif