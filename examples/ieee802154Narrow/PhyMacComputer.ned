package org.mixim.examples.ieee802154Narrow;

import org.mixim.modules.mobility.LinearMobility;
//import org.mixim.modules.application.BurstApplLayer;
import org.mixim.base.modules.BaseWorldUtility;
import org.mixim.base.modules.BaseArp;
import org.mixim.base.modules.BaseMobility;
import org.mixim.modules.power.battery.SimpleBattery;
import org.mixim.modules.power.battery.BatteryStats;

import org.mixim.modules.nic.Nic802154_TI_CC2420;

module PhyMacComputer
{
    parameters:
        double numComputers; // total number of hosts in the network

        @display("bgb=363,335,white;bgp=10,10");
    gates:
        input radioIn;

    submodules:
        batteryStats: BatteryStats {
            @display("p=273,182;i=block/circle");
        }
        battery: SimpleBattery {
            @display("p=205,182;i=block/control");
        }
        utility: BaseWorldUtility {
            parameters:
                @display("p=239,122;i=block/blackboard");
        }
        mobility: BaseMobility {
            parameters:
                @display("p=239,57;i=block/cogwheel");
        }
        arp: BaseArp {
            @display("p=239,240;b=30,25;i=block/process");
        }
        appl: ComputerAppLayer {
            parameters:
                @display("p=119,42;b=20,20,rect,white;i=block/app");
        }
        trans: ComputerTransLayer {
            parameters:
                @display("p=59,96;b=20,20,rect,white;i=block/join");
        }
        net: ComputerNetLayer {
            parameters:
                @display("p=119,152;b=20,20,rect,white;i=block/network2");
        }
        nic: Nic802154_TI_CC2420 {
            parameters:
                @display("b=32,30;p=59,206;i=block/ifcard");
        }
    connections:
        trans.upperLayerOut --> { @display("ls=black;m=m,25,50,25,0"); } --> appl.lowerLayerIn;
        trans.upperLayerIn <-- { @display("ls=black;m=m,15,50,15,0"); } <-- appl.lowerLayerOut;
        trans.upperControlOut --> { @display("ls=red;m=m,75,50,75,0"); } --> appl.lowerControlIn;
        trans.upperControlIn <-- { @display("ls=red;m=m,85,0,85,0"); } <-- appl.lowerControlOut;
        net.upperLayerOut --> { @display("ls=black;m=m,25,50,25,0"); } --> trans.lowerLayerIn;
        net.upperLayerIn <-- { @display("ls=black;m=m,15,50,15,0"); } <-- trans.lowerLayerOut;
        net.upperControlOut --> { @display("ls=red;m=m,75,50,75,0"); } --> trans.lowerControlIn;
        net.upperControlIn <-- { @display("ls=red;m=m,85,0,85,0"); } <-- trans.lowerControlOut;
        nic.upperLayerOut --> { @display("ls=black;m=m,25,50,25,0"); } --> net.lowerLayerIn;
        nic.upperLayerIn <-- { @display("ls=black;m=m,15,50,15,0"); } <-- net.lowerLayerOut;
        nic.upperControlOut --> { @display("ls=red;m=m,75,50,75,0"); } --> net.lowerControlIn;
        nic.upperControlIn <-- { @display("ls=red;m=m,85,0,85,0"); } <-- net.lowerControlOut;
        radioIn --> nic.radioIn;

}

