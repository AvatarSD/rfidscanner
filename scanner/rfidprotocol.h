#ifndef RFIDPROTOCOL_H
#define RFIDPROTOCOL_H


class RFIDProtocol
{
public:
    RFIDProtocol();

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

#endif // RFIDPROTOCOL_H
