package org.mixim.base.modules;

// Interface for mobility modules
moduleinterface IMobility
{
    parameters:
        double initialX @unit(m); // initial x coordinate of the nodes' position (1m/0 = random)
        double initialY @unit(m); // initial y coordinate of the nodes' position (1m/0 = random)
        double initialZ @unit(m); // initial z coordinate of the nodes' position (1m/0 = random)
        @display("i=block/cogwheel");
}

