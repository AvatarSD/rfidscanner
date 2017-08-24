#ifndef RFIDREADER_H
#define RFIDREADER_H

#include "readertransport.h"
#include "types.h"


/**** General Reader Settings ****/
typedef int dBmRead;
typedef int dBmWrite;
typedef int dBmSensivity;
typedef int Frequency;
typedef std::list<Frequency> WorkingFrequencies;
enum Modulation{
    FM0,
    Miller1,
    Miller2,
    Miller4,
    Miller8
};
enum Target{
    Single, Dual
};
enum Session{
    S0 = 0, S1 = 1, S2 = 2, S3 = 3
};
typedef uint QValue;


/**** RFID Reader ****/
class RFIDReader
{
public:
    RFIDReader();
    virtual ~RFIDReader(){}

    bool getFullMem(Tag & tag);
    virtual bool getEPCMem(Tag &){return false;}
    virtual bool getUserMem(Tag&){return false;}
    virtual bool getReserveMem(Tag&){return false;}

    const QList<Tag> & lastVisiableTag(){return visiableTags;}
    virtual const QList<Tag> & scanVisiableTag(){return visiableTags;};

private:
    QList<Tag> visiableTags;
};

class RFIDReaderSettings
{

};

#endif // RFIDREADER_H
