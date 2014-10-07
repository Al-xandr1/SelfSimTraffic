// Collects packets from all individual sources into one output stream

#include <omnetpp.h>

class ONOFF_Collector : public cSimpleModule
{
  protected:
    
    // The following redefined virtual function holds the algorithm.
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

};

// The module class needs to be registered with OMNeT++
Define_Module(ONOFF_Collector);

void ONOFF_Collector::initialize()
{
    // Initialize is called at the beginning of the simulation.
    
}

void ONOFF_Collector::handleMessage(cMessage *msg)
{
    // The handleMessage() method is called whenever a message arrives
    // at the module.
    send(msg,"out");
}
