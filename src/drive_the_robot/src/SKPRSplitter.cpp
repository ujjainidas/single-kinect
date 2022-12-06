#include "SKPRSplitter.h"

void SKPRSplitter::addRecipient(SKPRecipient *r) {
    _v.push_back(r);
}

void SKPRSplitter::receiveFrame(SKPacket &skp) {
    for(size_t i = 0; i < _v.size(); i++)
        _v[i]->receiveFrame(skp);
}