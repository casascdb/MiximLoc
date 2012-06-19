#include "AnchorAppLayer.h"

Define_Module(AnchorAppLayer);

void AnchorAppLayer::initialize(int stage)
{
	AppLayer::initialize(stage);

	if(stage == 0) {
		// Parameter load
		syncInSlot = par("syncInSlot");
		phaseRepetitionNumber = par("phaseRepetitionNumber");
	   	//syncFirstMaxRandomTime = par("syncFirstMaxRandomTime");
		// Now the first maximum interpacket transmission time is the same as the rest of the times, it could be different, that's why we have 2 parameters
		syncFirstMaxRandomTime = par("syncRestMaxRandomTimes");
	   	syncRestMaxRandomTimes = par("syncRestMaxRandomTimes");

	   	// Variable initialization, we could change this into parameters if necessary
	   	syncPhaseNumber = SYNC_PHASE_1;
		syncPacketsPerSyncPhaseCounter = 1;
		scheduledSlot = 0;
		broadcastCounter = (int*)calloc(sizeof(int), numberOfNodes);
		broadPriority = (int*)calloc(sizeof(int), numberOfNodes);
		broadNodeMode = (int*)calloc(sizeof(int), numberOfNodes);
	} else if (stage == 1) {
		// Assign the type of host to 1 (anchor)
		anchor = cc->findNic(myNetwAddr);
		anchor->moduleType = 1;
	// We have to wait till stage 4 to make this because till stage 3 the slot configuration is not done
	} else if (stage == 2) {
		int list[25] = {1,2,3,25,3,6,2,8,25,25,11,7,8,9,9,16,12,12,13,14,16,17,17,18,19}; // Lists the parent Anchor of each Anchor
		parentAnchor = list[getParentModule()->getIndex()];	// Sents the parent anchor for the current anchor
		numPckToSent = 0;
		numPckSentOk = 0;
		selfSuccess = 1;
		parentSuccess = 1;
		meanWindow = 4;	// Sets the lenght of the effectiveness mean window
		meanIndex = 0;
		formerSuccess = (double*)calloc(sizeof(double), meanWindow);
		for(int i = 0; i < meanWindow; i++) formerSuccess[i] = 1;
		successToTx = 1;
		meanSuccess = 0;
		numPeriods = 0;
	} else if (stage == 4) {
		// Necessary variables for the queue initialization
		checkQueue = new cMessage("transmit queue elements", CHECK_QUEUE);
		queueElementCounter = 0;
		maxQueueElements = 30;
		priorityLengthAddition = 5; //n = 2; m = 3
		packetsQueue.setMaxLength(maxQueueElements);
		transfersQueue.setMaxLength(maxQueueElements);
		macDeviceFree = true;

		int anchNum = getParentModule()->getIndex();
		// Sets the amount of hops between the current anchor and the coordinator
		if((anchNum == 0) || (anchNum == 5) || (anchNum == 10) || (anchNum == 15) || (anchNum == 20) || (anchNum == 21) || (anchNum == 22) ||
				(anchNum == 23) || (anchNum == 24))
			hops = 3;
		if((anchNum == 1) || (anchNum == 6) || (anchNum == 11) || (anchNum == 16) || (anchNum == 17) || (anchNum == 18) || (anchNum == 19))
			hops = 2;
		if((anchNum == 4) || (anchNum == 2) || (anchNum == 7) || (anchNum == 12) || (anchNum == 13) || (anchNum == 14))
			hops = 1;
		if((anchNum == 3) || (anchNum == 8) || (anchNum == 9))
			hops = 0;

		fromNode = (int*)calloc(sizeof(int), numberOfNodes);
		memset(fromNode, 0, sizeof(int)*numberOfNodes);

		numPck = 0;
		regPck = (int*)calloc(sizeof(int), numberOfAnchors*10000);
		memset(regPck, 0, sizeof(int)*(numberOfAnchors)*10000);

		broadNew = (int*)calloc(sizeof(int), 5);
		broadDrop = (int*)calloc(sizeof(int), 5);
		broadNoAck = (int*)calloc(sizeof(int), 5);
		broadPckQueue = (int*)calloc(sizeof(int), 5);
		broadUpdate = (int*)calloc(sizeof(int), 5);
		broadNoTime = (int*)calloc(sizeof(int), 5);
		broadSent = (int*)calloc(sizeof(int), 5);
		broadSentOK = (int*)calloc(sizeof(int), 5);

		reportNew = (int*)calloc(sizeof(int), 5);
		reportDrop = (int*)calloc(sizeof(int), 5);
		reportNoAck = (int*)calloc(sizeof(int), 5);
		reportPckQueue = (int*)calloc(sizeof(int), 5);
		reportUpdate = (int*)calloc(sizeof(int), 5);
		reportNoTime = (int*)calloc(sizeof(int), 5);
		reportSent = (int*)calloc(sizeof(int), 5);
		reportSentOK = (int*)calloc(sizeof(int), 5);

		drop = (int*)calloc(sizeof(int), 9);
		memset(drop, 0, sizeof(int)*9);

		for(int i = 0; i<5; i++) {
			broadNew[i] = 0;
			broadDrop[i] = 0;
			broadNoAck[i] = 0;
			broadPckQueue[i] = 0;
			broadUpdate[i] = 0;
			broadNoTime[i] = 0;
			broadSent[i] = 0;
			broadSentOK[i] = 0;

			reportNew[i] = 0;
			reportDrop[i] = 0;
			reportNoAck[i] = 0;
			reportPckQueue[i] = 0;
			reportUpdate[i] = 0;
			reportNoTime[i] = 0;
			reportSent[i] = 0;
			reportSentOK[i] = 0;
		}

		requestNew = 0;
		requestDrop = 0;
		requestNoAck = 0;
		requestPckQueue = 0;
		requestOK = 0;
		requestUpdate = 0;
		requestNoTime = 0;
		requestSent = 0;
		requestSentOK = 0;

		// Broadcast message, slotted or not is without CSMA, we wait the random time in the Appl Layer
		delayTimer = new cMessage("sync-delay-timer", SEND_SYNC_TIMER_WITHOUT_CSMA);

	}
}

AnchorAppLayer::~AnchorAppLayer() {
	cancelAndDelete(delayTimer);
	cancelAndDelete(checkQueue);
	cancelAndDelete(beginPhases);
	packetsQueue.clear();
	for(cQueue::Iterator iter(packetsQueue, 1); !iter.end(); iter++) {
		delete(iter());
	}
	for(cQueue::Iterator iter(transfersQueue, 1); !iter.end(); iter++) {
		delete(iter());
	}
}


void AnchorAppLayer::finish()
{
	recordScalar("Dropped Packets in AN - No ACK received", nbPacketDroppedNoACK);
	recordScalar("Dropped Packets in AN - Max MAC BackOff tries", nbPacketDroppedBackOff);
	recordScalar("Dropped Packets in AN - App Queue Full", nbPacketDroppedAppQueueFull);
	recordScalar("Dropped Packets in AN - Mac Queue Full", nbPacketDroppedMacQueueFull);
	recordScalar("Dropped Packets in AN - No Time in the Phase", nbPacketDroppedNoTimeApp);
	recordScalar("Erased Packets in AN - No more BackOff retries", nbErasedPacketsBackOffMax);
	recordScalar("Erased Packets in AN - No more No ACK retries", nbErasedPacketsNoACKMax);
	recordScalar("Erased Packets in AN - No more MAC Queue Full retries", nbErasedPacketsMacQueueFull);
	recordScalar("Number of AN Broadcasts Successfully sent", nbBroadcastPacketsSent);
	recordScalar("Number of AN Reports with ACK", nbReportsWithACK);
	recordScalar("Number of Broadcasts received in AN", nbBroadcastPacketsReceived);
	recordScalar("Number of Reports received in AN", nbReportsReceived);
	recordScalar("Number of Reports really for me received in AN", nbReportsForMeReceived);

	recordScalar("Mean of successful packets", meanSuccess/numPeriods);

	recordScalar("Number of packets 1 created by a mobile broadcast", broadNew[1]);
	recordScalar("Number of packets 1 created by a mobile broadcast dropped in packets queue", broadPckQueue[1] + packetsQueue.broadQueueDrop[0]);
	recordScalar("Number of packets 1 created by a mobile broadcast discarded due no ACK", broadNoAck[1]);
	recordScalar("Number of packets 1 created by a mobile broadcast dropped by backoff", broadDrop[1]);
	recordScalar("Number of packets 1 created by a mobile broadcast dropped by TOF update", broadUpdate[1]);
	recordScalar("Number of packets 1 created by a mobile broadcast erased because TX had to stop", broadNoTime[1]);
	recordScalar("Number of packets 1 created by a mobile broadcast sent from this anchor", broadSent[1]);
	recordScalar("Number of packets 1 created by a mobile broadcast sent from this anchor TX OK", broadSentOK[1]);

	recordScalar("Number of packets 1 created by a mobile report", reportNew[1]);
	recordScalar("Number of packets 1 created by a mobile report dropped in packets queue", reportPckQueue[1] + packetsQueue.reportQueueDrop[0]);
	recordScalar("Number of packets 1 created by a mobile report discarded due no ACK", reportNoAck[1]);
	recordScalar("Number of packets 1 created by a mobile report dropped by backoff", reportDrop[1]);
	recordScalar("Number of packets 1 created by a mobile report dropped by TOF update", reportUpdate[1]);
	recordScalar("Number of packets 1 created by a mobile report erased because TX had to stop", reportNoTime[1]);
	recordScalar("Number of packets 1 created by a mobile report sent from this anchor", reportSent[1]);
	recordScalar("Number of packets 1 created by a mobile report sent from this anchor TX OK", reportSentOK[1]);

	recordScalar("Number of packets 2 created by a mobile broadcast", broadNew[2]);
	recordScalar("Number of packets 2 created by a mobile broadcast dropped in packets queue", broadPckQueue[2] + packetsQueue.broadQueueDrop[1]);
	recordScalar("Number of packets 2 created by a mobile broadcast discarded due no ACK", broadNoAck[2]);
	recordScalar("Number of packets 2 created by a mobile broadcast dropped by backoff", broadDrop[2]);
	recordScalar("Number of packets 2 created by a mobile broadcast dropped by TOF update", broadUpdate[2]);
	recordScalar("Number of packets 2 created by a mobile broadcast erased because TX had to stop", broadNoTime[2]);
	recordScalar("Number of packets 2 created by a mobile broadcast sent from this anchor", broadSent[2]);
	recordScalar("Number of packets 2 created by a mobile broadcast sent from this anchor TX OK", broadSentOK[2]);

	recordScalar("Number of packets 2 created by a mobile report", reportNew[2]);
	recordScalar("Number of packets 2 created by a mobile report dropped in packets queue", reportPckQueue[2] + packetsQueue.reportQueueDrop[1]);
	recordScalar("Number of packets 2 created by a mobile report discarded due no ACK", reportNoAck[2]);
	recordScalar("Number of packets 2 created by a mobile report dropped by backoff", reportDrop[2]);
	recordScalar("Number of packets 2 created by a mobile report dropped by TOF update", reportUpdate[2]);
	recordScalar("Number of packets 2 created by a mobile report erased because TX had to stop", reportNoTime[2]);
	recordScalar("Number of packets 2 created by a mobile report sent from this anchor", reportSent[2]);
	recordScalar("Number of packets 2 created by a mobile report sent from this anchor TX OK", reportSentOK[2]);

	recordScalar("Number of packets 3 created by a mobile broadcast", broadNew[3]);
	recordScalar("Number of packets 3 created by a mobile broadcast dropped in packets queue", broadPckQueue[3] + packetsQueue.broadQueueDrop[2]);
	recordScalar("Number of packets 3 created by a mobile broadcast discarded due no ACK", broadNoAck[3]);
	recordScalar("Number of packets 3 created by a mobile broadcast dropped by backoff", broadDrop[3]);
	recordScalar("Number of packets 3 created by a mobile broadcast dropped by TOF update", broadUpdate[3]);
	recordScalar("Number of packets 3 created by a mobile broadcast erased because TX had to stop", broadNoTime[3]);
	recordScalar("Number of packets 3 created by a mobile broadcast sent from this anchor", broadSent[3]);
	recordScalar("Number of packets 3 created by a mobile broadcast sent from this anchor TX OK", broadSentOK[3]);

	recordScalar("Number of packets 3 created by a mobile report", reportNew[3]);
	recordScalar("Number of packets 3 created by a mobile report dropped in packets queue", reportPckQueue[3] + packetsQueue.reportQueueDrop[2]);
	recordScalar("Number of packets 3 created by a mobile report discarded due no ACK", reportNoAck[3]);
	recordScalar("Number of packets 3 created by a mobile report dropped by backoff", reportDrop[3]);
	recordScalar("Number of packets 3 created by a mobile report dropped by TOF update", reportUpdate[3]);
	recordScalar("Number of packets 3 created by a mobile report erased because TX had to stop", reportNoTime[3]);
	recordScalar("Number of packets 3 created by a mobile report sent from this anchor", reportSent[3]);
	recordScalar("Number of packets 3 created by a mobile report sent from this anchor TX OK", reportSentOK[3]);

	recordScalar("Number of packets 4 created by a mobile broadcast", broadNew[4]);
	recordScalar("Number of packets 4 created by a mobile broadcast dropped in packets queue", broadPckQueue[4] + packetsQueue.broadQueueDrop[3]);
	recordScalar("Number of packets 4 created by a mobile broadcast discarded due no ACK", broadNoAck[4]);
	recordScalar("Number of packets 4 created by a mobile broadcast dropped by backoff", broadDrop[4]);
	recordScalar("Number of packets 4 created by a mobile broadcast dropped by TOF update", broadUpdate[4]);
	recordScalar("Number of packets 4 created by a mobile broadcast erased because TX had to stop", broadNoTime[4]);
	recordScalar("Number of packets 4 created by a mobile broadcast sent from this anchor", broadSent[4]);
	recordScalar("Number of packets 4 created by a mobile broadcast sent from this anchor TX OK", broadSentOK[4]);

	recordScalar("Number of packets 4 created by a mobile report", reportNew[4]);
	recordScalar("Number of packets 4 created by a mobile report dropped in packets queue", reportPckQueue[4] + packetsQueue.reportQueueDrop[3]);
	recordScalar("Number of packets 4 created by a mobile report discarded due no ACK", reportNoAck[4]);
	recordScalar("Number of packets 4 created by a mobile report dropped by backoff", reportDrop[4]);
	recordScalar("Number of packets 4 created by a mobile report dropped by TOF update", reportUpdate[4]);
	recordScalar("Number of packets 4 created by a mobile report erased because TX had to stop", reportNoTime[4]);
	recordScalar("Number of packets 4 created by a mobile report sent from this anchor", reportSent[4]);
	recordScalar("Number of packets 4 created by a mobile report sent from this anchor TX OK", reportSentOK[4]);

	recordScalar("Number of packets created by a mobile request", requestNew);
	recordScalar("Number of packets created by a mobile request TX OK", requestOK);
	recordScalar("Number of packets created by a mobile request dropped in packets queue",requestPckQueue);
	recordScalar("Number of packets created by a mobile request discarded due no ACK", requestNoAck);
	recordScalar("Number of packets created by a mobile request dropped by backoff", requestDrop);
	recordScalar("Number of packets created by a mobile request dropped by TOF update", requestUpdate);
	recordScalar("Number of packets created by a mobile request erased because TX had to stop", requestNoTime);
	recordScalar("Number of packets created by a mobile request sent from this anchor", requestSent);
	recordScalar("Number of packets created by a mobile request sent from this anchor TX OK", requestSentOK);

	for(int i = 0; i < numberOfNodes; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Number of packets sent from mobile node %d", i);
		recordScalar(buffer, fromNode[i]);
	}
}

void AnchorAppLayer::handleSelfMsg(cMessage *msg)
{
	switch(msg->getKind())
	{
	case SEND_SYNC_TIMER_WITHOUT_CSMA:
		sendBroadcast(); // Send the broadcast
		// If we still have full phases to do
		if (syncInSlot) // Slotted mode
		{
			scheduledSlot++;
			if (scheduledSlot < anchor->numSlots) { // If an Anchor has more than one slot per slot period (reuse of slots), first we assign all the slots
				scheduleAt(nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime), delayTimer);
				EV << "Time for next Sync Packet " << nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime) << endl;
			} else { // Calculate the first time of the next slot period
				scheduledSlot = 0; // Reset of the scheduled slots in this slot period (when more than one slot per Anchor)
				if (syncPacketsPerSyncPhaseCounter < syncPacketsPerSyncPhase) {
					syncPacketsPerSyncPhaseCounter++; // Increments the mini sync phases covered from a total of syncPacketsPerSyncPhase
					nextPhaseStart = nextPhaseStart + (timeSyncPhase / syncPacketsPerSyncPhase); // Here we add the time of a mini sync phase (another slot period)
					scheduleAt(nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime), delayTimer);
					EV << "Time for next Sync Packet " << nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime) << endl;
				} else { // If we reached all the slot periods (mini sync phase) from a sync phase, don't schedule any more, in next sync phase all will start again
					syncPacketsPerSyncPhaseCounter = 1;
				}
			}
		}
		else
		{
			// ---------------------------------------------------------------------------
			// - Here we would manage the sync packets when they are not in slotted mode -
			// ---------------------------------------------------------------------------
		}
		break;
	case CHECK_QUEUE:
		if (packetsQueue.length() > 0) { // If there are messages to send
			ApplPkt* msg = check_and_cast<ApplPkt*>((cMessage *)packetsQueue.pop());
			EV << "Sending packet from the Queue to Anchor with addr. " << msg->getDestAddr() << endl;

			macDeviceFree = false; // Mark the MAC as occupied
			if(msg->getTimestampAnchorTX() == 0) { // Save the sending times if the message in enrouted for the first time
				msg->setTimestampAnchorTX(simTime().dbl());
				msg->setTimestampComRelated(simTime().dbl() - comsinkPhaseStartTime);
			}
			//Counts the number of packets sent from the anchor depending to its type
			if(msg->getWasBroadcast())
				broadSent[msg->getPriority()]++;
			else if(msg->getWasReport())
				reportSent[msg->getPriority()]++;
			else if(msg->getWasRequest())
				requestSent++;

			transfersQueue.insert(msg->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
			sendDown(msg);
			if (packetsQueue.length() > 0) { // We have to check again if the queue has still elements after taking the element previously
				// Schedule the next queue element in the next random time
				queueElementCounter++;
				EV << "Still " << packetsQueue.length() << " elements in the Queue." << endl;
				EV << "Random Transmission number " << queueElementCounter + 1 << " at : " << randomQueueTime[queueElementCounter] << " s" << endl;
				scheduleAt(randomQueueTime[queueElementCounter], checkQueue);
			}
		}
		break;
	case BEGIN_PHASE:
		// Empty the transmission Queue
		if (!transfersQueue.empty()) {
			EV << "Emptying the queue with " << transfersQueue.length() << " elements in phase change" << endl;
			nbPacketDroppedNoTimeApp = nbPacketDroppedNoTimeApp + transfersQueue.length();
			//Flushes the transfersQueue if not empty for a new phase
			//transfersQueue.clear();
//			while(!transfersQueue.empty()) {
//				ApplPkt* pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
//				if(pkt->getWasBroadcast())
//					broadNoTime[pkt->getPriority()]++;
//				else if(pkt->getWasReport())
//					reportNoTime[pkt->getPriority()]++;
//				else if(pkt->getWasRequest())
//					requestNoTime++;
//				delete pkt;
//			}

			//Packets are recovered for a new attempt...
			while(!transfersQueue.empty()) {
				ApplPkt* pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
				if(pkt->getTimeOfLife() != 1) { // ...if they still have enough TTL...
					if(packetsQueue.insertElem(pkt)){} // ...by reinserting them into the anchors queue
					else { // Count if there was no space in queue
						if(pkt->getWasBroadcast())
							broadPckQueue[pkt->getPriority()]++;
						else if(pkt->getWasReport())
							reportPckQueue[pkt->getPriority()]++;
						else if(pkt->getWasRequest())
							requestPckQueue++;
						delete pkt;
					}
				} else { // Count if they had not enough TTL
					if(pkt->getWasBroadcast())
						broadNoTime[pkt->getPriority()]++;
					else if(pkt->getWasReport())
						reportNoTime[pkt->getPriority()]++;
					else if(pkt->getWasRequest())
						requestNoTime++;
					delete pkt;
				}
			}
		}
			// --------------------------------------------------------------------------------------
			// - If we don't want to clear the queue and lose the packets, --------------------------
			// - we could somehow here leave the rest of the queue for the next full phase (period) -
			// --------------------------------------------------------------------------------------
		else {
			EV << "App Transmission Queue empty in phase change." << endl;
		}
		if (phase == AppLayer::SYNC_PHASE_1) { //Update the values of TOF when the new period starts
			drop = packetsQueue.updateQueue(regPck); // A buffer must be used to know how many and why the packet were dropped inside the update method
			for(int i = 1; i < 5; i++) {
				broadUpdate[i] = broadUpdate[i] + drop[i-1];
				reportUpdate[i] = reportUpdate[i] + drop[i+3];
			}
			requestUpdate = requestUpdate + drop[8];
		}
		switch (nextPhase)
		{
		case AppLayer::SYNC_PHASE_1:
			phase = AppLayer::SYNC_PHASE_1;
			nextPhase = AppLayer::REPORT_PHASE;
			nextPhaseStartTime = simTime() + timeSyncPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			// Schedule the sync packets. If we execute some full phase (-1 not limited full phases)

			EV << "The number of packets sent from this anchor is: " << numPckToSent << endl;
			EV << "The number of packets correctly sent from this anchor is: " << numPckSentOk << endl;

			if(numPckToSent == 0 || (numPckSentOk > numPckToSent)) 	// If no packets were to be sent
				selfSuccess = 1;									// the success could be calculated; it is set to 1
			else
				selfSuccess = numPckSentOk/numPckToSent;			// else, the real ratio is calculated

			EV << "Hence, the success of my Tx is: " << selfSuccess << endl;
			EV << "My father is the anchor: " << parentAnchor << endl;
			EV << "The success of my anchor father was: " << parentSuccess << endl;

			// Maybe we should only take in account the periods in which we really sent packets in order to have a more accurate rate
			formerSuccess[meanIndex] = selfSuccess;
			meanIndex++;
			meanIndex = meanIndex % meanWindow;

			//To test the improvement BORRAR SI NO FUNCIONA!
//			if(numPckToSent != 0) {
//				formerSuccess[meanIndex] = selfSuccess;
//				meanIndex++;
//				meanIndex = meanIndex % meanWindow;
//			}

			successToTx = 0;
			// Calculates the mean of the successes of the last periods
			for(int i = 0; i < meanWindow; i++) {
				EV << "Former successes for me were: " << formerSuccess[i] << endl;
				successToTx = successToTx + formerSuccess[i];
			}
			successToTx = (successToTx / meanWindow) * parentSuccess; // Sets the final success to transmit to the air

			if(numPckToSent != 0) {
				meanSuccess = meanSuccess + successToTx;
				numPeriods++;
			}

			EV << "So, the real success for me is: " << successToTx << endl;

			numPckToSent = numPckSentOk = 0;

			if (phaseRepetitionNumber != 0 && syncInSlot) { // If sync phase slotted
				nextPhaseStart = simTime();
				scheduleAt(nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime), delayTimer);
				EV << "Time for next Sync Packet " << nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime) << endl;
				scheduledSlot++;
			} else if (phaseRepetitionNumber != 0) { // If sync phase with random transmissions
				scheduleAt(simTime() + uniform(0, syncFirstMaxRandomTime, 0), delayTimer);
			}
			break;
		case AppLayer::REPORT_PHASE:
			phase = AppLayer::REPORT_PHASE;
			nextPhase = AppLayer::VIP_PHASE;
			nextPhaseStartTime = simTime() + timeReportPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case AppLayer::VIP_PHASE:
			phase = AppLayer::VIP_PHASE;
			nextPhase = AppLayer::SYNC_PHASE_2;
			nextPhaseStartTime = simTime() + timeVIPPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case AppLayer::SYNC_PHASE_2:
			phase = AppLayer::SYNC_PHASE_2;
			nextPhase = AppLayer::COM_SINK_PHASE_1;
			nextPhaseStartTime = simTime() + timeSyncPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			for (int i = 0; i < numberOfNodes; i++) {
				if (broadcastCounter[i] > 0) { // If the AN has received at least one Broadcast
					ApplPkt *pkt = new ApplPkt("Report with CSMA", REPORT_WITH_CSMA);
					pkt->setBitLength(bcastMixANPacketLength + priorityLengthAddition);
					pkt->setRealDestAddr(getParentModule()->getParentModule()->getSubmodule("computer", 0)->findSubmodule("nic"));
					pkt->setDestAddr(pkt->getRealDestAddr());
					pkt->setSrcAddr(myNetwAddr);
					pkt->setRealSrcAddr(getParentModule()->getParentModule()->getSubmodule("node", i)->findSubmodule("nic"));
					pkt->setRetransmisionCounterBO(0);	// Reset the retransmission counter BackOff
					pkt->setRetransmisionCounterACK(0);	// Reset the retransmission counter ACK
					pkt->setCSMA(true);
					pkt->setPriority(broadPriority[i]);
					pkt->setNodeMode(broadNodeMode[i]);
					pkt->setFromNode(i);
					pkt->setTimeOfLife(1);
					pkt->setWasBroadcast(true);
					//broadNew[broadPriority]++;
					fromNode[i]++;
					broadNew[broadPriority[i]]++;
					pkt->setId(numPck);
					pkt->setCreatedIn(getParentModule()->getIndex());
					//regPck[pkt->getCreatedIn()*10000 + pkt->getId()]++;
					numPck++;

					if (packetsQueue.insertElem(pkt)) { // There is still place in the queue for this packet
						EV << "Enqueing broadcast RSSI values from Mobile Node " << i << endl;
					} else {
						EV << "Queue full, discarding packet" << endl;

						broadPckQueue[broadPriority[i]]++;

						nbPacketDroppedAppQueueFull++;
						delete pkt;
					}
				}
			}
			broadcastCounter = (int*)calloc(sizeof(int), numberOfNodes); // Reset the counter of broadcast a AN received from Mobile Nodes
			broadPriority = (int*)calloc(sizeof(int), numberOfNodes);
			broadNodeMode = (int*)calloc(sizeof(int), numberOfNodes);
			// Schedule the sync packets. If we execute some full phase (-1 not limited full phases)
			if (phaseRepetitionNumber != 0 && syncInSlot) { // If sync phase slotted
				nextPhaseStart = simTime();
				scheduleAt(nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime), delayTimer);
				EV << "Time for next Sync Packet " << nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime) << endl;
				scheduledSlot++;
			} else if (phaseRepetitionNumber != 0) { // If sync phase with random transmissions
				scheduleAt(simTime() + uniform(0, syncFirstMaxRandomTime, 0), delayTimer);
			}
			break;
		case AppLayer::COM_SINK_PHASE_1:
			phase = AppLayer::COM_SINK_PHASE_1;
			nextPhase = AppLayer::SYNC_PHASE_3;
			comsinkPhaseStartTime = simTime().dbl();
			nextPhaseStartTime = simTime() + timeComSinkPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			// At the beginning of the Com Sink 1 the Anchor checks its queue to transmit the elements and calculate all the random transmission times
			randomQueueTime = (simtime_t*)calloc(sizeof(simtime_t), maxQueueElements);
			if (packetsQueue.length() > 0) { // Only if the Queue has elements we do calculate all the intermediate times
				stepTimeComSink1 = (timeComSinkPhase - guardTimeComSinkPhase/* - (0.030 * hops)*/) / packetsQueue.length();
				//stepTimeComSink1 = 0.025;
				EV << "Transmitting the " << packetsQueue.length() << " elements of the queue in the following moments." << endl;
				for (int i = 0; i < packetsQueue.length(); i++) {
					randomQueueTime[i] = simTime() + (i * stepTimeComSink1) + uniform(0, stepTimeComSink1, 0);
					EV << "Time " << i << ": " << randomQueueTime[i] << endl;
				}
				numPckToSent = packetsQueue.length();
				queueElementCounter = 0; // Reset the index to know which random time from vector to use
				EV << "Still " << packetsQueue.length() << " elements in the Queue." << endl;
				EV << "Random Transmission number " << queueElementCounter + 1 << " at : " << randomQueueTime[queueElementCounter] << " s" << endl;
				scheduleAt(randomQueueTime[queueElementCounter], checkQueue);
			} else {
				EV << "Queue empty, Anchor has nothing to communicate this full phase (period)." << endl;
			}

			break;
		case AppLayer::SYNC_PHASE_3:
			phase = AppLayer::SYNC_PHASE_3;
			nextPhase = AppLayer::COM_SINK_PHASE_2;
			nextPhaseStartTime = simTime() + timeSyncPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			// Schedule the sync packets. If we execute some full phase (-1 not limited full phases)
			if (phaseRepetitionNumber != 0 && syncInSlot) { // If sync phase slotted
				nextPhaseStart = simTime();
				scheduleAt(nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime), delayTimer);
				EV << "Time for next Sync Packet " << nextPhaseStart + (anchor->transmisionSlot[scheduledSlot] * syncPacketTime) << endl;
				scheduledSlot++;
			} else if (phaseRepetitionNumber != 0) { // If sync phase with random transmissions
				scheduleAt(simTime() + uniform(0, syncFirstMaxRandomTime, 0), delayTimer);
			}
			if (phaseRepetitionNumber > 0) {
				phaseRepetitionNumber--; // If the number of full phases is limited, decrease one as we just finished one
			}
			break;
		case AppLayer::COM_SINK_PHASE_2:
			phase = AppLayer::COM_SINK_PHASE_2;
			nextPhase = AppLayer::SYNC_PHASE_1;
			nextPhaseStartTime = simTime() + timeComSinkPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		}
		break;
	default:
		EV << "Unkown selfmessage! -> delete, kind: "<<msg->getKind() <<endl;
		delete msg;
		break;
	}
}


void AnchorAppLayer::handleLowerMsg(cMessage *msg)
{
	// Convert the message to an Appl Packet
	ApplPkt* pkt = check_and_cast<ApplPkt*>(msg);
	EV << "Received packet from (" << pkt->getSrcAddr() << ", " << pkt->getRealSrcAddr() << ") to (" << pkt->getDestAddr() << ", " << pkt->getRealDestAddr() << ") with Name: " << pkt->getName() << endl;

	// Get control info attached by base class decapsMsg method to get RSSI and BER
	assert(dynamic_cast<NetwControlInfo*>(pkt->getControlInfo()));
	NetwControlInfo* cInfo = static_cast<NetwControlInfo*>(pkt->removeControlInfo());
	EV << "The RSSI in Appl Layer is: " << cInfo->getRSSI() << endl;
	EV << "The BER in Appl Layer is: " << cInfo->getBitErrorRate() << endl;

	// Pointer to the source host
	host = cc->findNic(pkt->getSrcAddr());



	// Filter first according to the phase we are in
	switch(phase)
	{
	case AppLayer::SYNC_PHASE_1:
	case AppLayer::SYNC_PHASE_2:
	case AppLayer::SYNC_PHASE_3:
		switch(pkt->getKind())
	    {
	    case AppLayer::REPORT_WITHOUT_CSMA:
	    case AppLayer::REPORT_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
				EV << "Discarding the packet, in Sync Phase Anchor cannot receive any Report from a Mobile Node" << endl;
	    	} else { // Computer or AN
	    		EV << "Discarding the packet, in Sync Phase Anchor cannot receive any Report from a Anchor" << endl;
	    	}
			delete msg;
			break;
	    case AppLayer::SYNC_MESSAGE_WITHOUT_CSMA:
	    case AppLayer::SYNC_MESSAGE_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
				EV << "Discarding the packet, in Sync Phase Anchor cannot receive any Broadcast from a Mobile Node" << endl;
	    	} else { // Computer or AN
	    		// --------------------------------------------------------------------------------------
	    		// - If we need to do something with the sync packets from the Anchors, it will be here -
	    		// --------------------------------------------------------------------------------------
	    		EV << "Broadcast received from anchor: " << simulation.getModule(pkt->getSrcAddr())->getParentModule()->getIndex() << endl;
	    		if(simulation.getModule(pkt->getSrcAddr())->getParentModule()->getIndex() == parentAnchor) // Gets the success of its parent anchor
	    		{
		    		EV << "It's success was: " << pkt->getBroadcastedSuccess() << endl;

		    		parentSuccess = pkt->getBroadcastedSuccess();

	    		}
	    		EV << "Discarding the packet, Anchor doesn't make anything with Sync Broadcast packets from the AN" << endl;
	    	}
			delete msg;
	    	break;
	    default:
			EV << "Unknown Packet! -> delete, kind: " << msg->getKind() << endl;
			delete msg;
			break;
	    }
		break;
	case AppLayer::REPORT_PHASE:
	case AppLayer::VIP_PHASE:
		switch(pkt->getKind())
	    {
	    case AppLayer::REPORT_WITHOUT_CSMA:
	    case AppLayer::REPORT_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
	    		nbReportsReceived++;
				if (pkt->getDestAddr() == myNetwAddr) { //The packet is for me
					if (pkt->getRealDestAddr() == myNetwAddr) { //The packet is really for me
						nbReportsForMeReceived++;
						// The packet its for me, probably Distributed-A, or request for configuration or info
						if (pkt->getRequestPacket()) { // We are in a request process, we received a request packet
							// -------------------------------------------------------------------------------------------
							// - Here we could write a conditional to send the info from a queue where we would have it --
							// - or a packet saying there is no info available yet, we will just send an standard packet -
							// -------------------------------------------------------------------------------------------
							// Change the packet info to answer the mobile node
							EV << "The Anchor answers immediately to the Mobile Node Request packet" << endl;
							pkt->setBitLength(answerANtoMNPacketLength);
							pkt->setDestAddr(pkt->getSrcAddr());
							pkt->setRealDestAddr(pkt->getRealSrcAddr());
							pkt->setSrcAddr(myNetwAddr);
							pkt->setRealSrcAddr(myNetwAddr);
							pkt->setRetransmisionCounterBO(0);	// Reset the retransmission counter BackOff
							pkt->setRetransmisionCounterACK(0);	// Reset the retransmission counter ACK
							pkt->setCSMA(true);
							pkt->setRequestPacket(false);
							pkt->setAskForRequest(false);
							transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
							sendDown(pkt);
							break;
						} else {
							EV << "The Anchor received a petition for him but not a Request, Mobile Node is in Distributed A mode" << endl;
							// The packet is a Distributed A packet, calculate position and put a message into the queue for the mobile node to wait when it asks
							// ----------------------------------------------------------------------------------------------------------------------------
							// - Here we have to calculate the position and put it into a queue, not delete the message, but in our case is not important -
							// ----------------------------------------------------------------------------------------------------------------------------
							delete msg;
						}
					}
					else
					{
						// The packet is probably for the computer, Centralized, we assign the dest. addr the computer addr and put it in the queue to transmit
						// We change also the src addr for the addr from the AN, so the next AN can find it in the routing table
						pkt->setDestAddr(pkt->getRealDestAddr());
						pkt->setSrcAddr(myNetwAddr);
						pkt->setRetransmisionCounterBO(0);	// Reset the retransmission counter BackOff
						pkt->setRetransmisionCounterACK(0);	// Reset the retransmission counter ACK
						pkt->setCSMA(true);

						fromNode[pkt->getFromNode()]++;

						if(pkt->getAskForRequest()) {
							pkt->setWasRequest(true);
							requestNew++;
						}
						else {
							pkt->setWasReport(true);
							reportNew[pkt->getPriority()]++;
						}

						pkt->setId(numPck);
						pkt->setCreatedIn(getParentModule()->getIndex());
						numPck++;

						pkt->setRequestPacket(false);
						pkt->setAskForRequest(false);

						if (packetsQueue.insertElem(pkt)) { // There is still place in the queue for more packets
							EV << "The packet is not really for me, is for the computer, I put it in the queue to send it in next Com Sink 1 to the computer" << endl;
						}
						else {
							EV << "The packet is not really for me, is for the computer, but Queue full, discarding packet" << endl;
							if(pkt->getWasReport())
								reportPckQueue[pkt->getPriority()]++;
							else if(pkt->getWasRequest())
								requestPckQueue++;
							nbPacketDroppedAppQueueFull++;
							delete msg;
						}
					}
				} else {
					// Don't do anything if a Mobile Node sent a Report and its not for me
					EV << "Discarding the packet, the MAC should have discarded the packet as is not for me" << endl;;
	    			delete msg;
				}
	    	} else { // Computer or AN
	    		EV << "Discarding the packet, in Report or VIP phases Anchor cannot receive any Report from an Anchor" << endl;
	    		delete msg;
	    	}
			break;
	    case AppLayer::SYNC_MESSAGE_WITHOUT_CSMA:
	    case AppLayer::SYNC_MESSAGE_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
				// If we receive a Broadcast Packet from a Mobile Node, we have to check if we received the last one in this phase
				// -----------------------------------------------------------------------------------------------------------------------------------------------------
				// - We should make here an array to store all the RSSI values we read from the broadcast to include them in the packet we send at the next Com Sink 1 -
				// -----------------------------------------------------------------------------------------------------------------------------------------------------
				// The computer will analyze all the received packets from different AN to estimate the position
	    		nbBroadcastPacketsReceived++;
	    		indexBroadcast = simulation.getModule(pkt->getSrcAddr())->getParentModule()->getIndex();
				broadcastCounter[indexBroadcast]++;
	    		broadPriority[indexBroadcast] = pkt->getPriority();
	    		broadNodeMode[indexBroadcast] = pkt->getNodeMode();
				EV << "Received the Broadcast Packet number " << broadcastCounter[indexBroadcast] << ". From the Mobile Node with Addr: " << pkt->getSrcAddr() << endl;
				delete msg;
	    	} else { // Computer or AN
	    		EV << "Discarding the packet, in Report or VIP phases Anchor cannot receive a Broadcast from any Anchor" << endl;
				delete msg;
	    	}
	    	break;
	    default:
			EV << "Unknown Packet! -> delete, kind: " << msg->getKind() << endl;
			delete msg;
			break;
	    }
		break;
	case AppLayer::COM_SINK_PHASE_1:
	case AppLayer::COM_SINK_PHASE_2:
		switch(pkt->getKind())
	    {
	    case AppLayer::REPORT_WITHOUT_CSMA:
	    case AppLayer::REPORT_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
				EV << "Discarding the packet, in Com Sink Phases Anchor cannot receive any Report from a Mobile Node" << endl;
				delete msg;
	    	} else { // Computer or AN
	    		nbReportsReceived++;
				if (pkt->getDestAddr() == myNetwAddr) { // The packet is for me
					if (pkt->getRealDestAddr() == myNetwAddr) { // The packet is really for me
						nbReportsForMeReceived++;
						// Communication computer - anchor or anchor - computer (the anchor - anchor communication is not happening in our study)
						EV << "The computer wanted to say something to this Anchor, but not for a Mobile Node" << endl;
						getParentModule()->bubble("I've received the message");
					} else { // The destination is a mobile node, we put the message in a queue till the mobile node requests for it
						pkt->setDestAddr(pkt->getRealDestAddr());
						pkt->setSrcAddr(myNetwAddr);
						pkt->setRetransmisionCounterBO(0);	// Reset the retransmission counter BackOff
						pkt->setRetransmisionCounterACK(0);	// Reset the retransmission counter ACK
						// The packet is probably for a mobile node, answer from the computer, we assign the dest. addr the mobile node addr and transmit it directly
						EV << "We received a packet for a mobile node, we have to save it until it is requested" << endl;
						// ------------------------------------------------------------------------------------------------------------------------
						// - Here we would not delete the message, we would put it into a queue to have it there when the Mobile Node asks for it -
						// ------------------------------------------------------------------------------------------------------------------------

						if((phase == AppLayer::COM_SINK_PHASE_2) && (regPck[pkt->getCreatedIn()*10000 + pkt->getId()] != 3)) {
							regPck[pkt->getCreatedIn()*10000 + pkt->getId()] = 3;
							if(pkt->getIsAnswer())
								requestOK++;

						}
					}
					delete msg;
				} else { // If the message is not for us, we just route the packet

					numPckToSent++;

					pkt->setRetransmisionCounterBO(0);	// Reset the retransmission counter BackOff
					pkt->setRetransmisionCounterACK(0);	// Reset the retransmission counter ACK
					EV << "Origen: " << pkt->getSrcAddr() << endl;
					EV << "Origen real: " << pkt->getRealSrcAddr() << endl;
					EV << "Destino: " << pkt->getDestAddr() << endl;
					EV << "Destino real: " << pkt->getRealDestAddr() << endl;
					EV << "Tipo: " << pkt->getKind() << endl;
					EV << "Nombre: " << pkt->getName() << endl;
					EV << "CSMA: " << pkt->getCSMA() << endl;

					if(phase == AppLayer::COM_SINK_PHASE_1) { // If we are on COM_SINK_1, priority applies
						if((packetsQueue.getLength() != 0) && packetsQueue.firstHasPriority(packetsQueue.get(0), pkt)) { 	// If queue is not empty and received
							ApplPkt* buffer = check_and_cast<ApplPkt*>((cMessage *)packetsQueue.pop());						// packet has less priority than
							packetsQueue.insertElem(pkt, true);																// than the first in queue, it is
							macDeviceFree = false;																			// enqueued after the first in the queue
							transfersQueue.insert(buffer->dup());															// is routed
							if(buffer->getWasBroadcast())
								broadSent[buffer->getPriority()]++;
							else if(buffer->getWasReport())
								reportSent[buffer->getPriority()]++;
							else if(buffer->getWasRequest())
								requestSent++;
							sendDown(buffer);
						} else {																							// Else, the received packet is directly
							macDeviceFree = false;																			// routed
							transfersQueue.insert(pkt->dup());
							if(pkt->getWasBroadcast())
								broadSent[pkt->getPriority()]++;
							else if(pkt->getWasReport())
								reportSent[pkt->getPriority()]++;
							else if(pkt->getWasRequest())
								requestSent++;
							sendDown(pkt);
						}
					} else { // There is no priority; message directly sent to MAC
						macDeviceFree = false;
						transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
						if(pkt->getWasBroadcast())
							broadSent[pkt->getPriority()]++;
						else if(pkt->getWasReport())
							reportSent[pkt->getPriority()]++;
						else if(pkt->getWasRequest())
							requestSent++;
						sendDown(pkt);
					}
					break;
				}
	    	}
			break;
	    case AppLayer::SYNC_MESSAGE_WITHOUT_CSMA:
	    case AppLayer::SYNC_MESSAGE_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
				EV << "Discarding the packet, in Com Sink Phases Anchor cannot receive any Broadcast from a Mobile Node" << endl;
	    	} else { // Computer or AN
	    		EV << "Discarding the packet, in Com Sink Phases Anchor cannot receive any Broadcast from an Anchor" << endl;
	    	}
			delete msg;
	    	break;
	    default:
			EV << "Unknown Packet! -> delete, kind: " << msg->getKind() << endl;
			delete msg;
			break;
	    }
		break;
	default:
		EV << "Unknown Phase! -> deleting packet, kind: " << msg->getKind() << endl;
		delete msg;
		break;
	}
	delete cInfo;
}


void AnchorAppLayer::handleLowerControl(cMessage *msg)
{
	ApplPkt* pkt;
	switch (msg->getKind())
	{
	case BaseMacLayer::PACKET_DROPPED_BACKOFF: // In case its dropped due to maximum BackOffs periods reached
		// Take the first message from the transmission queue, the first is always the one the MAC is referring to...
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
		nbPacketDroppedBackOff++;
		// Will check if we already tried the maximum number of tries and if not increase the number of retransmission in the packet variable
		EV << "Packet was dropped because it reached maximum BackOff periods, ";
		if (pkt->getRetransmisionCounterBO() < maxRetransDroppedBackOff) {
			pkt->setRetransmisionCounterBO(pkt->getRetransmisionCounterBO() + 1);
			EV << " retransmission number " << pkt->getRetransmisionCounterBO() << " of " << maxRetransDroppedBackOff;
			transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
			sendDown(pkt);
		} else { // We reached the maximum number of retransmissions
			EV << " maximum number of retransmission reached, dropping the packet in App Layer.";
			nbErasedPacketsBackOffMax++;

			if(regPck[pkt->getCreatedIn()*10000 + pkt->getId()] == 0) {
				regPck[pkt->getCreatedIn()*10000 + pkt->getId()] = 2;
				if(pkt->getWasBroadcast())
					broadDrop[pkt->getPriority()]++;
				else if(pkt->getWasReport())
					reportDrop[pkt->getPriority()]++;
				else if(pkt->getWasRequest())
					requestDrop++;
			}

			delete pkt;
			macDeviceFree = true;
		}
		EV << endl;
		break;
	case BaseMacLayer::PACKET_DROPPED: // In case its dropped due to no ACK received...
		// Take the first message from the transmission queue, the first is always the one the MAC is referring to...
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
		nbPacketDroppedNoACK++;
		// Will check if we already tried the maximum number of tries and if not increase the number of retransmission in the packet variable
		EV << "Packet was dropped because it reached maximum tries of transmission in MAC without ACK, ";
		if (pkt->getRetransmisionCounterACK() < maxRetransDroppedReportAN) {
			pkt->setRetransmisionCounterACK(pkt->getRetransmisionCounterACK() + 1);
			EV << " retransmission number " << pkt->getRetransmisionCounterACK() << " of " << maxRetransDroppedReportAN;
			transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
			sendDown(pkt);
		} else { // We reached the maximum number of retransmissions
			EV << " maximum number of retransmission reached, dropping the packet in App Layer.";
			nbErasedPacketsNoACKMax++;

			if(regPck[pkt->getCreatedIn()*10000 + pkt->getId()] == 0) {
				regPck[pkt->getCreatedIn()*10000 + pkt->getId()] = 2;
				if(pkt->getWasBroadcast())
					broadNoAck[pkt->getPriority()]++;
				else if(pkt->getWasReport())
					reportNoAck[pkt->getPriority()]++;
				else if(pkt->getWasRequest())
					requestNoAck++;
			}

			delete pkt;
			macDeviceFree = true;
		}
		EV << endl;
		break;
	case BaseMacLayer::QUEUE_FULL:
		// Take the last message from the transmission queue, the last because as the mac queue is full the Mac queue never had this packet
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.remove(transfersQueue.back()));
		EV << "The queue in MAC is full, discarding the message" << endl;
		nbPacketDroppedMacQueueFull++;
		nbErasedPacketsMacQueueFull++;
		delete pkt;
		break;
	case BaseMacLayer::SYNC_SENT:
		// Take the first message from the transmission queue, the first is always the one the MAC is referring to...
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
		EV << "The Broadcast packet was successfully transmitted into the air" << endl;
		nbBroadcastPacketsSent++;
		macDeviceFree = true;
		delete pkt;
		break;
	case BaseMacLayer::TX_OVER:
		// Take the first message from the transmission queue, the first is always the one the MAC is referring to...
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
		EV << "Message correctly transmitted, received the ACK." << endl;
		nbReportsWithACK++;

		if(pkt->getWasBroadcast())
			broadSentOK[pkt->getPriority()]++;
		else if(pkt->getWasReport())
			reportSentOK[pkt->getPriority()]++;
		else if(pkt->getWasRequest())
			requestSentOK++;

		numPckSentOk++;

		macDeviceFree = true; // TX ended, MAC can be set as free
		delete pkt;
		break;
	case BaseMacLayer::ACK_SENT:
		EV << "ACK correctly sent" << endl;
		break;
	}
	delete msg;
}

void AnchorAppLayer::sendBroadcast()
{
	// It doesn't matter if we have slotted version or not, CSMA must be disabled, we control random time and retransmission in Appl layer
	ApplPkt *pkt = new ApplPkt("SYNC_MESSAGE_WITHOUT_CSMA", SYNC_MESSAGE_WITHOUT_CSMA);
	pkt->setBitLength(syncPacketLength);
	//pkt->setBitLength(88);

	pkt->setSrcAddr(myNetwAddr);
	pkt->setRealSrcAddr(myNetwAddr);
	pkt->setDestAddr(destination);
	pkt->setRealDestAddr(destination);
	pkt->setRetransmisionCounterBO(0);	// Reset the retransmission counter BackOff
	pkt->setRetransmisionCounterACK(0);	// Reset the retransmission counter ACK
	pkt->setCSMA(false);
	pkt->setBroadcastedSuccess(successToTx);

	EV << "Inserting Broadcast Packet in Transmission Queue" << endl;
	transfersQueue.insert(pkt->dup()); // Make a copy of the sent packet till the MAC says it's ok or to retransmit it when something fails
	sendDown(pkt);
}

void AnchorAppLayer::createScheduleEvents()
{
	// IMPORTANT!
	// Calcular guardTimeComSinkPhase per un sol paquet en el pitjor cas! Ja no cal tenir en compte els paquets que queden dins de la cua de MAC perqu� nom�s
	// hi ha un paquet a la cua de MAC.

	simtime_t minSlot = 0.0016;
//	simtime_t minSlot = 0.025;
	simtime_t maxWindow = hops * 0.0016;
	simtime_t referenceTime = max(simTime(), randomQueueTime[queueElementCounter]); // The time to use to calculate the new events cannot be earlier than an already scheduled event
	if ((nextPhaseStartTime - referenceTime - guardTimeComSinkPhase - maxWindow) > (minSlot * packetsQueue.length())) {
		stepTimeComSink1 = (nextPhaseStartTime - referenceTime - guardTimeComSinkPhase - maxWindow) / packetsQueue.length();
	}
	else
		stepTimeComSink1 = minSlot;
	EV << "Transmitting the " << packetsQueue.length() << " elements of the queue in the following moments." << endl;
	for (int i = 0; i < packetsQueue.length(); i++) {
		randomQueueTime[i] = referenceTime + (i * stepTimeComSink1) + uniform(0, stepTimeComSink1, 0);
		EV << "Time " << i << ": " << randomQueueTime[i] << endl;
	}
	queueElementCounter = 0; // Reset the index to know which random time from vector to use
	EV << "Still " << packetsQueue.length() << " elements in the Queue." << endl;
	EV << "Random Transmission number " << queueElementCounter + 1 << " at : " << randomQueueTime[queueElementCounter] << " s" << endl;
}
