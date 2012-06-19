#include "ComputerAppLayer.h"

Define_Module(ComputerAppLayer);

void ComputerAppLayer::initialize(int stage)
{
	AppLayer::initialize(stage);

	if (stage == 1) {
		// Assign the type of host to 3 (computer)
		computer = cc->findNic(myNetwAddr);
		computer->moduleType = 3;
	// We have to wait till stage 3 to make this because in case the anchors are randomly situated, it's done in stage 2
	} else if (stage == 3) { // Slot calculation
		BaseConnectionManager::NicEntries& nicList = cc->getNicList();
		EV << "Number of AN: " << numberOfAnchors << endl;
		int j = 0;
		double distance;
		NicEntry* anchors[numberOfAnchors]; // Array to store pointers to all the anchors
		int matrix[numberOfAnchors][numberOfAnchors]; memset(matrix, 0, sizeof(int)*numberOfAnchors*numberOfAnchors); // Distance matrix between all Anchors 1, 2, 3, ... according to the maxinterferencedistance
		int slotCounter[numberOfAnchors];  memset(slotCounter, 0, sizeof(int)*numberOfAnchors); // Counts the number of elements in each slot
		int slots[numberOfAnchors][numberOfAnchors]; memset(slots, 0, sizeof(int)*numberOfAnchors*numberOfAnchors); // Stores the AN numbers in every slot and slot position
		int queue[numberOfAnchors]; memset(queue, 0, sizeof(int)*numberOfAnchors); // Variable to store the compatible Anchors for this slot but already have an slot, to analyze them at the end of the row
		int numerTimesQueue[numberOfAnchors]; memset(numerTimesQueue, 0, sizeof(int)*numberOfAnchors); // The number of times an element in this queue position already appeared, to order them and give more priority the ones that appeared the less
		int queueCounter = 0;
		int hasSlots;
		numTotalSlots = 0;
		bool lookRow;
		bool compatible;

//		EV << "   0  1  2  3  4  5  6  7  8  9  10 11 " << endl;
		EV << "Coverage " << cc->getMaxInterferenceDistance() << endl; // Maximal interference distance
		// First here we look for the Anchors and discard computer an MN, and put them in our array
		for(BaseConnectionManager::NicEntries::iterator i = nicList.begin(); i != nicList.end(); ++i) {
			if (i->second->moduleType == 1) {
				anchors[j] = i->second;
				anchors[j]->numSlots = 0;
				j++;
			}
		}
		// Cover the anchors array
		for (j = 0; j <= numberOfAnchors-1; j++) {

//		EV << j << " Anchor " << anchors[j]->nicId << " type " << anchors[j]->moduleType;
//		EV << "PosNic despues (" << anchors[j]->pos.getX() << ", " << anchors[j]->pos.getY() << ")" << endl;
//		if (j<10)
//			EV << j << "  ";
//		else
//			EV << j << " ";

			lookRow = false;
			queueCounter = 0;
			// If this anchor doesn't have an slot, we create a new one for it, if it's already assigned, we skip the row
			if (hasSlot(*slots, slotCounter, numTotalSlots, j, numberOfAnchors) == 0)
			{
//				EV << "Añadiendo nuevo Slot " << numTotalSlots << " con el anchor " << j << endl;
				numTotalSlots++; // Incremente the number of Slots
				slotCounter[numTotalSlots-1]++; // Increments the number of elements in this Slot
				slots[numTotalSlots-1][slotCounter[numTotalSlots-1]-1] = j; // Assign this slot and slot position the number of this AN (j variable)
				lookRow = true;
			}

			for (int k = 0; k <= numberOfAnchors-1; k++)
			{
				// This "if" takes out the 1, 2, 3 ... distance form the real value
				if (matrix[j][k] == 0)
				{
					distance = sqrt(pow(anchors[k]->pos.x - anchors[j]->pos.x,2) + pow(anchors[k]->pos.y - anchors[j]->pos.y,2));
					matrix[j][k] = int((distance / cc->getMaxInterferenceDistance()) + 1);
					matrix[k][j] = matrix[j][k];
				}
        		EV << matrix[j][k] << "  ";
				// If distance >= 3 means we don't have hidden terminal problem
				// "lookrow" indicates that this element wasn't in any slot so we have to check his row
				if ((matrix[j][k] >= 3) && lookRow)
				{
					// This "for" checks if the possible new element in the slot is compatible with all the elements that are already in
					compatible = true;
					for (int l = 0; l < slotCounter[numTotalSlots-1]; l++)
					{
						if (matrix[slots[numTotalSlots-1][l]][k] == 0)
						{
							distance = sqrt(pow(anchors[k]->pos.x - anchors[slots[numTotalSlots-1][l]]->pos.x,2) + pow(anchors[k]->pos.y - anchors[slots[numTotalSlots-1][l]]->pos.y,2));
							matrix[slots[numTotalSlots-1][l]][k] = int((distance / cc->getMaxInterferenceDistance()) + 1);
							matrix[k][slots[numTotalSlots-1][l]] = matrix[slots[numTotalSlots-1][l]][k];
						}
						if (matrix[slots[numTotalSlots-1][l]][k] <= 2)
							compatible = false;
					}
					if (compatible) // If the new possible element is compatible with all the existing elements, goes inside the slot
					{
						hasSlots = hasSlot(*slots, slotCounter, numTotalSlots, k, numberOfAnchors);
						// If this new element hadn't already an slot its directly added to this slot
						if (hasSlots == 0)
						{
//        					EV << "Adding new anchor " << k << " to the slot " << numTotalSlots-1 << endl;
							slotCounter[numTotalSlots-1]++;
							slots[numTotalSlots-1][slotCounter[numTotalSlots-1]-1] = k;
						}
						else // If it had an slot, goes to a queue to check it later and give more opportunities to the rest of elements of the row
						{
							queue[queueCounter] = k;
							numerTimesQueue[queueCounter] = hasSlots;
							queueCounter++;
						}
					}
				}
			}
			EV << "\n";
			orderQueue(queue, numerTimesQueue, queueCounter); // Orders the queue in ascendent order according to the number of assigned slots
			// This "for" checks the queue to assign extra slots to the nodes that are still compatibles
			for (int k = 0; k < queueCounter; k++)
			{
				// This "for" checks if the possible new element is compatible with the rest of the elements of the row
				compatible = true;
				for (int l = 0; l < slotCounter[numTotalSlots-1]; l++)
				{
					if (matrix[slots[numTotalSlots-1][l]][queue[k]] == 0)
					{
						distance = sqrt(pow(anchors[queue[k]]->pos.x - anchors[slots[numTotalSlots-1][l]]->pos.x,2) + pow(anchors[queue[k]]->pos.y - anchors[slots[numTotalSlots-1][l]]->pos.y,2));
						matrix[slots[numTotalSlots-1][l]][queue[k]] = int((distance / cc->getMaxInterferenceDistance()) + 1);
						matrix[queue[k]][slots[numTotalSlots-1][l]] = matrix[slots[numTotalSlots-1][l]][queue[k]];
					}
					if (matrix[slots[numTotalSlots-1][l]][queue[k]] <= 2)
						compatible = false;
				}
				if (compatible) // If its compatible, then we add it to the slot
				{
//       				EV << "Reusing anchor " << queue[k] << " in the slot " << numTotalSlots-1 << endl;
					slotCounter[numTotalSlots-1]++;
					slots[numTotalSlots-1][slotCounter[numTotalSlots-1]-1] = queue[k];
				}
			}
//        	EV << endl;
		}

		// This for prints the SLot configuration and assigns slot number values
		// -----------------------------------------------------------------------------------
		// - To have a more real system, here the computer would send packets to the Anchors -
		// - with the slot configuration, we just configured the directly --------------------
		// -----------------------------------------------------------------------------------
		for (j = 0; j < numTotalSlots; j++)
		{
			EV << "Slot " << j << ": ";
			for (int k = 0; k < slotCounter[j]; k++)
			{
				anchors[slots[j][k]]->transmisionSlot[anchors[slots[j][k]]->numSlots] = j;
				anchors[slots[j][k]]->numSlots = anchors[slots[j][k]]->numSlots +1;
				anchors[slots[j][k]]->numTotalSlots = numTotalSlots;
				EV << "(" << anchors[slots[j][k]]->numSlots << ")";
				EV << slots[j][k] << ",";
			}
			EV << endl;
		}
		computer->numTotalSlots = numTotalSlots; // Asign in the computer NIC the total number of slots to have it accessible from other hosts

		// This for prints the Slots in which every Anchor transmits (sometimes they could have more than one)
		j= 0;
		for(BaseConnectionManager::NicEntries::iterator i = nicList.begin(); i != nicList.end(); ++i)
		{
			if (i->second->moduleType == 1) {
				EV << "Nic (" << j << ")" << i->second->nicId << " transmits in Slot: ";
				for (int k = 0; k < i->second->numSlots; k++)
				{
					EV << i->second->transmisionSlot[k] << ", ";
				}
				EV << endl;
				j++;
			}
		}
	} else if (stage == 4) {
		// Just prints the time on the screen to see them when we launch the simulation
		EV << "Number of Slots: " << numTotalSlots << endl;
		EV << "T Sync: " << timeSyncPhase << endl;
		EV << "T Report: " << timeReportPhase << endl;
		EV << "T VIP: " << timeVIPPhase << endl;
		EV << "T Com_Sink: " << timeComSinkPhase << endl;

		// Necessary variables for the queue initialization
		checkQueue = new cMessage("transmit queue elements", CHECK_QUEUE);
		queueElementCounter = 0;
		maxQueueElements = 200;

		packetsQueue.setMaxLength(maxQueueElements); // Sets the lenght of the queue

		hops = (int*)calloc(sizeof(int), 25);
		// Defines the amount of hops between the coordinator and an anchor with id = anchNum
		for (int anchNum = 0; anchNum < numberOfAnchors; anchNum++) {
			if((anchNum == 0) || (anchNum == 5) || (anchNum == 10) || (anchNum == 15) || (anchNum == 20) || (anchNum == 21) || (anchNum == 22) ||
				(anchNum == 23) || (anchNum == 24))
				hops[anchNum] = 4;
			if((anchNum == 1) || (anchNum == 6) || (anchNum == 11) || (anchNum == 16) || (anchNum == 17) || (anchNum == 18) || (anchNum == 19))
				hops[anchNum] = 3;
			if((anchNum == 4) || (anchNum == 2) || (anchNum == 7) || (anchNum == 12) || (anchNum == 13) || (anchNum == 14))
				hops[anchNum] = 2;
			if((anchNum == 3) || (anchNum == 8) || (anchNum == 9))
				hops[anchNum] = 1;
		}

		fromNode = (int*)calloc(sizeof(int), numberOfNodes);
		memset(fromNode, 0, sizeof(int)*numberOfNodes);

		receivedId = (bool*)calloc(sizeof(bool), numberOfAnchors*10000);
		for(int i = 0; i < numberOfAnchors*10000; i++)
			receivedId[i] = false;

		broadOK = (int*)calloc(sizeof(int), 5);
		reportOK = (int*)calloc(sizeof(int), 5);
		memset(broadOK, 0, sizeof(int)*5);
		memset(reportOK, 0, sizeof(int)*5);

		requestOK = 0;

		regPck = (int*)calloc(sizeof(int), numberOfAnchors*10000);
		memset(regPck, 0, sizeof(int)*(numberOfAnchors)*10000);

		meanBroad1 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanBroad1, 0, sizeof(double)*(numberOfAnchors));
		meanBroad2 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanBroad2, 0, sizeof(double)*(numberOfAnchors));
		meanBroad3 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanBroad3, 0, sizeof(double)*(numberOfAnchors));
		meanBroad4 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanBroad4, 0, sizeof(double)*(numberOfAnchors));
		meanReport1 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanReport1, 0, sizeof(double)*(numberOfAnchors));
		meanReport2 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanReport2, 0, sizeof(double)*(numberOfAnchors));
		meanReport3 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanReport3, 0, sizeof(double)*(numberOfAnchors));
		meanReport4 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanReport4, 0, sizeof(double)*(numberOfAnchors));
		meanRequest = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanRequest, 0, sizeof(double)*(numberOfAnchors));

		maxBroad1 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxBroad1, 0, sizeof(double)*(numberOfAnchors));
		maxBroad2 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxBroad2, 0, sizeof(double)*(numberOfAnchors));
		maxBroad3 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxBroad3, 0, sizeof(double)*(numberOfAnchors));
		maxBroad4 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxBroad4, 0, sizeof(double)*(numberOfAnchors));
		maxReport1 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxReport1, 0, sizeof(double)*(numberOfAnchors));
		maxReport2 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxReport2, 0, sizeof(double)*(numberOfAnchors));
		maxReport3 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxReport3, 0, sizeof(double)*(numberOfAnchors));
		maxReport4 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxReport4, 0, sizeof(double)*(numberOfAnchors));
		maxRequest = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxRequest, 0, sizeof(double)*(numberOfAnchors));

		minBroad1 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minBroad1, 0, sizeof(double)*(numberOfAnchors));
		minBroad2 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minBroad2, 0, sizeof(double)*(numberOfAnchors));
		minBroad3 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minBroad3, 0, sizeof(double)*(numberOfAnchors));
		minBroad4 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minBroad4, 0, sizeof(double)*(numberOfAnchors));
		minReport1 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minReport1, 0, sizeof(double)*(numberOfAnchors));
		minReport2 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minReport2, 0, sizeof(double)*(numberOfAnchors));
		minReport3 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minReport3, 0, sizeof(double)*(numberOfAnchors));
		minReport4 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minReport4, 0, sizeof(double)*(numberOfAnchors));
		minRequest = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minRequest, 0, sizeof(double)*(numberOfAnchors));

		meanDelayBroad1 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanDelayBroad1, 0, sizeof(double)*(numberOfAnchors));
		meanDelayBroad2 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanDelayBroad2, 0, sizeof(double)*(numberOfAnchors));
		meanDelayBroad3 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanDelayBroad3, 0, sizeof(double)*(numberOfAnchors));
		meanDelayBroad4 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanDelayBroad4, 0, sizeof(double)*(numberOfAnchors));
		meanDelayReport1 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanDelayReport1, 0, sizeof(double)*(numberOfAnchors));
		meanDelayReport2 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanDelayReport2, 0, sizeof(double)*(numberOfAnchors));
		meanDelayReport3 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanDelayReport3, 0, sizeof(double)*(numberOfAnchors));
		meanDelayReport4 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanDelayReport4, 0, sizeof(double)*(numberOfAnchors));
		meanDelayRequest = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(meanDelayRequest, 0, sizeof(double)*(numberOfAnchors));

		maxDelayBroad1 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxDelayBroad1, 0, sizeof(double)*(numberOfAnchors));
		maxDelayBroad2 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxDelayBroad2, 0, sizeof(double)*(numberOfAnchors));
		maxDelayBroad3 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxDelayBroad3, 0, sizeof(double)*(numberOfAnchors));
		maxDelayBroad4 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxDelayBroad4, 0, sizeof(double)*(numberOfAnchors));
		maxDelayReport1 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxDelayReport1, 0, sizeof(double)*(numberOfAnchors));
		maxDelayReport2 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxDelayReport2, 0, sizeof(double)*(numberOfAnchors));
		maxDelayReport3 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxDelayReport3, 0, sizeof(double)*(numberOfAnchors));
		maxDelayReport4 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxDelayReport4, 0, sizeof(double)*(numberOfAnchors));
		maxDelayRequest = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(maxDelayRequest, 0, sizeof(double)*(numberOfAnchors));

		minDelayBroad1 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minDelayBroad1, 0, sizeof(double)*(numberOfAnchors));
		minDelayBroad2 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minDelayBroad2, 0, sizeof(double)*(numberOfAnchors));
		minDelayBroad3 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minDelayBroad3, 0, sizeof(double)*(numberOfAnchors));
		minDelayBroad4 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minDelayBroad4, 0, sizeof(double)*(numberOfAnchors));
		minDelayReport1 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minDelayReport1, 0, sizeof(double)*(numberOfAnchors));
		minDelayReport2 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minDelayReport2, 0, sizeof(double)*(numberOfAnchors));
		minDelayReport3 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minDelayReport3, 0, sizeof(double)*(numberOfAnchors));
		minDelayReport4 = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minDelayReport4, 0, sizeof(double)*(numberOfAnchors));
		minDelayRequest = (double*)calloc(sizeof(double), numberOfAnchors);
		memset(minDelayRequest, 0, sizeof(double)*(numberOfAnchors));

		timeBroad1 = (double*)calloc(sizeof(double), numberOfAnchors*4000);
		memset(timeBroad1, 0, sizeof(double)*(numberOfAnchors)*4000);
		timeBroad2 = (double*)calloc(sizeof(double), numberOfAnchors*4000);
		memset(timeBroad2, 0, sizeof(double)*(numberOfAnchors)*4000);
		timeBroad3 = (double*)calloc(sizeof(double), numberOfAnchors*4000);
		memset(timeBroad3, 0, sizeof(double)*(numberOfAnchors)*4000);
		timeBroad4 = (double*)calloc(sizeof(double), numberOfAnchors*4000);
		memset(timeBroad4, 0, sizeof(double)*(numberOfAnchors)*4000);
		timeReport1 = (double*)calloc(sizeof(double), numberOfAnchors*700);
		memset(timeReport1, 0, sizeof(double)*(numberOfAnchors)*700);
		timeReport2 = (double*)calloc(sizeof(double), numberOfAnchors*700);
		memset(timeReport2, 0, sizeof(double)*(numberOfAnchors)*700);
		timeReport3 = (double*)calloc(sizeof(double), numberOfAnchors*700);
		memset(timeReport3, 0, sizeof(double)*(numberOfAnchors)*700);
		timeReport4 = (double*)calloc(sizeof(double), numberOfAnchors*700);
		memset(timeReport4, 0, sizeof(double)*(numberOfAnchors)*700);
		timeRequest = (double*)calloc(sizeof(double), numberOfAnchors*200);
		memset(timeRequest, 0, sizeof(double)*(numberOfAnchors)*200);

		delayBroad1 = (double*)calloc(sizeof(double), numberOfAnchors*4000);
		memset(delayBroad1, 0, sizeof(double)*(numberOfAnchors)*4000);
		delayBroad2 = (double*)calloc(sizeof(double), numberOfAnchors*4000);
		memset(delayBroad2, 0, sizeof(double)*(numberOfAnchors)*4000);
		delayBroad3 = (double*)calloc(sizeof(double), numberOfAnchors*4000);
		memset(delayBroad3, 0, sizeof(double)*(numberOfAnchors)*4000);
		delayBroad4 = (double*)calloc(sizeof(double), numberOfAnchors*4000);
		memset(delayBroad4, 0, sizeof(double)*(numberOfAnchors)*4000);
		delayReport1 = (double*)calloc(sizeof(double), numberOfAnchors*700);
		memset(delayReport1, 0, sizeof(double)*(numberOfAnchors)*700);
		delayReport2 = (double*)calloc(sizeof(double), numberOfAnchors*700);
		memset(delayReport2, 0, sizeof(double)*(numberOfAnchors)*700);
		delayReport3 = (double*)calloc(sizeof(double), numberOfAnchors*700);
		memset(delayReport3, 0, sizeof(double)*(numberOfAnchors)*700);
		delayReport4 = (double*)calloc(sizeof(double), numberOfAnchors*700);
		memset(delayReport4, 0, sizeof(double)*(numberOfAnchors)*700);
		delayRequest = (double*)calloc(sizeof(double), numberOfAnchors*200);
		memset(delayRequest, 0, sizeof(double)*(numberOfAnchors)*200);
	}
}

ComputerAppLayer::~ComputerAppLayer() {
	cancelAndDelete(checkQueue);
	cancelAndDelete(beginPhases);
	for(cQueue::Iterator iter(packetsQueue, 1); !iter.end(); iter++) {
		delete(iter());
	}
	for(cQueue::Iterator iter(transfersQueue, 1); !iter.end(); iter++) {
		delete(iter());
	}
}

void ComputerAppLayer::finish()
{
	recordScalar("Dropped Packets in Comp - No ACK received", nbPacketDroppedNoACK);
	recordScalar("Dropped Packets in Comp - Max MAC BackOff tries", nbPacketDroppedBackOff);
	recordScalar("Dropped Packets in Comp - App Queue Full", nbPacketDroppedAppQueueFull);
	recordScalar("Dropped Packets in Comp - Mac Queue Full", nbPacketDroppedMacQueueFull);
	recordScalar("Dropped Packets in Comp - No Time in the Phase", nbPacketDroppedNoTimeApp);
	recordScalar("Erased Packets in Comp - No more BackOff retries", nbErasedPacketsBackOffMax);
	recordScalar("Erased Packets in Comp - No more No ACK retries", nbErasedPacketsNoACKMax);
	recordScalar("Erased Packets in Comp - No more MAC Queue Full retries", nbErasedPacketsMacQueueFull);
	recordScalar("Number of Comp Reports with ACK", nbReportsWithACK);
	recordScalar("Number of Reports received in Comp", nbReportsReceived);
	recordScalar("Number of Reports really for me received in Comp", nbReportsForMeReceived);

	recordScalar("Number of packets 1 created by a mobile broadcast TX OK", broadOK[1]);
	recordScalar("Number of packets 1 created by a mobile report TX OK", reportOK[1]);

	recordScalar("Number of packets 2 created by a mobile broadcast TX OK", broadOK[2]);
	recordScalar("Number of packets 2 created by a mobile report TX OK", reportOK[2]);

	recordScalar("Number of packets 3 created by a mobile broadcast TX OK", broadOK[3]);
	recordScalar("Number of packets 3 created by a mobile report TX OK", reportOK[3]);

	recordScalar("Number of packets 4 created by a mobile broadcast TX OK", broadOK[4]);
	recordScalar("Number of packets 4 created by a mobile report TX OK", reportOK[4]);

	recordScalar("Number of packets created by a mobile request TX OK", requestOK);

	for(int i=0; i<numberOfAnchors; i++) {
		int n1,n2,n3,n4,n5,n6,n7,n8,n9;
		n1=n2=n3=n4=n5=n6=n7=n8=n9=0;

		int m1,m2,m3,m4,m5,m6,m7,m8,m9;
		m1=m2=m3=m4=m5=m6=m7=m8=m9=0;

		maxBroad1[i] = timeBroad1[4000*i];
		maxBroad2[i] = timeBroad2[4000*i];
		maxBroad3[i] = timeBroad3[4000*i];
		maxBroad4[i] = timeBroad4[4000*i];
		maxReport1[i] = timeReport1[700*i];
		maxReport2[i] = timeReport2[700*i];
		maxReport3[i] = timeReport3[700*i];
		maxReport4[i] = timeReport4[700*i];
		maxRequest[i] = timeRequest[200*i];

		maxDelayBroad1[i] = delayBroad1[4000*i];
		maxDelayBroad2[i] = delayBroad2[4000*i];
		maxDelayBroad3[i] = delayBroad3[4000*i];
		maxDelayBroad4[i] = delayBroad4[4000*i];
		maxDelayReport1[i] = delayReport1[700*i];
		maxDelayReport2[i] = delayReport2[700*i];
		maxDelayReport3[i] = delayReport3[700*i];
		maxDelayReport4[i] = delayReport4[700*i];
		maxDelayRequest[i] = delayRequest[200*i];

		minBroad1[i] = timeBroad1[4000*i];
		minBroad2[i] = timeBroad2[4000*i];
		minBroad3[i] = timeBroad3[4000*i];
		minBroad4[i] = timeBroad4[4000*i];
		minReport1[i] = timeReport1[700*i];
		minReport2[i] = timeReport2[700*i];
		minReport3[i] = timeReport3[700*i];
		minReport4[i] = timeReport4[700*i];
		minRequest[i] = timeRequest[200*i];

		minDelayBroad1[i] = delayBroad1[4000*i];
		minDelayBroad2[i] = delayBroad2[4000*i];
		minDelayBroad3[i] = delayBroad3[4000*i];
		minDelayBroad4[i] = delayBroad4[4000*i];
		minDelayReport1[i] = delayReport1[700*i];
		minDelayReport2[i] = delayReport2[700*i];
		minDelayReport3[i] = delayReport3[700*i];
		minDelayReport4[i] = delayReport4[700*i];
		minDelayRequest[i] = delayRequest[200*i];

		for(int j=0; j<4000; j++) {
			if(timeBroad1[4000*i + j] != 0) {meanBroad1[i] = meanBroad1[i] + timeBroad1[4000*i + j]; n1++;}
			if(timeBroad2[4000*i + j] != 0) {meanBroad2[i] = meanBroad2[i] + timeBroad2[4000*i + j]; n2++;}
			if(timeBroad3[4000*i + j] != 0) {meanBroad3[i] = meanBroad3[i] + timeBroad3[4000*i + j]; n3++;}
			if(timeBroad4[4000*i + j] != 0) {meanBroad4[i] = meanBroad4[i] + timeBroad4[4000*i + j]; n4++;}

			if(timeBroad1[4000*i + j] > maxBroad1[i]) maxBroad1[i] = timeBroad1[4000*i + j];
			if(timeBroad2[4000*i + j] > maxBroad2[i]) maxBroad2[i] = timeBroad2[4000*i + j];
			if(timeBroad3[4000*i + j] > maxBroad3[i]) maxBroad3[i] = timeBroad3[4000*i + j];
			if(timeBroad4[4000*i + j] > maxBroad4[i]) maxBroad4[i] = timeBroad4[4000*i + j];

			if(timeBroad1[4000*i + j] < minBroad1[i]) minBroad1[i] = timeBroad1[4000*i + j];
			if(timeBroad2[4000*i + j] < minBroad2[i]) minBroad2[i] = timeBroad2[4000*i + j];
			if(timeBroad3[4000*i + j] < minBroad3[i]) minBroad3[i] = timeBroad3[4000*i + j];
			if(timeBroad4[4000*i + j] < minBroad4[i]) minBroad4[i] = timeBroad4[4000*i + j];

			if(delayBroad1[4000*i + j] != 0) {meanDelayBroad1[i] = meanDelayBroad1[i] + delayBroad1[4000*i + j]; m1++;}
			if(delayBroad2[4000*i + j] != 0) {meanDelayBroad2[i] = meanDelayBroad2[i] + delayBroad2[4000*i + j]; m2++;}
			if(delayBroad3[4000*i + j] != 0) {meanDelayBroad3[i] = meanDelayBroad3[i] + delayBroad3[4000*i + j]; m3++;}
			if(delayBroad4[4000*i + j] != 0) {meanDelayBroad4[i] = meanDelayBroad4[i] + delayBroad4[4000*i + j]; m4++;}

			if(delayBroad1[4000*i + j] > maxDelayBroad1[i]) maxDelayBroad1[i] = delayBroad1[4000*i + j];
			if(delayBroad2[4000*i + j] > maxDelayBroad2[i]) maxDelayBroad2[i] = delayBroad2[4000*i + j];
			if(delayBroad3[4000*i + j] > maxDelayBroad3[i]) maxDelayBroad3[i] = delayBroad3[4000*i + j];
			if(delayBroad4[4000*i + j] > maxDelayBroad4[i]) maxDelayBroad4[i] = delayBroad4[4000*i + j];

			if(delayBroad1[4000*i + j] < minDelayBroad1[i]) minDelayBroad1[i] = delayBroad1[4000*i + j];
			if(delayBroad2[4000*i + j] < minDelayBroad2[i]) minDelayBroad2[i] = delayBroad2[4000*i + j];
			if(delayBroad3[4000*i + j] < minDelayBroad3[i]) minDelayBroad3[i] = delayBroad3[4000*i + j];
			if(delayBroad4[4000*i + j] < minDelayBroad4[i]) minDelayBroad4[i] = delayBroad4[4000*i + j];
		}

		for(int j=0; j<700; j++) {
			if(timeReport1[700*i + j] != 0) {meanReport1[i] = meanReport1[i] + timeReport1[700*i + j]; n5++;}
			if(timeReport2[700*i + j] != 0) {meanReport2[i] = meanReport2[i] + timeReport2[700*i + j]; n6++;}
			if(timeReport3[700*i + j] != 0) {meanReport3[i] = meanReport3[i] + timeReport3[700*i + j]; n7++;}
			if(timeReport4[700*i + j] != 0) {meanReport4[i] = meanReport4[i] + timeReport4[700*i + j]; n8++;}

			if(timeReport1[700*i + j] > maxReport1[i]) maxReport1[i] = timeReport1[700*i + j];
			if(timeReport2[700*i + j] > maxReport2[i]) maxReport2[i] = timeReport2[700*i + j];
			if(timeReport3[700*i + j] > maxReport3[i]) maxReport3[i] = timeReport3[700*i + j];
			if(timeReport4[700*i + j] > maxReport4[i]) maxReport4[i] = timeReport4[700*i + j];

			if(timeReport1[700*i + j] < minReport1[i]) minReport1[i] = timeReport1[700*i + j];
			if(timeReport2[700*i + j] < minReport2[i]) minReport2[i] = timeReport2[700*i + j];
			if(timeReport3[700*i + j] < minReport3[i]) minReport3[i] = timeReport3[700*i + j];
			if(timeReport4[700*i + j] < minReport4[i]) minReport4[i] = timeReport4[700*i + j];

			if(delayReport1[700*i + j] != 0) {meanDelayReport1[i] = meanDelayReport1[i] + delayReport1[700*i + j]; m5++;}
			if(delayReport2[700*i + j] != 0) {meanDelayReport2[i] = meanDelayReport2[i] + delayReport2[700*i + j]; m6++;}
			if(delayReport3[700*i + j] != 0) {meanDelayReport3[i] = meanDelayReport3[i] + delayReport3[700*i + j]; m7++;}
			if(delayReport4[700*i + j] != 0) {meanDelayReport4[i] = meanDelayReport4[i] + delayReport4[700*i + j]; m8++;}

			if(delayReport1[700*i + j] > maxDelayReport1[i]) maxDelayReport1[i] = delayReport1[700*i + j];
			if(delayReport2[700*i + j] > maxDelayReport2[i]) maxDelayReport2[i] = delayReport2[700*i + j];
			if(delayReport3[700*i + j] > maxDelayReport3[i]) maxDelayReport3[i] = delayReport3[700*i + j];
			if(delayReport4[700*i + j] > maxDelayReport4[i]) maxDelayReport4[i] = delayReport4[700*i + j];

			if(delayReport1[700*i + j] < minDelayReport1[i]) minDelayReport1[i] = delayReport1[700*i + j];
			if(delayReport2[700*i + j] < minDelayReport2[i]) minDelayReport2[i] = delayReport2[700*i + j];
			if(delayReport3[700*i + j] < minDelayReport3[i]) minDelayReport3[i] = delayReport3[700*i + j];
			if(delayReport4[700*i + j] < minDelayReport4[i]) minDelayReport4[i] = delayReport4[700*i + j];
		}

		for(int j=0; j<200; j++) {
			if(timeRequest[200*i + j] != 0) {meanRequest[i] = meanRequest[i] + timeRequest[200*i + j]; n9++;}

			if(timeRequest[200*i + j] > maxRequest[i]) maxRequest[i] = timeRequest[200*i + j];

			if(timeRequest[200*i + j] < minRequest[i]) minRequest[i] = timeRequest[200*i + j];

			if(delayRequest[200*i + j] != 0) {meanDelayRequest[i] = meanDelayRequest[i] + delayRequest[200*i + j]; m9++;}

			if(delayRequest[200*i + j] > maxDelayRequest[i]) maxDelayRequest[i] = delayRequest[200*i + j];

			if(delayRequest[200*i + j] < minDelayRequest[i]) minDelayRequest[i] = delayRequest[200*i + j];
		}

		if(n1 > 0) meanBroad1[i] = meanBroad1[i]/n1;
		if(n2 > 0) meanBroad2[i] = meanBroad2[i]/n2;
		if(n3 > 0) meanBroad3[i] = meanBroad3[i]/n3;
		if(n4 > 0) meanBroad4[i] = meanBroad4[i]/n4;
		if(n5 > 0) meanReport1[i] = meanReport1[i]/n5;
		if(n6 > 0) meanReport2[i] = meanReport2[i]/n6;
		if(n7 > 0) meanReport3[i] = meanReport3[i]/n7;
		if(n8 > 0) meanReport4[i] = meanReport4[i]/n8;
		if(n9 > 0) meanRequest[i] = meanRequest[i]/n9;

		if(m1 > 0) meanDelayBroad1[i] = meanDelayBroad1[i]/m1;
		if(m2 > 0) meanDelayBroad2[i] = meanDelayBroad2[i]/m2;
		if(m3 > 0) meanDelayBroad3[i] = meanDelayBroad3[i]/m3;
		if(m4 > 0) meanDelayBroad4[i] = meanDelayBroad4[i]/m4;
		if(m5 > 0) meanDelayReport1[i] = meanDelayReport1[i]/m5;
		if(m6 > 0) meanDelayReport2[i] = meanDelayReport2[i]/m6;
		if(m7 > 0) meanDelayReport3[i] = meanDelayReport3[i]/m7;
		if(m8 > 0) meanDelayReport4[i] = meanDelayReport4[i]/m8;
		if(m9 > 0) meanDelayRequest[i] = meanDelayRequest[i]/m9;

		char buffer[100] = "";
		sprintf(buffer, "Number of Broadcast 1 from node %d", i);
		recordScalar(buffer, m1);
		sprintf(buffer, "Number of Broadcast 2 from node %d", i);
		recordScalar(buffer, m2);
		sprintf(buffer, "Number of Broadcast 3 from node %d", i);
		recordScalar(buffer, m3);
		sprintf(buffer, "Number of Broadcast 4 from node %d", i);
		recordScalar(buffer, m4);
		sprintf(buffer, "Number of Report 1 from node %d", i);
		recordScalar(buffer, m5);
		sprintf(buffer, "Number of Report 2 from node %d", i);
		recordScalar(buffer, m6);
		sprintf(buffer, "Number of Report 3 from node %d", i);
		recordScalar(buffer, m7);
		sprintf(buffer, "Number of Report 4 from node %d", i);
		recordScalar(buffer, m8);
		sprintf(buffer, "Number of Request from node %d", i);
		recordScalar(buffer, m9);
	}

	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean time for Broadcast 1 from node %d", i);
		recordScalar(buffer, meanBroad1[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean time for Broadcast 2 from node %d", i);
		recordScalar(buffer, meanBroad2[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean time for Broadcast 3 from node %d", i);
		recordScalar(buffer, meanBroad3[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean time for Broadcast 4 from node %d", i);
		recordScalar(buffer, meanBroad4[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean time for Report 1 from node %d", i);
		recordScalar(buffer, meanReport1[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean time for Report 2 from node %d", i);
		recordScalar(buffer, meanReport2[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean time for Report 3 from node %d", i);
		recordScalar(buffer, meanReport3[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean time for Report 4 from node %d", i);
		recordScalar(buffer, meanReport4[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean time for Request from node %d", i);
		recordScalar(buffer, meanRequest[i]);
	}

	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean delay for Broadcast 1 from node %d", i);
		recordScalar(buffer, meanDelayBroad1[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean delay for Broadcast 2 from node %d", i);
		recordScalar(buffer, meanDelayBroad2[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean delay for Broadcast 3 from node %d", i);
		recordScalar(buffer, meanDelayBroad3[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean delay for Broadcast 4 from node %d", i);
		recordScalar(buffer, meanDelayBroad4[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean delay for Report 1 from node %d", i);
		recordScalar(buffer, meanDelayReport1[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean delay for Report 2 from node %d", i);
		recordScalar(buffer, meanDelayReport2[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean delay for Report 3 from node %d", i);
		recordScalar(buffer, meanDelayReport3[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean delay for Report 4 from node %d", i);
		recordScalar(buffer, meanDelayReport4[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Mean delay for Request from node %d", i);
		recordScalar(buffer, meanDelayRequest[i]);
	}

	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum time for Broadcast 1 from node %d", i);
		recordScalar(buffer, maxBroad1[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum time for Broadcast 2 from node %d", i);
		recordScalar(buffer, maxBroad2[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum time for Broadcast 3 from node %d", i);
		recordScalar(buffer, maxBroad3[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum time for Broadcast 4 from node %d", i);
		recordScalar(buffer, maxBroad4[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum time for Report 1 from node %d", i);
		recordScalar(buffer, maxReport1[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum time for Report 2 from node %d", i);
		recordScalar(buffer, maxReport2[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum time for Report 3 from node %d", i);
		recordScalar(buffer, maxReport3[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum time for Report 4 from node %d", i);
		recordScalar(buffer, maxReport4[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum time for Request from node %d", i);
		recordScalar(buffer, maxRequest[i]);
	}

	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum time for Broadcast 1 from node %d", i);
		recordScalar(buffer, minBroad1[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum time for Broadcast 2 from node %d", i);
		recordScalar(buffer, minBroad2[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum time for Broadcast 3 from node %d", i);
		recordScalar(buffer, minBroad3[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum time for Broadcast 4 from node %d", i);
		recordScalar(buffer, minBroad4[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum time for Report 1 from node %d", i);
		recordScalar(buffer, minReport1[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum time for Report 2 from node %d", i);
		recordScalar(buffer, minReport2[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum time for Report 3 from node %d", i);
		recordScalar(buffer, minReport3[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum time for Report 4 from node %d", i);
		recordScalar(buffer, minReport4[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum time for Request from node %d", i);
		recordScalar(buffer, minRequest[i]);
	}

	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum delay for Broadcast 1 from node %d", i);
		recordScalar(buffer, maxDelayBroad1[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum delay for Broadcast 2 from node %d", i);
		recordScalar(buffer, maxDelayBroad2[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum delay for Broadcast 3 from node %d", i);
		recordScalar(buffer, maxDelayBroad3[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum delay for Broadcast 4 from node %d", i);
		recordScalar(buffer, maxDelayBroad4[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum delay for Report 1 from node %d", i);
		recordScalar(buffer, maxDelayReport1[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum delay for Report 2 from node %d", i);
		recordScalar(buffer, maxDelayReport2[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum delay for Report 3 from node %d", i);
		recordScalar(buffer, maxDelayReport3[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum delay for Report 4 from node %d", i);
		recordScalar(buffer, maxDelayReport4[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Maximum delay for Request from node %d", i);
		recordScalar(buffer, maxDelayRequest[i]);
	}

	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum delay for Broadcast 1 from node %d", i);
		recordScalar(buffer, minDelayBroad1[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum delay for Broadcast 2 from node %d", i);
		recordScalar(buffer, minDelayBroad2[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum delay for Broadcast 3 from node %d", i);
		recordScalar(buffer, minDelayBroad3[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum delay for Broadcast 4 from node %d", i);
		recordScalar(buffer, minDelayBroad4[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum delay for Report 1 from node %d", i);
		recordScalar(buffer, minDelayReport1[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum delay for Report 2 from node %d", i);
		recordScalar(buffer, minDelayReport2[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum delay for Report 3 from node %d", i);
		recordScalar(buffer, minDelayReport3[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum delay for Report 4 from node %d", i);
		recordScalar(buffer, minDelayReport4[i]);
	}
	for(int i = 0; i < numberOfAnchors; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Minimum delay for Request from node %d", i);
		recordScalar(buffer, minDelayRequest[i]);
	}

	for(int i = 0; i < numberOfNodes; i++) {
		char buffer[100] = "";
		sprintf(buffer, "Number of packets sent from mobile node %d", i);
		recordScalar(buffer, fromNode[i]);
	}
}

void ComputerAppLayer::handleSelfMsg(cMessage *msg)
{
	switch(msg->getKind())
	{
	case CHECK_QUEUE:
		if (packetsQueue.length() > 0) {
			cMessage * msg = (cMessage *)packetsQueue.pop();
			EV << "Sending packet from the Queue to Anchor with addr. " << static_cast<ApplPkt *>(msg)->getDestAddr() << endl;
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
			transfersQueue.clear();
			// --------------------------------------------------------------------------------------
			// - If we don't want to clear the queue and lose the packets, --------------------------
			// - we could somehow here leave the rest of the queue for the next full phase (period) -
			// --------------------------------------------------------------------------------------
		} else {
			EV << "App Transmission Queue empty in phase change." << endl;
		}
		// Change the phase and prepare the data for the new phase
		switch (nextPhase)
		{
		case AppLayer::SYNC_PHASE_1:
			phase = AppLayer::SYNC_PHASE_1;
			nextPhase = AppLayer::REPORT_PHASE;
			nextPhaseStartTime = simTime() + timeSyncPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
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
			break;
		case AppLayer::COM_SINK_PHASE_1:
			phase = AppLayer::COM_SINK_PHASE_1;
			nextPhase = AppLayer::SYNC_PHASE_3;
			nextPhaseStartTime = simTime() + timeComSinkPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case AppLayer::SYNC_PHASE_3:
			phase = AppLayer::SYNC_PHASE_3;
			nextPhase = AppLayer::COM_SINK_PHASE_2;
			nextPhaseStartTime = simTime() + timeSyncPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			break;
		case AppLayer::COM_SINK_PHASE_2:
			phase = AppLayer::COM_SINK_PHASE_2;
			nextPhase = AppLayer::SYNC_PHASE_1;
			nextPhaseStartTime = simTime() + timeComSinkPhase;
			scheduleAt(nextPhaseStartTime, beginPhases);
			// At the beginning of the Com Sink 2 the Computer checks its queue to transmit the elements and calculate all the random transmission times
			if (packetsQueue.length() > 0) { // Only if the Queue has elements we do calculate all the intermediate times
				stepTimeComSink2 = (timeComSinkPhase - guardTimeComSinkPhase) / packetsQueue.length();
				randomQueueTime = (simtime_t*)calloc(sizeof(simtime_t), packetsQueue.length());
				EV << "Transmitting the " << packetsQueue.length() << " elements of the queue in the following moments." << endl;
				for (int i = 0; i < packetsQueue.length(); i++) {
					randomQueueTime[i] = simTime() + (i * stepTimeComSink2) + uniform(0, stepTimeComSink2, 0);
					EV << "Time " << i << ": " << randomQueueTime[i] << endl;
				}
				queueElementCounter = 0; // Reset the index to know which random time from vector to use
				EV << "Still " << packetsQueue.length() << " elements in the Queue." << endl;
				EV << "Random Transmission number " << queueElementCounter + 1 << " at : " << randomQueueTime[queueElementCounter] << " s" << endl;
				scheduleAt(randomQueueTime[queueElementCounter], checkQueue);
			} else {
				EV << "Queue empty, computer has nothing to communicate this full phase (period)." << endl;
			}
			break;
		}
		break;
	default:
		EV << "Unknown selfmessage! -> delete, kind: " << msg->getKind() << endl;
		delete msg;
		break;
	}
}

void ComputerAppLayer::handleLowerMsg(cMessage *msg)
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
				EV << "Discarding the packet, in Sync Phase computer cannot receive any Report from a Mobile Node" << endl;
	    	} else { // Computer or AN
	    		EV << "Discarding the packet, in Sync Phase computer cannot receive any Report from a Anchor" << endl;
	    	}
			delete msg;
			break;
	    case AppLayer::SYNC_MESSAGE_WITHOUT_CSMA:
	    case AppLayer::SYNC_MESSAGE_WITH_CSMA:
	    	if (host->moduleType == 2) { // Mobile Node
				EV << "Discarding the packet, in Sync Phase computer cannot receive any Broadcast from a Mobile Node" << endl;
	    	} else { // Computer or AN
	    		// --------------------------------------------------------------------------------------
	    		// - If we need to do something with the sync packets from the Anchors, it will be here -
	    		// --------------------------------------------------------------------------------------
	    		EV << "Discarding the packet, computer doesn't make anything with Sync Broadcast packets from the AN" << endl;
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
		EV << "Discarding the packet, computer doesn't receive any packet during the Report and VIP phases" << endl;
		delete msg;
		break;
	case AppLayer::COM_SINK_PHASE_1:
		if (host->moduleType == 2) { // Mobile Node
			EV << "Discarding the packet, computer cannot receive in Com Sink 1 Phase any packet from a Mobile Node" << endl;
			delete msg;
		} else { // Anchor
			switch(pkt->getKind())
		    {
		    case AppLayer::REPORT_WITHOUT_CSMA:
		    case AppLayer::REPORT_WITH_CSMA:
		    	nbReportsReceived++;
				if (pkt->getDestAddr() == myNetwAddr) { // If the packet is for the computer
					if (pkt->getRealDestAddr() == myNetwAddr) { // If the packet was really for the computer
						nbReportsForMeReceived++;
						// The packet its for the computer, communication received from an Anchor
						getParentModule()->bubble("I've received the message");
						// ---------------------------------------------------------------------------------------------------------
						// - Here the Computer would do all the calculations to answer the anchor or the mobile node through an AN -
						// ---------------------------------------------------------------------------------------------------------
						// Now we interchange destination and source to send it back to the mobile node
						int realSrc = pkt->getRealDestAddr();
						int src = pkt->getDestAddr();
						pkt->setRealDestAddr(pkt->getRealSrcAddr());
						pkt->setDestAddr(pkt->getSrcAddr());
						pkt->setRealSrcAddr(realSrc);
						pkt->setSrcAddr(src);
						pkt->setRetransmisionCounterBO(0);	// Reset the retransmission counter BackOff
						pkt->setRetransmisionCounterACK(0);	// Reset the retransmission counter ACK
						pkt->setCSMA(true);

						if(!receivedId[pkt->getCreatedIn()*10000 + pkt->getId()]) { // Checks if the packet was already received to only count it once
							fromNode[pkt->getFromNode()]++;
							receivedId[pkt->getCreatedIn()*10000 + pkt->getId()] = true;
						}

						regPck[pkt->getCreatedIn()*10000 + pkt->getId()]++; // Saves the number of times a packet is received

						if(regPck[pkt->getCreatedIn()*10000 + pkt->getId()] == 1) { // If is the first delivery time
							if(pkt->getWasRequest()) { // Check if it was an ask
								requestOK++;

								timeRequest[(pkt->getCreatedIn())*200 + pkt->getId()] = pkt->getTimestampComRelated();
								delayRequest[(pkt->getCreatedIn())*200 + pkt->getId()] = (simTime().dbl() - pkt->getTimestampAnchorTX());

								pkt->setIsAnswer(true); // Puts a flag to inform the packet is now an answer to the ask for request
							} else {
								if(pkt->getWasBroadcast()) {	// Checks if it was a Broadcast
									broadOK[pkt->getPriority()]++;

									if(pkt->getNodeMode() == 1){
										timeBroad1[(pkt->getCreatedIn())*4000 + pkt->getId()] = pkt->getTimestampComRelated();
										delayBroad1[(pkt->getCreatedIn())*4000 + pkt->getId()] = (simTime().dbl() - pkt->getTimestampAnchorTX());
									}
									else if(pkt->getNodeMode() == 2){
										timeBroad2[(pkt->getCreatedIn())*4000 + pkt->getId()] = pkt->getTimestampComRelated();
										delayBroad2[(pkt->getCreatedIn())*4000 + pkt->getId()] = (simTime().dbl() - pkt->getTimestampAnchorTX());
									}
									else if(pkt->getNodeMode() == 3){
										timeBroad3[(pkt->getCreatedIn())*4000 + pkt->getId()] = pkt->getTimestampComRelated();
										delayBroad3[(pkt->getCreatedIn())*4000 + pkt->getId()] = (simTime().dbl() - pkt->getTimestampAnchorTX());
									}
									else if(pkt->getNodeMode() == 4){
										timeBroad4[(pkt->getCreatedIn())*4000 + pkt->getId()] = pkt->getTimestampComRelated();
										delayBroad4[(pkt->getCreatedIn())*4000 + pkt->getId()] = (simTime().dbl() - pkt->getTimestampAnchorTX());
									}
								}
								else if(pkt->getWasReport()) {	// Check if it was a report
									reportOK[pkt->getPriority()]++;

									if(pkt->getNodeMode() == 1){
										timeReport1[(pkt->getCreatedIn())*700 + pkt->getId()] = pkt->getTimestampComRelated();
										delayReport1[(pkt->getCreatedIn())*700 + pkt->getId()] = (simTime().dbl() - pkt->getTimestampAnchorTX());
									}
									else if(pkt->getNodeMode() == 2){
										timeReport2[(pkt->getCreatedIn())*700 + pkt->getId()] = pkt->getTimestampComRelated();
										delayReport2[(pkt->getCreatedIn())*700 + pkt->getId()] = (simTime().dbl() - pkt->getTimestampAnchorTX());
									}
									else if(pkt->getNodeMode() == 3){
										timeReport3[(pkt->getCreatedIn())*700 + pkt->getId()] = pkt->getTimestampComRelated();
										delayReport3[(pkt->getCreatedIn())*700 + pkt->getId()] = (simTime().dbl() - pkt->getTimestampAnchorTX());
									}
									else if(pkt->getNodeMode() == 4){
										timeReport4[(pkt->getCreatedIn())*700 + pkt->getId()] = pkt->getTimestampComRelated();
										delayReport4[(pkt->getCreatedIn())*700 + pkt->getId()] = (simTime().dbl() - pkt->getTimestampAnchorTX());
									}

								}
							}
						}
						// Setting the flags to identify the kind of message to false
						pkt->setWasRequest(false);
						pkt->setWasBroadcast(false);
						pkt->setWasReport(false);

						if(pkt->getIsAnswer()) { // If there has to be an answer to the packet received, the message is enqueued and adressed
							if (packetsQueue.length() < maxQueueElements) { // There is still place in the queue for more packets
								EV << "Enqueing  packet" << endl;
								pkt->setPriority(hops[pkt->getCreatedIn()]);
								packetsQueue.insertElem(pkt);
								EV << "Origen: " << pkt->getSrcAddr() << endl;
								EV << "Origen real: " << pkt->getRealSrcAddr() << endl;
								EV << "Destino: " << pkt->getDestAddr() << endl;
								EV << "Destino real: " << pkt->getRealDestAddr() << endl;
								EV << "Tipo: " << pkt->getKind() << endl;
								EV << "Nombre: " << pkt->getName() << endl;
								EV << "CSMA: " << pkt->getCSMA() << endl;
							} else {
								EV << "Queue full, discarding packet" << endl;
								nbPacketDroppedAppQueueFull++;
								delete msg;
							}
						}
						else {
							// Only askForRequest packets have to get an answer from the computer
							delete msg;
						}
					} else {
						// This case won't happen, a packet for the computer will be always in real for the computer
						EV << "Discarding packet, the computer received a packet that was not really for him, for us not possible" << endl;
		    			delete msg;
					}
				} else { // The computer doesn't route the packet
					// In this case we will not have this situation because the AN cannot communicate with each other,
					// they communicate with the computer and then this sends the packet to the other AN
					EV << "Discarding packet, the computer received a packet for another Anchor, and the computer does not route packets" << endl;
	    			delete msg;
				}
				break;
		    case AppLayer::SYNC_MESSAGE_WITHOUT_CSMA:
		    case AppLayer::SYNC_MESSAGE_WITH_CSMA:
		    	EV << "Discarding Packet, computer won't receive any Sync Broadcast Packet during Com Sink 1 Phase" << endl;
		    	delete msg;
		    	break;
		    default:
				EV << "Unknown Packet! -> delete, kind: " << msg->getKind() << endl;
				delete msg;
		    }
		}
		break;
	case AppLayer::COM_SINK_PHASE_2:
		EV << "Discarding packet, the computer doesn't receive any packet during Com Sink 2 Phase" << endl;
		delete msg;
		break;
	default:
		EV << "Unknown Phase! -> deleting packet, kind: " << msg->getKind() << endl;
		delete msg;
		break;
	}
	delete cInfo;
}

void ComputerAppLayer::handleLowerControl(cMessage *msg)
{
	ApplPkt* pkt;
	switch (msg->getKind())
	{
	case BaseMacLayer::PACKET_DROPPED_BACKOFF: // In case its dropped due to maximum BackOffs periods reached
		// Take the first message from the transmission queue, the first is always the one the MAC is refering to...
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
			delete pkt;
		}
		EV << endl;
		break;
	case BaseMacLayer::PACKET_DROPPED: // In case its dropped due to no ACK received...
		// Take the first message from the transmission queue, the first is always the one the MAC is refering to...
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
			delete pkt;
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
		// Take the first message from the transmission queue, the first is always the one the MAC is refering to...
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
		EV << "The Broadcast packet was successfully transmitted into the air" << endl;
		nbBroadcastPacketsSent++;
		delete pkt;
		break;
	case BaseMacLayer::TX_OVER:
		// Take the first message from the transmission queue, the first is always the one the MAC is refering to...
		pkt = check_and_cast<ApplPkt*>((cMessage *)transfersQueue.pop());
		EV << "Message correctly transmitted, received the ACK." << endl;
		nbReportsWithACK++;
		delete pkt;
		break;
	case BaseMacLayer::ACK_SENT:
		EV << "ACK correctly sent" << endl;
		break;
	}
	delete msg;
}

int ComputerAppLayer::hasSlot(int *slots, int *slotCounter, int numTotalSlots, int anchor, int numAnchors)
{
	int presenceInSlots = 0;
	for (int i = 0; i < numTotalSlots; i++)
	{
		for (int j = 0; j < *(slotCounter + i); j++)
		{
			if (*(slots + (i * numAnchors) + j) == anchor)
				presenceInSlots++;
		}
	}
	return presenceInSlots;
}

void ComputerAppLayer::orderQueue(int *queue, int *numerTimesQueue, int queueCounter)
{
    int tempQueue[(queueCounter+1)]; memset(tempQueue, 0, sizeof(int)*(queueCounter+1));
    int tempNumerTimesQueue[(queueCounter+1)]; memset(tempNumerTimesQueue, 0, sizeof(int)*(queueCounter+1));
    int tempQueueCounter = 0;
    int posMax = 0;
    int posMin = 0;

    for (int i = 0; i < queueCounter; i++)
    {
    	tempQueue[i] = queue[i];
    	tempNumerTimesQueue[i] = numerTimesQueue[i];
    	if (numerTimesQueue[i] > numerTimesQueue[posMax])
    		posMax = i;
    }
    for (tempQueueCounter = 0; tempQueueCounter <= queueCounter; tempQueueCounter++)
    {
    	posMin = posMax;
    	for (int i = 0; i < queueCounter; i++)
    	{
    		if (tempNumerTimesQueue[i] <= tempNumerTimesQueue[posMin])
    		{
    			posMin = i;
    		}
    	}
    	queue[tempQueueCounter] = tempQueue[posMin];
    	numerTimesQueue[tempQueueCounter] = tempNumerTimesQueue[posMin];
    	tempNumerTimesQueue[posMin] = tempNumerTimesQueue[posMax] + 1;
    }
}
