//
// Generated file, do not edit! Created by opp_msgc 4.2 from messages/ProbabilisticBroadcastPkt.msg.
//

#ifndef _PROBABILISTICBROADCASTPKT_M_H_
#define _PROBABILISTICBROADCASTPKT_M_H_

#include <omnetpp.h>

// opp_msgc version check
#define MSGC_VERSION 0x0402
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of opp_msgc: 'make clean' should help.
#endif

// cplusplus {{
#include "NetwPkt_m.h"
// }}



/**
 * Class generated from <tt>messages/ProbabilisticBroadcastPkt.msg</tt> by opp_msgc.
 * <pre>
 * message ProbabilisticBroadcastPkt extends NetwPkt
 * {
 *     long             id;    
 *     simtime_t        appTtl;   
 *     LAddress::L3Type initialSrcAddr;
 *     LAddress::L3Type finalDestAddr;
 *     int              nbHops;
 * }
 * </pre>
 */
class ProbabilisticBroadcastPkt : public ::NetwPkt
{
  protected:
    long id_var;
    simtime_t appTtl_var;
    LAddress::L3Type initialSrcAddr_var;
    LAddress::L3Type finalDestAddr_var;
    int nbHops_var;

  private:
    void copy(const ProbabilisticBroadcastPkt& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ProbabilisticBroadcastPkt&);

  public:
    ProbabilisticBroadcastPkt(const char *name=NULL, int kind=0);
    ProbabilisticBroadcastPkt(const ProbabilisticBroadcastPkt& other);
    virtual ~ProbabilisticBroadcastPkt();
    ProbabilisticBroadcastPkt& operator=(const ProbabilisticBroadcastPkt& other);
    virtual ProbabilisticBroadcastPkt *dup() const {return new ProbabilisticBroadcastPkt(*this);}
    virtual void parsimPack(cCommBuffer *b);
    virtual void parsimUnpack(cCommBuffer *b);

    // field getter/setter methods
    virtual long getId() const;
    virtual void setId(long id);
    virtual simtime_t getAppTtl() const;
    virtual void setAppTtl(simtime_t appTtl);
    virtual LAddress::L3Type& getInitialSrcAddr();
    virtual const LAddress::L3Type& getInitialSrcAddr() const {return const_cast<ProbabilisticBroadcastPkt*>(this)->getInitialSrcAddr();}
    virtual void setInitialSrcAddr(const LAddress::L3Type& initialSrcAddr);
    virtual LAddress::L3Type& getFinalDestAddr();
    virtual const LAddress::L3Type& getFinalDestAddr() const {return const_cast<ProbabilisticBroadcastPkt*>(this)->getFinalDestAddr();}
    virtual void setFinalDestAddr(const LAddress::L3Type& finalDestAddr);
    virtual int getNbHops() const;
    virtual void setNbHops(int nbHops);
};

inline void doPacking(cCommBuffer *b, ProbabilisticBroadcastPkt& obj) {obj.parsimPack(b);}
inline void doUnpacking(cCommBuffer *b, ProbabilisticBroadcastPkt& obj) {obj.parsimUnpack(b);}


#endif // _PROBABILISTICBROADCASTPKT_M_H_
