#ifndef RFIDREADER_H
#define RFIDREADER_H

#include "types.h"
#include "events.h"

#include <QObject>
#include <QSharedPointer>

/**** General Reader Settings ****/
typedef int dBm;
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

/************/
class RFIDReaderSettings
{

};

/**** RFID Reader ****/
class RFIDReader : public QObject
{
    Q_OBJECT
public:
    RFIDReader();
    virtual ~RFIDReader(){}


};


#endif // RFIDREADER_H
