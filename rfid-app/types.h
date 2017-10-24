#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QDateTime>


/************* Serialaizeable ************/

//class Serialaizeable
//{
//public:
//    virtual ~Serialaizeable(){}
//    /*** interface: ***/
//    virtual QString toString() const = 0;
//    virtual QJsonObject toJson() const = 0;
//    virtual QJsonParseError fromJson(const QJsonObject&) = 0;
//};


/************** Tag Memory **************/

//#define TID_LENGHT 12

//class TagMemory : public Serialaizeable
//{
//public:
//    enum TagMemoryBank{
//        NONE = 0,
//        RESERVE = 0b0001,
//        EPC = 0b0010,
//        TID = 0b0100,
//        USER = 0b1000
//    };
//    virtual ~TagMemory(){}
//    virtual TagMemoryBank memoryBankType() const ;
//};

//class TagID : public TagMemory
//{
//public:
//    virtual ~TagID(){}
//    virtual QString toString() const;
//    virtual TagMemoryBank memoryBankType() const;
//    int fromString(const QString& tag);
//    bool operator ==(const TagID & val);
//private:
//    uint8_t id[TID_LENGHT];
//};


/*
class EPCMem : public TagMemory
{
public:
    EPCMem();
    EPCMem(const EPCMem & val);
    const EPCMem &operator =(const EPCMem & val);
    virtual ~EPCMem(){}
    virtual QSharedPointer<QString> toString() const;
    virtual QSharedPointer<QJsonObject> toJson() const;
private:
    // todo
};

class User : public TagMemory
{
public:
    User();
    User(const User & val);
    const User &operator =(const User & val);
    virtual ~User(){}
    virtual QSharedPointer<QString> toString() const;
    virtual QSharedPointer<QJsonObject> toJson() const;
private:
    // todo
};

class Reserve : public TagMemory
{
public:
    Reserve();
    Reserve(const Reserve & val);
    const Reserve &operator =(const Reserve & val);
    virtual ~Reserve(){}
    virtual QSharedPointer<QString> toString() const;
    virtual QSharedPointer<QJsonObject> toJson() const;
private:
    // todo
};*/


/***************** Tag ******************/
/*
class Tag : public Serialaizeable
{
public:
    Tag(const TagID&, const EPCMem&, const User&, const Reserve&);
    const TagID & getTagID() const;
    const EPCMem & getEpc() const;
    const User & getUser() const;
    const Reserve & getReserve() const;
    void setEpc(const EPCMem &value);
    void setUser(const User &value);
    void setReserve(const Reserve &value);

    // Serialaizeable interface
    QString toString() const;
    void toJson(QJsonObject &) const;
    void fromJson(const QJsonObject &, QJsonParseError *);

private:
    const TagID tid;
    EPCMem epc;
    User user;
    Reserve reserve;

};*/






#endif // TYPES_H
