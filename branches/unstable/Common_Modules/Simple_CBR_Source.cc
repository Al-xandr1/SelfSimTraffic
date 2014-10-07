// Simplest constant bit rate source. It generates packets (cells) with 
// constant time between them. Size of packets (cells) is also constant.

#include <string>
#include <vector>
#include <math.h>
#include <omnetpp.h>

class Simple_CBR_Source : public cSimpleModule
{
  protected:
    simtime_t intertime;     // time between packets
    int bytesize;            // cell size in bytes
            
    // The following redefined virtual function holds the algorithm.
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};


// The module class needs to be registered with OMNeT++
Define_Module(Simple_CBR_Source);


void Simple_CBR_Source::initialize()
{
    // Initialize is called at the beginning of the simulation.
    
    // basic parameters initialization
    intertime  = (simtime_t)par("intertime");
    bytesize  = par("bytesize");

    // Starting message scheduling
    cMessage *msg = new cMessage("next");
    scheduleAt( simTime(), msg);
    
}


void Simple_CBR_Source::handleMessage(cMessage *msg)
{
    // The handleMessage() method is called whenever a message arrives
    // at the module.
    
       cPacket *pkt = new cPacket("packet");
       pkt->setByteLength(bytesize);
       send(pkt, "out");
       scheduleAt(simTime()+intertime, msg);
}
