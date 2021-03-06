#ifndef SCANNERPROTOCOL_H
#define SCANNERPROTOCOL_H

#include <QJsonObject>
#include <QJsonParseError>
#include <QStringList>


/********************** Level 2(B) ***********************/

/*************** Interface ****************/

/***** ScannerProtocol *****/
class ScannerProtocol
{
public:
    ScannerProtocol();

    /* bool getFullMem(Tag& tag);
     const std::list<ScannerError> & getScannerErrors();

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
/*
    int ap_getaddress(int iHandle, int* oAddress, int* oVer);// ap_getaddress - 获取地址
    int ap_setaddress(int iHandle, int iAddress, int iData);// ap_setaddress - 设置地址

    int ap_getconfig(int iHandle, int iAddress, uint8_t* oData, uint8_t oSize);// ap_getconfig - 获取参数
    int ap_setconfig(int iHandle, int iAddress, uint8_t* iData, uint8_t iSize);// ap_setconfig - 设置参数

    int ap_gettcpip(int iHandle, int iAddress, uint8_t* oData, uint8_t oSize);// ap_gettcpip - 获取tcpip参数
    int ap_settcpip(int iHandle, int iAddress, uint8_t* iData, uint8_t iSize);// ap_settcpip - 设置tcpip参数

    int ap_identify6b(int iHandle, int iAddress, uint8_t* oData, uint8_t oSize);// ap_identify6b - 识别6b卡号
    int ap_read6b(int iHandle, int iAddress, uint8_t* oData, uint8_t iAddr, uint8_t iSize);// ap_read6b - 读6b数据
    int ap_write6b(int iHandle, int iAddress, uint8_t* iData, uint8_t iAddr, uint8_t iSize);// ap_write6b - 写6b数据

    int ap_identify6c(int iHandle, int iAddress, uint8_t* oData, uint8_t oSize);// ap_identify6c - 识别6C卡号
    int ap_identify6c_c(int iHandle, int iAddress, CardInfo oCard);// ap_identify6c - 识别6C卡号p
    int ap_identify6cmult(int iHandle, int iAddress, uint8_t* oData, uint8_t oSize);// ap_identify6cmult - 识别6C卡号-多卡
    int ap_read6c(int iHandle, int iAddress, uint8_t* oData, uint8_t iMem, uint8_t iAddr, uint8_t iSize);// ap_read6c - 读6C数据
    int ap_write6c(int iHandle, int iAddress, uint8_t* iData, uint8_t iMem, uint8_t iAddr, uint8_t iSize);// ap_write6c - 写6C数据

    int ap_encrypt(int iHandle, int iAddress);// ap_encrypt - 加密
    int ap_softreset(int iHandle, int iAddress);// ap_softreset - 软重启设备
    int ap_getautocard(int iHandle, uint8_t* oData, uint8_t oSize);//ap_getautocard - 获取自动上送卡号

    typedef int CommandInfo;
    typedef int CardInfo;
    int at_datatocommand(uint8_t* iData, int iSize, CommandInfo oCommand);
    int at_commandtocard(CommandInfo * oCommand, CardInfo * oCard);
    int at_datatocard(uint8_t* iData, int iSize, uint8_t* oCard, int oSize);
    int at_checksum(uint8_t* iData, int iStart, int iSize);*/

};

class ScannerCommand
{
public:
    enum CommandType{
        RUN,
        SLEEP,
        STATUS,
        INVENTORY
    };
    virtual ~ScannerCommand(){}
    virtual CommandType type() const = 0;
};

/****** ScannerRequest *****/
class ScannerRequest : public ScannerCommand
{
public:
    
    ScannerRequest(){}
    virtual ~ScannerRequest(){}
//    virtual QByteArray execute(const ScannerProtocol & prot) {}

    // Serialaizeable interface
//    virtual QString toString() const {}
//    virtual QJsonObject toJson() const {}
//    virtual QJsonParseError fromJson(const QJsonObject &) {}
};

/******* ScannerReply ******/
class ScannerReply : public ScannerCommand
{
public:
    ScannerReply(){}
    virtual ~ScannerReply(){}

    // Serialaizeable interface
    virtual QString toString() const {}
    virtual QJsonObject toJson() const {}
};


/************ Implementation **************/

/******** Status ********/
class StatusRq : public ScannerRequest
{
public:
    virtual CommandType type() const {return STATUS;}
};
class StatusRp : public ScannerReply
{
public:
    virtual CommandType type() const {return STATUS;}
};

/******** Inventory ********/
class InventoryRq : public ScannerRequest
{
public:
    virtual CommandType type() const {return INVENTORY;}
};
class InventoryRp : public ScannerReply
{
public:
    virtual CommandType type() const {return INVENTORY;}
    QStringList tags;
};

#endif // SCANNERPROTOCOL_H
