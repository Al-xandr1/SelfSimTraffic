// Simplest Sink for arriving packets

#include <omnetpp.h>

class Sink : public cSimpleModule
{
  protected:
   
    // The following redefined virtual function holds the algorithm.
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};


// The module class needs to be registered with OMNeT++
Define_Module(Sink);


void Sink::initialize(){}


void Sink::handleMessage(cMessage *msg)
{
    delete msg;    
}
