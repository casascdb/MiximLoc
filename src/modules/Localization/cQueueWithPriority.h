/*
 * cQueueWithPriority.h
 *
 *  Created on: 28/06/2011
 *      Author: Rafel Estelrich Moreno
 */

#ifndef CQUEUEWITHPRIORITY_H_
#define CQUEUEWITHPRIORITY_H_


#include "BaseLayer.h"
#include "ApplPkt_m.h"
#include "SimpleAddress.h"
#include "BaseArp.h"
#include <BaseWorldUtility.h>
#include "BaseConnectionManager.h"
#include "NetwControlInfo.h"
#include "AppToNetControlInfo.h"
#include <cassert>
#include <Packet.h>
#include <BaseMacLayer.h>
#include <omnetpp.h>

class SIM_API cQueueWithPriority : public cQueue
{

protected:

	int maxElem;	// Defines the maximum number of elements in the priority queue

public:

	int* reportQueueDrop;	// Buffer to pass the number of report dropped in the queue
	int* broadQueueDrop;	// Buffer to pass the number of broadcast dropped in the queue
	int requestQueueDrop;	// Buffer to pass the number of ask dropped in the queue

public:

	//Sort the elements of a given queue by priority
	void sortQueue();

	//Check if packet a has priority over b
	bool firstHasPriority(cObject *a, cObject *b);

	//Try to introduce an object into the queue; returns false if queue is full and element has lower priority than
	//the latest object in queue
	bool insertElem(cObject *element, bool putFirstInQueue);

	bool insertElem(cObject *element) { return insertElem(element, false); }

	//Sets the maximum number of elements of the queue
	void setMaxLength(int num);

	//Updates the values of time of life of the packets when a new period starts
	int* updateQueue(int* regPck);

	//Look up for a message sent to MAC layer
	cObject* getSentPacket();

};

#endif /* CQUEUEWITHPRIORITY_H_ */
