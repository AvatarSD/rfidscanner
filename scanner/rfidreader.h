#ifndef RFIDREADER_H
#define RFIDREADER_H


#include "types.h"
#include "events.h"
#include <QObject>

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


/**** RFID Reader ****/
class RFIDReader
{
public:
    RFIDReader();
    virtual ~RFIDReader(){}

   /* bool getFullMem(Tag& tag);
    const std::list<ReaderError> & getReaderErrors();

    virtual bool scanVisiableTags(QList<Tag> & tags){return false;};

    virtual bool getEPCMem(Tag& tag){return false;}
    virtual bool getUserMem(Tag& tag){return false;}
    virtual bool getReserveMem(Tag& tag){return false;}

    virtual bool setEnabled(bool enable){return false;};
    virtual bool setReadPower(dBm power){return false;};
    virtual bool setWritePower(dBm power){return false;};
    virtual bool setSensitivity(dBm power){return false;};
    virtual bool setWorkingFrequencies(const WorkingFrequencies && frequencies){return false;};
    virtual bool setModulationd(Modulation modulation){return false;};
    virtual bool setTarget(Target inventory){return false;};
    virtual bool setSession(Session session){return false;};
    virtual bool setQValue(QValue q){return false;};

    virtual bool isEnabled(){return false;};
    virtual bool setReadPower(dBm& power){return false;};
    virtual bool setWritePower(dBm& power){return false;};
    virtual bool setSensitivity(dBm& power){return false;};
    virtual bool getWorkingFrequencies(WorkingFrequencies & frequencies){return false;};
    virtual bool setModulationd(Modulation& modulation){return false;};
    virtual bool setTarget(Target& inventory){return false;};
    virtual bool setSession(Session& session){return false;};
    virtual bool setQValue(QValue& q){return false;};*/
};

class RFIDReaderSettings
{

};

#endif // RFIDREADER_H
