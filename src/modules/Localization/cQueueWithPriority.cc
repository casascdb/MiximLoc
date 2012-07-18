/*
 * cQueueWithPriority.c
 *
 *  Created on: 29/06/2011
 *      Author: Rafel Estelrich Moreno
 */

#include "cQueueWithPriority.h"

Register_Class(cQueueWithPriority);

void cQueueWithPriority::setMaxLength(int num)
{
	maxElem = num;
	reportQueueDrop = (int*)calloc(sizeof(int), 4);
	broadQueueDrop = (int*)calloc(sizeof(int), 4);
	requestQueueDrop = 0;
	memset(reportQueueDrop, 0, sizeof(int)*4);
	memset(broadQueueDrop, 0, sizeof(int)*4);
}

bool cQueueWithPriority::firstHasPriority(cObject *a, cObject *b)
{
	//Parameter load
	ApplPkt* A = check_and_cast<ApplPkt*>((cMessage*)a);
	ApplPkt* B = check_and_cast<ApplPkt*>((cMessage*)b);

	//Check if the packet A has a higher priority or lower or same time of life in case the two packets have the same priority
	if((A->getPriority() > B->getPriority()) || ((A->getPriority() == B->getPriority()) && (A->getTimeOfLife() < B->getTimeOfLife())))
		return true;
	else
		return false;
}

void cQueueWithPriority::sortQueue()
{
	int i, j;
	cObject* buffer;
	bool move = false;

	//From the second element in queue to the last, we check if that packet should be placed before in the queue
	for(i=1; i<getLength(); i++)
	{
		//Comparison between packets start from the position immediately before
		j=i-1;

		//If our packet has priority over the one in that position, we mark our packet for moving, and set, as next
		//packet to compare, the one before. Since we arrange the elements from the first, we can be sure that, if we
		//find a packet with a priority higher than ours, before that packet no other exist with a lower priority.
		//Hence, we can settle in that position or when we arrive to the beginning.
		while(firstHasPriority(get(i), get(j)) && j>=0) {
			move = true;
			j--;
		}
		//We only have to move the packet if before exist packets with lower priority.
		if (move) {
			//Disable the flag for next iteration
			move = false;
			//We take out our packet on the queue
			buffer = remove(get(i));
			//And we put it immediately before the last packet who has less priority than us, which will be our new
			//place in queue. The "j" integer value does not mean our position in queue but the one related to the very
			//after packet we can precede, as in a circular queue all elements point and are pointed by others. Note that
			//the lower position value we can reach is -1, meaning before the first element in queue, although queue[-1]
			//would have no meaning.
			insertBefore(get(j+1), buffer);
		}
	}
}

bool cQueueWithPriority::insertElem(cObject *element, bool putFirstInQueue)
{
	int index = maxElem-1;
	//First we check if queue is full
	if (getLength() >= maxElem){
		//If that is the case, we check if the packet we want to include has priority over the one in last position
		//But first, we check if the last message is in the MAC. If it is the case, we have to save this packet; we check the one before.
		if(check_and_cast<ApplPkt*>(get(index))->getSendInProcess())
			index--;
		if(firstHasPriority(element, get(index))) {
			//So, if we have priority, we discard the last element available on queue
			if(check_and_cast<ApplPkt*>(get(index))->getWasBroadcast())
				broadQueueDrop[check_and_cast<ApplPkt*>(get(index))->getPriority()-1]++;
			else if(check_and_cast<ApplPkt*>(get(index))->getWasReport())
				reportQueueDrop[check_and_cast<ApplPkt*>(get(index))->getPriority()-1]++;
			else if(check_and_cast<ApplPkt*>(get(index))->getWasRequest())
				requestQueueDrop++;
			delete remove(get(index));
		}
		//Or we exit informing that we cannot insert the element because queue is full with more important packets.
		else
			return false;
	}

	//Here we have space in the queue because there has always been or because our packet had priority enough to take
	//out one packet of the queue.
	//Finally we place the object in front of the queue
	if(putFirstInQueue && (getLength() > 0))
		insertBefore(get(0), element);
	//Or we push the object to the last position of the queue.
	else
		insert(element);
	//We rearrange the queue
	sortQueue();
	//And finally, we exit informing the operation concluded succesfully.
	return true;
}

int* cQueueWithPriority::updateQueue(int* regPck)
{
	int i, *drop, priority;
	ApplPkt* buffer;

	drop = (int*)calloc(sizeof(int), 9);
	memset(drop, 0, sizeof(int)*9);

	for (i=0; i<getLength(); i++) {
		buffer = check_and_cast<ApplPkt*>((cMessage*)get(i));
		if(buffer->getTimeOfLife() == 1) {
			if(regPck[buffer->getCreatedIn()*10000 + buffer->getId()] == 0) {
				priority = buffer->getPriority();
				//priority = buffer->getNodeMode();
				if(buffer->getWasBroadcast())
					drop[priority-1]++;
				else if(buffer->getWasReport())
					drop[priority+3]++;
				else if(buffer->getWasRequest())
					drop[8]++;
				regPck[buffer->getCreatedIn()*10000 + buffer->getId()] = 2;
			}
			delete remove(get(i));
			i--;
		}
		else if (buffer->getTimeOfLife() != -1)
			buffer->setTimeOfLife(buffer->getTimeOfLife() - 1);
	}
	return drop;
}

cObject* cQueueWithPriority::getSentPacket()
{
	int i;
	ApplPkt* buffer;

	for (i = 0; i<getLength(); i++) {
		buffer = check_and_cast<ApplPkt*>((cMessage*)get(i));
		if(buffer->getSendInProcess())
			break;
	}
	return remove(get(i));
}
