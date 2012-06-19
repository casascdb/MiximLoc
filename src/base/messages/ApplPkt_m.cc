//
// Generated file, do not edit! Created by opp_msgc 4.2 from messages/ApplPkt.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "ApplPkt_m.h"

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




Register_Class(ApplPkt);

ApplPkt::ApplPkt(const char *name, int kind) : cPacket(name,kind)
{
    this->destAddr_var = LAddress::L3BROADCAST;
    this->srcAddr_var = LAddress::L3BROADCAST;
    this->sequenceId_var = 0;
    this->priority_var = 1;
    this->timeOfLife_var = 1;
    this->status_var = 0;
    this->posX_var = 0;
    this->posY_var = 0;
    this->posZ_var = 0;
    this->timestamp_var = 0;
    this->realDestAddr_var = -1;
    this->realSrcAddr_var = -1;
    this->retransmisionCounterBO_var = 0;
    this->retransmisionCounterACK_var = 0;
    this->CSMA_var = true;
    this->askForRequest_var = false;
    this->requestPacket_var = false;
    this->numberOfBroadcasts_var = 0;
    this->sendInProcess_var = false;
    this->wasRequest_var = false;
    this->wasReport_var = false;
    this->wasBroadcast_var = false;
    this->isAnswer_var = false;
    this->id_var = 0;
    this->createdIn_var = 0;
    this->nodeMode_var = 0;
    this->fromNode_var = 0;
    this->timestampAnchorTX_var = 0;
    this->timestampComRelated_var = 0;
    this->broadcastedSuccess_var = 1;
}

ApplPkt::ApplPkt(const ApplPkt& other) : cPacket(other)
{
    copy(other);
}

ApplPkt::~ApplPkt()
{
}

ApplPkt& ApplPkt::operator=(const ApplPkt& other)
{
    if (this==&other) return *this;
    cPacket::operator=(other);
    copy(other);
    return *this;
}

void ApplPkt::copy(const ApplPkt& other)
{
    this->destAddr_var = other.destAddr_var;
    this->srcAddr_var = other.srcAddr_var;
    this->sequenceId_var = other.sequenceId_var;
    this->priority_var = other.priority_var;
    this->timeOfLife_var = other.timeOfLife_var;
    this->status_var = other.status_var;
    this->posX_var = other.posX_var;
    this->posY_var = other.posY_var;
    this->posZ_var = other.posZ_var;
    this->timestamp_var = other.timestamp_var;
    this->realDestAddr_var = other.realDestAddr_var;
    this->realSrcAddr_var = other.realSrcAddr_var;
    this->retransmisionCounterBO_var = other.retransmisionCounterBO_var;
    this->retransmisionCounterACK_var = other.retransmisionCounterACK_var;
    this->CSMA_var = other.CSMA_var;
    this->askForRequest_var = other.askForRequest_var;
    this->requestPacket_var = other.requestPacket_var;
    this->numberOfBroadcasts_var = other.numberOfBroadcasts_var;
    this->sendInProcess_var = other.sendInProcess_var;
    this->wasRequest_var = other.wasRequest_var;
    this->wasReport_var = other.wasReport_var;
    this->wasBroadcast_var = other.wasBroadcast_var;
    this->isAnswer_var = other.isAnswer_var;
    this->id_var = other.id_var;
    this->createdIn_var = other.createdIn_var;
    this->nodeMode_var = other.nodeMode_var;
    this->fromNode_var = other.fromNode_var;
    this->timestampAnchorTX_var = other.timestampAnchorTX_var;
    this->timestampComRelated_var = other.timestampComRelated_var;
    this->broadcastedSuccess_var = other.broadcastedSuccess_var;
}

void ApplPkt::parsimPack(cCommBuffer *b)
{
    cPacket::parsimPack(b);
    doPacking(b,this->destAddr_var);
    doPacking(b,this->srcAddr_var);
    doPacking(b,this->sequenceId_var);
    doPacking(b,this->priority_var);
    doPacking(b,this->timeOfLife_var);
    doPacking(b,this->status_var);
    doPacking(b,this->posX_var);
    doPacking(b,this->posY_var);
    doPacking(b,this->posZ_var);
    doPacking(b,this->timestamp_var);
    doPacking(b,this->realDestAddr_var);
    doPacking(b,this->realSrcAddr_var);
    doPacking(b,this->retransmisionCounterBO_var);
    doPacking(b,this->retransmisionCounterACK_var);
    doPacking(b,this->CSMA_var);
    doPacking(b,this->askForRequest_var);
    doPacking(b,this->requestPacket_var);
    doPacking(b,this->numberOfBroadcasts_var);
    doPacking(b,this->sendInProcess_var);
    doPacking(b,this->wasRequest_var);
    doPacking(b,this->wasReport_var);
    doPacking(b,this->wasBroadcast_var);
    doPacking(b,this->isAnswer_var);
    doPacking(b,this->id_var);
    doPacking(b,this->createdIn_var);
    doPacking(b,this->nodeMode_var);
    doPacking(b,this->fromNode_var);
    doPacking(b,this->timestampAnchorTX_var);
    doPacking(b,this->timestampComRelated_var);
    doPacking(b,this->broadcastedSuccess_var);
}

void ApplPkt::parsimUnpack(cCommBuffer *b)
{
    cPacket::parsimUnpack(b);
    doUnpacking(b,this->destAddr_var);
    doUnpacking(b,this->srcAddr_var);
    doUnpacking(b,this->sequenceId_var);
    doUnpacking(b,this->priority_var);
    doUnpacking(b,this->timeOfLife_var);
    doUnpacking(b,this->status_var);
    doUnpacking(b,this->posX_var);
    doUnpacking(b,this->posY_var);
    doUnpacking(b,this->posZ_var);
    doUnpacking(b,this->timestamp_var);
    doUnpacking(b,this->realDestAddr_var);
    doUnpacking(b,this->realSrcAddr_var);
    doUnpacking(b,this->retransmisionCounterBO_var);
    doUnpacking(b,this->retransmisionCounterACK_var);
    doUnpacking(b,this->CSMA_var);
    doUnpacking(b,this->askForRequest_var);
    doUnpacking(b,this->requestPacket_var);
    doUnpacking(b,this->numberOfBroadcasts_var);
    doUnpacking(b,this->sendInProcess_var);
    doUnpacking(b,this->wasRequest_var);
    doUnpacking(b,this->wasReport_var);
    doUnpacking(b,this->wasBroadcast_var);
    doUnpacking(b,this->isAnswer_var);
    doUnpacking(b,this->id_var);
    doUnpacking(b,this->createdIn_var);
    doUnpacking(b,this->nodeMode_var);
    doUnpacking(b,this->fromNode_var);
    doUnpacking(b,this->timestampAnchorTX_var);
    doUnpacking(b,this->timestampComRelated_var);
    doUnpacking(b,this->broadcastedSuccess_var);
}

LAddress::L3Type& ApplPkt::getDestAddr()
{
    return destAddr_var;
}

void ApplPkt::setDestAddr(const LAddress::L3Type& destAddr)
{
    this->destAddr_var = destAddr;
}

LAddress::L3Type& ApplPkt::getSrcAddr()
{
    return srcAddr_var;
}

void ApplPkt::setSrcAddr(const LAddress::L3Type& srcAddr)
{
    this->srcAddr_var = srcAddr;
}

int ApplPkt::getSequenceId() const
{
    return sequenceId_var;
}

void ApplPkt::setSequenceId(int sequenceId)
{
    this->sequenceId_var = sequenceId;
}

int ApplPkt::getPriority() const
{
    return priority_var;
}

void ApplPkt::setPriority(int priority)
{
    this->priority_var = priority;
}

int ApplPkt::getTimeOfLife() const
{
    return timeOfLife_var;
}

void ApplPkt::setTimeOfLife(int timeOfLife)
{
    this->timeOfLife_var = timeOfLife;
}

int8 ApplPkt::getStatus() const
{
    return status_var;
}

void ApplPkt::setStatus(int8 status)
{
    this->status_var = status;
}

int16 ApplPkt::getPosX() const
{
    return posX_var;
}

void ApplPkt::setPosX(int16 posX)
{
    this->posX_var = posX;
}

int16 ApplPkt::getPosY() const
{
    return posY_var;
}

void ApplPkt::setPosY(int16 posY)
{
    this->posY_var = posY;
}

int16 ApplPkt::getPosZ() const
{
    return posZ_var;
}

void ApplPkt::setPosZ(int16 posZ)
{
    this->posZ_var = posZ;
}

int32 ApplPkt::getTimestamp() const
{
    return timestamp_var;
}

void ApplPkt::setTimestamp(int32 timestamp)
{
    this->timestamp_var = timestamp;
}

int ApplPkt::getRealDestAddr() const
{
    return realDestAddr_var;
}

void ApplPkt::setRealDestAddr(int realDestAddr)
{
    this->realDestAddr_var = realDestAddr;
}

int ApplPkt::getRealSrcAddr() const
{
    return realSrcAddr_var;
}

void ApplPkt::setRealSrcAddr(int realSrcAddr)
{
    this->realSrcAddr_var = realSrcAddr;
}

int ApplPkt::getRetransmisionCounterBO() const
{
    return retransmisionCounterBO_var;
}

void ApplPkt::setRetransmisionCounterBO(int retransmisionCounterBO)
{
    this->retransmisionCounterBO_var = retransmisionCounterBO;
}

int ApplPkt::getRetransmisionCounterACK() const
{
    return retransmisionCounterACK_var;
}

void ApplPkt::setRetransmisionCounterACK(int retransmisionCounterACK)
{
    this->retransmisionCounterACK_var = retransmisionCounterACK;
}

bool ApplPkt::getCSMA() const
{
    return CSMA_var;
}

void ApplPkt::setCSMA(bool CSMA)
{
    this->CSMA_var = CSMA;
}

bool ApplPkt::getAskForRequest() const
{
    return askForRequest_var;
}

void ApplPkt::setAskForRequest(bool askForRequest)
{
    this->askForRequest_var = askForRequest;
}

bool ApplPkt::getRequestPacket() const
{
    return requestPacket_var;
}

void ApplPkt::setRequestPacket(bool requestPacket)
{
    this->requestPacket_var = requestPacket;
}

int ApplPkt::getNumberOfBroadcasts() const
{
    return numberOfBroadcasts_var;
}

void ApplPkt::setNumberOfBroadcasts(int numberOfBroadcasts)
{
    this->numberOfBroadcasts_var = numberOfBroadcasts;
}

bool ApplPkt::getSendInProcess() const
{
    return sendInProcess_var;
}

void ApplPkt::setSendInProcess(bool sendInProcess)
{
    this->sendInProcess_var = sendInProcess;
}

bool ApplPkt::getWasRequest() const
{
    return wasRequest_var;
}

void ApplPkt::setWasRequest(bool wasRequest)
{
    this->wasRequest_var = wasRequest;
}

bool ApplPkt::getWasReport() const
{
    return wasReport_var;
}

void ApplPkt::setWasReport(bool wasReport)
{
    this->wasReport_var = wasReport;
}

bool ApplPkt::getWasBroadcast() const
{
    return wasBroadcast_var;
}

void ApplPkt::setWasBroadcast(bool wasBroadcast)
{
    this->wasBroadcast_var = wasBroadcast;
}

bool ApplPkt::getIsAnswer() const
{
    return isAnswer_var;
}

void ApplPkt::setIsAnswer(bool isAnswer)
{
    this->isAnswer_var = isAnswer;
}

int ApplPkt::getId() const
{
    return id_var;
}

void ApplPkt::setId(int id)
{
    this->id_var = id;
}

int ApplPkt::getCreatedIn() const
{
    return createdIn_var;
}

void ApplPkt::setCreatedIn(int createdIn)
{
    this->createdIn_var = createdIn;
}

int ApplPkt::getNodeMode() const
{
    return nodeMode_var;
}

void ApplPkt::setNodeMode(int nodeMode)
{
    this->nodeMode_var = nodeMode;
}

int ApplPkt::getFromNode() const
{
    return fromNode_var;
}

void ApplPkt::setFromNode(int fromNode)
{
    this->fromNode_var = fromNode;
}

double ApplPkt::getTimestampAnchorTX() const
{
    return timestampAnchorTX_var;
}

void ApplPkt::setTimestampAnchorTX(double timestampAnchorTX)
{
    this->timestampAnchorTX_var = timestampAnchorTX;
}

double ApplPkt::getTimestampComRelated() const
{
    return timestampComRelated_var;
}

void ApplPkt::setTimestampComRelated(double timestampComRelated)
{
    this->timestampComRelated_var = timestampComRelated;
}

double ApplPkt::getBroadcastedSuccess() const
{
    return broadcastedSuccess_var;
}

void ApplPkt::setBroadcastedSuccess(double broadcastedSuccess)
{
    this->broadcastedSuccess_var = broadcastedSuccess;
}

class ApplPktDescriptor : public cClassDescriptor
{
  public:
    ApplPktDescriptor();
    virtual ~ApplPktDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(ApplPktDescriptor);

ApplPktDescriptor::ApplPktDescriptor() : cClassDescriptor("ApplPkt", "cPacket")
{
}

ApplPktDescriptor::~ApplPktDescriptor()
{
}

bool ApplPktDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<ApplPkt *>(obj)!=NULL;
}

const char *ApplPktDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int ApplPktDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 30+basedesc->getFieldCount(object) : 30;
}

unsigned int ApplPktDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISCOMPOUND,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<30) ? fieldTypeFlags[field] : 0;
}

const char *ApplPktDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "destAddr",
        "srcAddr",
        "sequenceId",
        "priority",
        "timeOfLife",
        "status",
        "posX",
        "posY",
        "posZ",
        "timestamp",
        "realDestAddr",
        "realSrcAddr",
        "retransmisionCounterBO",
        "retransmisionCounterACK",
        "CSMA",
        "askForRequest",
        "requestPacket",
        "numberOfBroadcasts",
        "sendInProcess",
        "wasRequest",
        "wasReport",
        "wasBroadcast",
        "isAnswer",
        "id",
        "createdIn",
        "nodeMode",
        "fromNode",
        "timestampAnchorTX",
        "timestampComRelated",
        "broadcastedSuccess",
    };
    return (field>=0 && field<30) ? fieldNames[field] : NULL;
}

int ApplPktDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='d' && strcmp(fieldName, "destAddr")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "srcAddr")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "sequenceId")==0) return base+2;
    if (fieldName[0]=='p' && strcmp(fieldName, "priority")==0) return base+3;
    if (fieldName[0]=='t' && strcmp(fieldName, "timeOfLife")==0) return base+4;
    if (fieldName[0]=='s' && strcmp(fieldName, "status")==0) return base+5;
    if (fieldName[0]=='p' && strcmp(fieldName, "posX")==0) return base+6;
    if (fieldName[0]=='p' && strcmp(fieldName, "posY")==0) return base+7;
    if (fieldName[0]=='p' && strcmp(fieldName, "posZ")==0) return base+8;
    if (fieldName[0]=='t' && strcmp(fieldName, "timestamp")==0) return base+9;
    if (fieldName[0]=='r' && strcmp(fieldName, "realDestAddr")==0) return base+10;
    if (fieldName[0]=='r' && strcmp(fieldName, "realSrcAddr")==0) return base+11;
    if (fieldName[0]=='r' && strcmp(fieldName, "retransmisionCounterBO")==0) return base+12;
    if (fieldName[0]=='r' && strcmp(fieldName, "retransmisionCounterACK")==0) return base+13;
    if (fieldName[0]=='C' && strcmp(fieldName, "CSMA")==0) return base+14;
    if (fieldName[0]=='a' && strcmp(fieldName, "askForRequest")==0) return base+15;
    if (fieldName[0]=='r' && strcmp(fieldName, "requestPacket")==0) return base+16;
    if (fieldName[0]=='n' && strcmp(fieldName, "numberOfBroadcasts")==0) return base+17;
    if (fieldName[0]=='s' && strcmp(fieldName, "sendInProcess")==0) return base+18;
    if (fieldName[0]=='w' && strcmp(fieldName, "wasRequest")==0) return base+19;
    if (fieldName[0]=='w' && strcmp(fieldName, "wasReport")==0) return base+20;
    if (fieldName[0]=='w' && strcmp(fieldName, "wasBroadcast")==0) return base+21;
    if (fieldName[0]=='i' && strcmp(fieldName, "isAnswer")==0) return base+22;
    if (fieldName[0]=='i' && strcmp(fieldName, "id")==0) return base+23;
    if (fieldName[0]=='c' && strcmp(fieldName, "createdIn")==0) return base+24;
    if (fieldName[0]=='n' && strcmp(fieldName, "nodeMode")==0) return base+25;
    if (fieldName[0]=='f' && strcmp(fieldName, "fromNode")==0) return base+26;
    if (fieldName[0]=='t' && strcmp(fieldName, "timestampAnchorTX")==0) return base+27;
    if (fieldName[0]=='t' && strcmp(fieldName, "timestampComRelated")==0) return base+28;
    if (fieldName[0]=='b' && strcmp(fieldName, "broadcastedSuccess")==0) return base+29;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *ApplPktDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "LAddress::L3Type",
        "LAddress::L3Type",
        "int",
        "int",
        "int",
        "int8",
        "int16",
        "int16",
        "int16",
        "int32",
        "int",
        "int",
        "int",
        "int",
        "bool",
        "bool",
        "bool",
        "int",
        "bool",
        "bool",
        "bool",
        "bool",
        "bool",
        "int",
        "int",
        "int",
        "int",
        "double",
        "double",
        "double",
    };
    return (field>=0 && field<30) ? fieldTypeStrings[field] : NULL;
}

const char *ApplPktDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int ApplPktDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    ApplPkt *pp = (ApplPkt *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string ApplPktDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    ApplPkt *pp = (ApplPkt *)object; (void)pp;
    switch (field) {
        case 0: {std::stringstream out; out << pp->getDestAddr(); return out.str();}
        case 1: {std::stringstream out; out << pp->getSrcAddr(); return out.str();}
        case 2: return long2string(pp->getSequenceId());
        case 3: return long2string(pp->getPriority());
        case 4: return long2string(pp->getTimeOfLife());
        case 5: return long2string(pp->getStatus());
        case 6: return long2string(pp->getPosX());
        case 7: return long2string(pp->getPosY());
        case 8: return long2string(pp->getPosZ());
        case 9: return long2string(pp->getTimestamp());
        case 10: return long2string(pp->getRealDestAddr());
        case 11: return long2string(pp->getRealSrcAddr());
        case 12: return long2string(pp->getRetransmisionCounterBO());
        case 13: return long2string(pp->getRetransmisionCounterACK());
        case 14: return bool2string(pp->getCSMA());
        case 15: return bool2string(pp->getAskForRequest());
        case 16: return bool2string(pp->getRequestPacket());
        case 17: return long2string(pp->getNumberOfBroadcasts());
        case 18: return bool2string(pp->getSendInProcess());
        case 19: return bool2string(pp->getWasRequest());
        case 20: return bool2string(pp->getWasReport());
        case 21: return bool2string(pp->getWasBroadcast());
        case 22: return bool2string(pp->getIsAnswer());
        case 23: return long2string(pp->getId());
        case 24: return long2string(pp->getCreatedIn());
        case 25: return long2string(pp->getNodeMode());
        case 26: return long2string(pp->getFromNode());
        case 27: return double2string(pp->getTimestampAnchorTX());
        case 28: return double2string(pp->getTimestampComRelated());
        case 29: return double2string(pp->getBroadcastedSuccess());
        default: return "";
    }
}

bool ApplPktDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    ApplPkt *pp = (ApplPkt *)object; (void)pp;
    switch (field) {
        case 2: pp->setSequenceId(string2long(value)); return true;
        case 3: pp->setPriority(string2long(value)); return true;
        case 4: pp->setTimeOfLife(string2long(value)); return true;
        case 5: pp->setStatus(string2long(value)); return true;
        case 6: pp->setPosX(string2long(value)); return true;
        case 7: pp->setPosY(string2long(value)); return true;
        case 8: pp->setPosZ(string2long(value)); return true;
        case 9: pp->setTimestamp(string2long(value)); return true;
        case 10: pp->setRealDestAddr(string2long(value)); return true;
        case 11: pp->setRealSrcAddr(string2long(value)); return true;
        case 12: pp->setRetransmisionCounterBO(string2long(value)); return true;
        case 13: pp->setRetransmisionCounterACK(string2long(value)); return true;
        case 14: pp->setCSMA(string2bool(value)); return true;
        case 15: pp->setAskForRequest(string2bool(value)); return true;
        case 16: pp->setRequestPacket(string2bool(value)); return true;
        case 17: pp->setNumberOfBroadcasts(string2long(value)); return true;
        case 18: pp->setSendInProcess(string2bool(value)); return true;
        case 19: pp->setWasRequest(string2bool(value)); return true;
        case 20: pp->setWasReport(string2bool(value)); return true;
        case 21: pp->setWasBroadcast(string2bool(value)); return true;
        case 22: pp->setIsAnswer(string2bool(value)); return true;
        case 23: pp->setId(string2long(value)); return true;
        case 24: pp->setCreatedIn(string2long(value)); return true;
        case 25: pp->setNodeMode(string2long(value)); return true;
        case 26: pp->setFromNode(string2long(value)); return true;
        case 27: pp->setTimestampAnchorTX(string2double(value)); return true;
        case 28: pp->setTimestampComRelated(string2double(value)); return true;
        case 29: pp->setBroadcastedSuccess(string2double(value)); return true;
        default: return false;
    }
}

const char *ApplPktDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldStructNames[] = {
        "LAddress::L3Type",
        "LAddress::L3Type",
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
    };
    return (field>=0 && field<30) ? fieldStructNames[field] : NULL;
}

void *ApplPktDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    ApplPkt *pp = (ApplPkt *)object; (void)pp;
    switch (field) {
        case 0: return (void *)(&pp->getDestAddr()); break;
        case 1: return (void *)(&pp->getSrcAddr()); break;
        default: return NULL;
    }
}


