/* -*- mode:c++ -*- ********************************************************
 * file:        MoBANCoordinator.h
 *
 * author:      Majid Nabi <m.nabi@tue.nl>
 *
 *              http://www.es.ele.tue.nl/nes
 *
 * copyright:   (C) 2010 Electronic Systems group(ES),
 *              Eindhoven University of Technology (TU/e), the Netherlands.
 *
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 ***************************************************************************
 * part of:    MoBAN (Mobility Model for wireless Body Area Networks)
 * description:     Implementation of the coordinator module of the MoBAN mobility model
 ***************************************************************************
 * Citation of the following publication is appreciated if you use MoBAN for
 * a publication of your own.
 *
 * M. Nabi, M. Geilen, T. Basten. MoBAN: A Configurable Mobility Model for Wireless Body Area Networks.
 * In Proc. of the 4th Int'l Conf. on Simulation Tools and Techniques, SIMUTools 2011, Barcelona, Spain, 2011.
 *
 * BibTeX:
 *		@inproceedings{MoBAN,
 * 		author = "M. Nabi and M. Geilen and T. Basten.",
 * 	 	title = "{MoBAN}: A Configurable Mobility Model for Wireless Body Area Networks.",
 *    	booktitle = "Proceedings of the 4th Int'l Conf. on Simulation Tools and Techniques.",
 *    	series = {SIMUTools '11},
 *    	isbn = {978-963-9799-41-7},
 *	    year = {2011},
 *    	location = {Barcelona, Spain},
 *	    publisher = {ICST} }
 *
 **************************************************************************/

#include <cxmlelement.h>
#include <vector>

#include <omnetpp.h>

#include "MiXiMDefs.h"
#include "BaseWorldUtility.h"
#include "Posture.h"
#include "PostureTransition.h"

/**
 *\page mobancpp MoBAN C++ reference
 * This is the C++ implementation reference for MoBAN, which is a configurable mobility model for wireless Body Area Networks. MoBANCoordinator and MoBANLocal
 * classes describe the behavior of the Coordinator module and local mobility module of the MoBAN, respectively. There are two more classes which are used in the
 * coordinator module.
 *
 * Class Posture is responsible for maintaining an instance of defined posture for the WBAN. In the initialization phase, the MoBANCoordinator reads the input
 * posture specification file (.xml) and makes a list of posture classes, each maintaining the information of one defined posture.
 *
 * Class PostureTransition is responsible for maintaining the configuration information regarding the spatial and temporal correlations in posture selection strategy of MoBAN.
 * In the initialization phase, the MoBANCoordinator reads the input configuration file and pass the given information about the area types, time domains, various
 * Markov transition matrices, and the setups which specify which matrix should be used in a particular space-time combination. During the simulation run, the class provides
 * a function that determines the proper Markov matrix for a give time and location.
 *
 * @ingroup MoBAN
 * @author Majid Nabi
 *
*/

/**
 * @brief This is the coordinator module of the MoBAN mobility model. It should be instantiated in the top level simulation network in MiXiM, once per WBAN.
 * The coordinator module is the main module that provides the group mobility and correlation between nodes in a WBAN.
 * In the initialization phase, it reads three user defined input files which are the postures specification file, a configuration file which includes all
 * required parameter for specific distributions, and the previously logged mobility pattern, if it is requested to use a logged pattern.
 * Note that all WBAN instances may use the same input files if they are exactly in the same situation.
 *
 * After the initialization phase, the MoBAN coordinator decides about the posture and the position of the Logical center of the group (WBAN).
 * The absolute position of the reference point of each belonging node is calculated by adding the current position of the logical center
 * by the reference point of that node in the selected posture. The coordinator publish the position of the reference point as well as
 * the speed and the radius of the local movement of nodes to their signaling systems.
 *
 * @ingroup mobility
 * @ingroup MoBAN
 * @author Majid Nabi
 */
class MIXIM_API MoBANCoordinator: public cSimpleModule
{
  private:
	/** @brief Copy constructor is not allowed.
	 */
	MoBANCoordinator(const MoBANCoordinator&);
	/** @brief Assignment operator is not allowed.
	 */
	MoBANCoordinator& operator=(const MoBANCoordinator&);

  protected:
	/** @brief Debug switch for all other modules*/
    bool debug;

    /** @brief Pointer to the BaseWorldUtility */
	 BaseWorldUtility *world;

    /** @brief Pointer to the top level (network)*/
    cModule* network;

    /** @brief Interval for updating the move (NED parameter) */
	simtime_t updateInterval;

    /** @brief A self message to trigger move step every updateInterval seconds. */
	cMessage* MoveMsg;


    /** @brief The number of nodes in the this WBAN */
    unsigned int numNodes;

    /** @brief Currently selected speed for the mobile posture */
    double speed;

    /** @brief Index of this WBAN */
    int index;

    /** @brief Pointer to the file for logging MoBAN mobility pattern for future use */
    FILE* logfile;

    /** @brief Number of predefined postures */
    unsigned int numPostures;

    /** @brief The list of all predefined postures (posture data base) */
    std::vector<Posture*> postureList;

    /** @brief The current selected posture */
    Posture* currentPosture;

    /** @brief The selected duration of the current posture */
    simtime_t duration;

    /** @brief The minimum value of the duration for stable postures */
    simtime_t minDuration;

    /** @brief The maximum value of the duration for stable postures */
    simtime_t maxDuration;

    /** @brief  the index list of nodes belong to this WBAN */
    int* nodeIndex;

    /** @brief Current position of the logical center of the group */
    Coord logicalCenter;

    /** @brief Number of steps already moved */
    int step;

	/** @brief Total number of steps of the current move or current posture */
    int numSteps;

    /** @brief Selected destination for the ongoing move of the logical center */
    Coord targetPos;

    /** @brief The intermediate target of the current move step */
    Coord stepTarget;

    /** @brief The size of the current move step*/
    Coord stepSize;


    /** @brief Variable that shows if reusing previously logged mobility pattern is requested.
     * The value is gotten from the parameter of the module*/
    bool useMobilityPattern;

    /** @brief Data type for one instance of mobility pattern. */
    typedef struct pattern{
    	unsigned int postureID;
    	Coord targetPos;
    	double speed;
    	simtime_t duration;

    	pattern()
    		: postureID(0)
    		, targetPos()
    		, speed(0)
    		, duration()
    	{}
    }Pattern;


    /** @brief The mobility pattern data base. */
    Pattern* mobilityPattern;

    /** @brief The number of mobility pattern instances which has been read from the input file
     * (length of mobility pattern data base).
    */
    int patternLength;

    /** @brief The index of the currently applied mobility pattern from */
    int currentPattern;

    /** @brief A matrix which maintains the transition probabilities of the Markov Model of posture pattern.
     *  To be given through configuration file.
    */
    double** markovMatrix;

    /** @brief Possible (supported) strategies for posture selection. */
    enum posture_sel_type {
      UNIFORM_RANDOM = 0,   // uniform random posture selection. No correlation is applied.
      MARKOV_BASE		    // Either a Markov model matrix or a steady state vector is given for space-time domains
    };

    /** @brief The requested strategy for posture selection. To be given through configuration file. */
    posture_sel_type postureSelStrategy;

    /** @brief Class for performing operation for spatial and temporal correlations in posture selection. */
    PostureTransition* transitions;

  public:
    MoBANCoordinator()
    	: cSimpleModule()
    	, debug(false)
    	, world(NULL)
    	, network(NULL)
    	, updateInterval()
    	, MoveMsg(NULL)
    	, numNodes(0)
    	, speed(0)
    	, index(0)
    	, logfile(NULL)
    	, numPostures(0)
    	, postureList()
    	, currentPosture(NULL)
    	, duration()
    	, minDuration()
    	, maxDuration()
    	, nodeIndex(NULL)
    	, logicalCenter()
    	, step(0)
    	, numSteps(0)
    	, targetPos()
    	, stepTarget()
    	, stepSize()
    	, useMobilityPattern(false)
    	, mobilityPattern(NULL)
    	, patternLength(0)
    	, currentPattern(0)
    	, markovMatrix(NULL)
    	, postureSelStrategy(UNIFORM_RANDOM)
    	, transitions(NULL)
    {}

    /** @brief Initializes the MoBAN mobility model.*/
    virtual void initialize(int);

  protected:

    /** @brief Handes self messages and initiate the next message for updateInterval seconds later. */
    void handleMessage(cMessage *msg);

    /** @brief To be called at the end of simulation run. */
    virtual void finish();

   /** @brief Function to select the next posture considering the current posture and the Markov model. */
    void selectPosture();

   /** @brief Function to select the destination in the case of a mobile posture. */
   Coord selectDestination();

   /** @brief Select a velocity value within the given velocity range. */
   double selectSpeed();

   /** @brief Select a stay time duration in the specified range for the new posture. */
   simtime_t  selectDuration();

    /** @brief The main process of the MoBAN mobility model. */
   void mainProcess();

   /** @brief Making one step move and publishing the new reference point position to all belonging nodes. */
   void stepMove();

   /** @brief Checks if all nodes of the WBAN are inside the simulation environment w	ith the given position of the logical center. */
   bool isInsideWorld(const Coord&) const;

   /** @brief Reading the input postures specification file and making the posture data base. */
   bool readPostureSpecificationFile();

   /** @brief Reads the input configuration file. */
   bool readConfigurationFile();

   /** @brief Reads the previously logged mobility pattern and make mobility pattern data base.*/
   bool readMobilityPatternFile();

   /** @brief Publishes the reference point and other information of the posture to the signaling system of the belonging nodes.*/
   void publishToNodes();

};
