#ifndef ANCHORAPPLAYER_H_
#define ANCHORAPPLAYER_H_

#include "AppLayer.h"

class ComputerAppLayer : public AppLayer
{
protected:

	cQueueWithPriority packetsQueue;	// FIFO to store the packets we receive in Com Sink 1 to send them back in Com Sink 2
	simtime_t *randomQueueTime;			// Vector of random times to transmit the queue along the Com Sink 2
	simtime_t stepTimeComSink2;			// Step time in which we divide the Com Sink 2 Phase - The guard time. We divide it in so many parts like elements in the queue
	int queueElementCounter;			// Variable to know how many queue elements have we already transmitted, therefore to calculate all the random transmitting times when = 0 or knowing which randomQueueTime is the next to use
	int maxQueueElements;				// Maximum number of packets in the queue to transmit in the next Com Sink 2 Phase

	cMessage *checkQueue;				// Variable to schedule the events to process the Queue elements

	int* hops;							// Variable to know hops from the PC to the anchors

	int* fromNode;						// Vector to count the amount of packets received from each node

	double* meanBroad1;					// Vector to save the mean sending time of broadcasts classified as type 1 per anchor
	double* meanBroad2;					// Vector to save the mean sending time of broadcasts classified as type 2 per anchor
	double* meanBroad3;					// Vector to save the mean sending time of broadcasts classified as type 3 per anchor
	double* meanBroad4;					// Vector to save the mean sending time of broadcasts classified as type 4 per anchor
	double* meanReport1;				// Vector to save the mean sending time of reports classified as type 1 per anchor
	double* meanReport2;				// Vector to save the mean sending time of reports classified as type 2 per anchor
	double* meanReport3;				// Vector to save the mean sending time of reports classified as type 3 per anchor
	double* meanReport4;				// Vector to save the mean sending time of reports classified as type 4 per anchor
	double* meanRequest;				// Vector to save the mean sending time of ask per anchor

	double* minBroad1;					// Vector to save the minimum sending time of broadcasts classified as type 1 per anchor
	double* minBroad2;					// Vector to save the minimum sending time of broadcasts classified as type 2 per anchor
	double* minBroad3;					// Vector to save the minimum sending time of broadcasts classified as type 3 per anchor
	double* minBroad4;					// Vector to save the minimum sending time of broadcasts classified as type 4 per anchor
	double* minReport1;					// Vector to save the minimum sending time of reports classified as type 1 per anchor
	double* minReport2;					// Vector to save the minimum sending time of reports classified as type 2 per anchor
	double* minReport3;					// Vector to save the minimum sending time of reports classified as type 3 per anchor
	double* minReport4;					// Vector to save the minimum sending time of reports classified as type 4 per anchor
	double* minRequest;					// Vector to save the minimum sending time of asks per anchor

	double* maxBroad1;					// Vector to save the maximum sending time of broadcasts classified as type 1 per anchor
	double* maxBroad2;					// Vector to save the maximum sending time of broadcasts classified as type 2 per anchor
	double* maxBroad3;					// Vector to save the maximum sending time of broadcasts classified as type 3 per anchor
	double* maxBroad4;					// Vector to save the maximum sending time of broadcasts classified as type 4 per anchor
	double* maxReport1;					// Vector to save the maximum sending time of reports classified as type 1 per anchor
	double* maxReport2;					// Vector to save the maximum sending time of reports classified as type 2 per anchor
	double* maxReport3;					// Vector to save the maximum sending time of reports classified as type 3 per anchor
	double* maxReport4;					// Vector to save the maximum sending time of reports classified as type 4 per anchor
	double* maxRequest;					// Vector to save the maximum sending time of asks per anchor

	double* meanDelayBroad1;			// Vector to save the mean time needed to reach the destination of broadcasts classified as type 1 per anchor
	double* meanDelayBroad2;			// Vector to save the mean time needed to reach the destination of broadcasts classified as type 2 per anchor
	double* meanDelayBroad3;			// Vector to save the mean time needed to reach the destination of broadcasts classified as type 3 per anchor
	double* meanDelayBroad4;			// Vector to save the mean time needed to reach the destination of broadcasts classified as type 4 per anchor
	double* meanDelayReport1;			// Vector to save the mean time needed to reach the destination of reports classified as type 1 per anchor
	double* meanDelayReport2;			// Vector to save the mean time needed to reach the destination of reports classified as type 2 per anchor
	double* meanDelayReport3;			// Vector to save the mean time needed to reach the destination of reports classified as type 3 per anchor
	double* meanDelayReport4;			// Vector to save the mean time needed to reach the destination of reports classified as type 4 per anchor
	double* meanDelayRequest;			// Vector to save the mean time needed to reach the destination of asks per anchor

	double* minDelayBroad1;				// Vector to save the minimum time needed to reach the destination of broadcasts classified as type 1 per anchor
	double* minDelayBroad2;				// Vector to save the minimum time needed to reach the destination of broadcasts classified as type 2 per anchor
	double* minDelayBroad3;				// Vector to save the minimum time needed to reach the destination of broadcasts classified as type 3 per anchor
	double* minDelayBroad4;				// Vector to save the minimum time needed to reach the destination of broadcasts classified as type 4 per anchor
	double* minDelayReport1;			// Vector to save the minimum time needed to reach the destination of reports classified as type 1 per anchor
	double* minDelayReport2;			// Vector to save the minimum time needed to reach the destination of reports classified as type 2 per anchor
	double* minDelayReport3;			// Vector to save the minimum time needed to reach the destination of reports classified as type 3 per anchor
	double* minDelayReport4;			// Vector to save the minimum time needed to reach the destination of reports classified as type 4 per anchor
	double* minDelayRequest;			// Vector to save the minimum time needed to reach the destination of asks per anchor

	double* maxDelayBroad1;				// Vector to save the maximum time needed to reach the destination of broadcasts classified as type 1 per anchor
	double* maxDelayBroad2;				// Vector to save the maximum time needed to reach the destination of broadcasts classified as type 1 per anchor
	double* maxDelayBroad3;				// Vector to save the maximum time needed to reach the destination of broadcasts classified as type 1 per anchor
	double* maxDelayBroad4;				// Vector to save the maximum time needed to reach the destination of broadcasts classified as type 1 per anchor
	double* maxDelayReport1;			// Vector to save the maximum time needed to reach the destination of reports classified as type 1 per anchor
	double* maxDelayReport2;			// Vector to save the maximum time needed to reach the destination of reports classified as type 2 per anchor
	double* maxDelayReport3;			// Vector to save the maximum time needed to reach the destination of reports classified as type 3 per anchor
	double* maxDelayReport4;			// Vector to save the maximum time needed to reach the destination of reports classified as type 4 per anchor
	double* maxDelayRequest;			// Vector to save the maximum time needed to reach the destination of asks per anchor

	double* delayBroad1;				// Vector to save the sending time of a broadcast classified as type 1
	double* delayBroad2;				// Vector to save the sending time of a broadcast classified as type 2
	double* delayBroad3;				// Vector to save the sending time of a broadcast classified as type 3
	double* delayBroad4;				// Vector to save the sending time of a broadcast classified as type 4
	double* delayReport1;				// Vector to save the sending time of a report classified as type 1
	double* delayReport2;				// Vector to save the sending time of a report classified as type 1
	double* delayReport3;				// Vector to save the sending time of a report classified as type 1
	double* delayReport4;				// Vector to save the sending time of a report classified as type 1
	double* delayRequest;				// Vector to save the sending time of an ask

	double* timeBroad1;					// Vector to save the time needed to reach the destination of a broadcast classified as type 1
	double* timeBroad2;					// Vector to save the time needed to reach the destination of a broadcast classified as type 2
	double* timeBroad3;					// Vector to save the time needed to reach the destination of a broadcast classified as type 3
	double* timeBroad4;					// Vector to save the time needed to reach the destination of a broadcast classified as type 4
	double* timeReport1;				// Vector to save the time needed to reach the destination of a report classified as type 1
	double* timeReport2;				// Vector to save the time needed to reach the destination of a report classified as type 2
	double* timeReport3;				// Vector to save the time needed to reach the destination of a report classified as type 3
	double* timeReport4;				// Vector to save the time needed to reach the destination of a report classified as type 4
	double* timeRequest;				// Vector to save the time needed to reach the destination of an ask

	bool* receivedId;					// Vector to control if a particular packet from an anchor has been received

    // Modified by Victor

    int *packetsResend;            // Packets that were successfully resend.
    int numPckToSentByPeriod;        // Saves the number of packets originally in queue and the received to route by period
    bool pktRepeated;                     // Flag to indicate if a packet is repeated

public:
	virtual ~ComputerAppLayer();

	virtual void initialize(int stage);

	virtual void finish();

protected:
	/** @brief Handle self messages such as timer... */
	virtual void handleSelfMsg(cMessage *msg);

	/** @brief Handle messages from lower layer */
	virtual void handleLowerMsg(cMessage *msg);

	/** @brief Handle control messages from lower layer */
	virtual void handleLowerControl(cMessage *msg);

	/** @brief Handle messages from upper layer */
	virtual void handleUpperMsg(cMessage *msg)
	{
		opp_error("ComputerAppLayer has no upper layers!");
		delete msg;
	}

	/** @brief Handle control messages from upper layer */
	virtual void handleUpperControl(cMessage *msg)
	{
		opp_error("ComputerAppLayer has no upper layers!");
		delete msg;
	}

	// Returns from the already assigned slots how many times a nicId appears
	int hasSlot(int *slots, int *slotCounter, int numSlots, int anchor, int numAnchors);

	// Order Queue min to max in number of slots assigned (numberTimesQueue)
	void orderQueue(int *queue, int *numerTimesQueue, int queueCounter);
};

#endif
