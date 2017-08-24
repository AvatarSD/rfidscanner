#ifndef RFIDREADER_H
#define RFIDREADER_H

#include "readertransport.h"
#include "types.h"

class RFIDReader
{
public:
    RFIDReader();

    bool getFullMem(Tag & tag);
    virtual bool getEPCMem(Tag &){return false;}
    virtual bool getUserMem(Tag&){return false;}
    virtual bool getReserveMem(Tag&){return false;}

    virtual const QList<Tag> & getVisiableTag(){return visiableTags;}

private:
    QList<Tag> visiableTags;
};

#endif // RFIDREADER_H
