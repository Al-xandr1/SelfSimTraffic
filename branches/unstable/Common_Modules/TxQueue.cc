// FIFO Queue of packets for transmission through output channel. 
// If flag "taging" is set, property "Kind" of each transmitted packet 
// will be set to the number of gate, from which this packet arrived to
// the queue. 

#include <string.h>
#include <omnetpp.h>

class TxQueue : public cSimpleModule
{
  protected:
    cQueue queue;
    cChannel *txOut;

    bool taging;
    int base;
    
    // The following redefined virtual function holds the algorithm.
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};


// The module class needs to be registered with OMNeT++
Define_Module(TxQueue);


void TxQueue::initialize()
{
    // Initialize is called at the beginning of the simulation.

    txOut=gate("out")->getTransmissionChannel();

    if( taging=par("taging") ) base=gate("in",0)->getId();
}


void TxQueue::handleMessage(cMessage *msg)
{
    // The handleMessage() method is called whenever a message arrives
    // at the module.
    if( msg->isSelfMessage() )   // channel became free
        // get packet from queue, if any, and send
        if( !queue.empty() ) {
            cPacket *pkt=check_and_cast<cPacket *>(queue.pop());
            send(pkt,"out");
            simtime_t channel_free=txOut->getTransmissionFinishTime();
            scheduleAt(channel_free, msg);
            }
        else delete msg;     
    else {      // new packet arrived
        // set packet kind equal to the number of arrival gate
        if( taging ) msg->setKind( msg->getArrivalGateId() - base);
        // sending or queuing this packet 
        if( queue.empty() && !txOut->isBusy() ) { 
            send(msg,"out");
            simtime_t channel_free=txOut->getTransmissionFinishTime();
            cMessage *m = new cMessage("self");
            scheduleAt(channel_free, m);
            }
        else {
            cPacket *pkt = check_and_cast<cPacket *>(msg);
            queue.insert(pkt);
            }
        }      
}
