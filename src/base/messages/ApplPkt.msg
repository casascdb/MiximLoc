//**************************************************************************
// * file:        ApplPkt.msg
// *
// * author:      Jorge Perez Hidalgo
// *
// * copyright:   (C) 2011 Telecommunication Networks Group (TKN) at
// *              Technische Universitaet Dresden, Germany.
// *
// *              This program is free software; you can redistribute it 
// *              and/or modify it under the terms of the GNU General Public 
// *              License as published by the Free Software Foundation; either
// *              version 2 of the License, or (at your option) any later 
// *              version.
// *              For further information see file COPYING 
// *              in the top level directory
// **************************************************************************
// * part of:     framework implementation developed by tkn
// * description: - base class for application layer packets
// *              - subclass to create your own application layer packets
// **************************************************************************/

cplusplus {{
    #include "SimpleAddress.h"
}}

class LAddress::L3Type extends void;

// Basic application packet definition
//
// subclass if you want to create your own application layer packet
// class
//
// @author Jorge Perez Hidalgo
packet ApplPkt
{
    LAddress::L3Type destAddr = LAddress::L3BROADCAST; // destination address
    LAddress::L3Type srcAddr  = LAddress::L3BROADCAST; // source address
	int sequenceId = 0;				// a counter to identify the packet
    int priority = 1; 				// Priority of the packet. From 1 to 4, 4 the highest.
    int timeOfLife = 1;				// Number of periods left to stay alive. -1 means immortal packet.
    int8 status = 0;
	int16 posX = 0;
	int16 posY = 0;
	int16 posZ = 0;
	int32 timestamp = 0;
	// This two address are there because: when a MN transmits the destAddr is the selected AN, but the real destination can be the computer
	// And also when a computer sends something to a MN, the destAddr is the selected AN but the real one is the MN, and more cases like this

	int realDestAddr = -1;			// Real destination, in case the packet has to be routed from MN to Computer through an AN
	int realSrcAddr = -1;			// Real source, in case the packet has to be routed from Computer to MN through an AN
	int retransmisionCounterBO = 0;	// This variable counts the number or retransmission of this packet due to backoff
	int retransmisionCounterACK = 0;// This variable counts the number or retransmission of this packet due to lack of ACK
	bool CSMA = true;				// Variable to activate or deactivate the CSMA
	bool askForRequest = false;		// This FLAG in the packet tells the AN that in the next full phase (period) the Mobile Node will ask for some information and the AN has to have it ready
	bool requestPacket = false;		// This FLAG in the packet tells the AN that it has to answer the Mobile Node with some info and if it doesn't have anything ask anyway saying the info is not yet there
	int numberOfBroadcasts = 0;		// When a mobile node sends a Broadcast, here it will write how many total broadcasts would it send
	bool sendInProcess = false;		// A FLAG to mark a packet when it has been send to the CSMA device layer
	
	bool wasRequest = false;		// A flag to mark the packets which were created from a request coming from a Mobile Node
	bool wasReport = false;			// A flag to mark the packets which were created from a report coming from a Mobile Node
	bool wasBroadcast = false;		// A flag to mark the packets which were created from a broadcast coming from a Mobile Node
	bool isAnswer = false;			// A flag to mark the packets which were created as an answer to a request packet

	int id;							// An unique identifier for any packet; for statistical purpose
	int createdIn;					// Variable to identify the first anchor which received the message; for statistical purpose only
	int nodeMode;					// Variable to identify the mode of the node who created the message; for statistical purpose only
	int fromNode;					// Variable to identify the node who sent the message; for statistical purpose only
	
	double timestampAnchorTX = 0;	//Variable to save the time when the packet was first sent from the first anchor of the route
	double timestampComRelated = 0; //Variable to save the time when the packet was first sent referred from the start time of Com Sink 1
	
	double broadcastedSuccess = 1;	//Variable to broadcast to other anchors the success of an anchor

}
