package org.mixim.base.modules;

// Module which is responsible for mobility
// related information like position and movement
// BaseMobility itself defines a static mobility
// pattern (means only a position, no movement).
// Mobility modules which extend from this module
// will define more complex movement patterns.
simple BaseMobility like IMobility
{
    parameters:
        bool notAffectedByHostState = default(true);
        bool coreDebug = default(false); // debug switch for the core framework
        double constraintAreaMinX @unit(m) = default(-1m/0); // min x position of the constraint area, unconstrained by default (negative infinity)
        double constraintAreaMinY @unit(m) = default(-1m/0); // min y position of the constraint area, unconstrained by default (negative infinity)
        double constraintAreaMinZ @unit(m) = default(-1m/0); // min z position of the constraint area, unconstrained by default (negative infinity)
        double constraintAreaMaxX @unit(m) = default(1m/0); // max x position of the constraint area, unconstrained by default (positive infinity)
        double constraintAreaMaxY @unit(m) = default(1m/0); // max y position of the constraint area, unconstrained by default (positive infinity)
        double constraintAreaMaxZ @unit(m) = default(1m/0); // max z position of the constraint area, unconstrained by default (positive infinity)
        double initialX @unit(m) = default(uniform(constraintAreaMinX, constraintAreaMaxX)); // initial x coordinate of the nodes' position
        double initialY @unit(m) = default(uniform(constraintAreaMinY, constraintAreaMaxY)); // initial y coordinate of the nodes' position
        double initialZ @unit(m) = default(uniform(constraintAreaMinZ, constraintAreaMaxZ)); // initial z coordinate of the nodes' position
        bool initFromDisplayString = default(false); // initial position will be read from display string
//***VMOD***/        
 		double x; // x coordinate of the nodes' position (-1 = random)
        double y; // y coordinate of the nodes' position (-1 = random)
        double z; // z coordinate of the nodes' position (-1 = random)
//***MOD***/
    	double minimumDistanceComputer = default(0); // Modified by Jorge Perez: minimum distance in m between any two computers in map
        double minimumDistanceAnchor = default(0); // Modified by Jorge Perez: minimum distance in m between any two anchors in map
        double minimumDistanceNode = default(0); // Modified by Jorge Perez: minimum distance in m between any two mobile nodes in map
//*********/   

		@display("i=block/cogwheel");
}

