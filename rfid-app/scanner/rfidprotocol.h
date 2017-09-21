#ifndef RFIDPROTOCOL_H
#define RFIDPROTOCOL_H

#include "types.h"

class Protocol
{
public:
    Protocol();

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

class ScannerRequest : public Serialaizeable
{
public:
    ScannerRequest(){}
    virtual ~ScannerRequest(){}

    virtual QByteArray execute(const Protocol & prot);

    // Serialaizeable interface
    virtual QString toString() const;
    virtual QJsonObject toJson() const;
    virtual QJsonParseError fromJson(const QJsonObject &);
};

class ScannerReply : public Serialaizeable
{
public:
    ScannerReply(){}
    virtual ~ScannerReply(){}

    // Serialaizeable interface
    virtual QString toString() const;
    virtual QJsonObject toJson() const;
};


#endif // RFIDPROTOCOL_H
