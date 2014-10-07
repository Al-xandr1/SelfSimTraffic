// FIFO Queue of packets for transmission through output channel. 
// If flag "taging" is set, property "Kind" of each transmitted packet 
// will be set to the number of gate, from which this packet arrived to
// the queue. 

#include <omnetpp.h>
#include <math.h>
#include <fstream>
#include <string>
#include <list>
#include "DevelopmentHelper.cc"
using namespace std;

class TxQueue : public cSimpleModule
{
  protected:
    cQueue queue;

    cMessage *queueMsg;
    cMessage *senderMsg;

    simtime_t timeThresholdForStatistics;
    list<int> queueSizePoints; //for queue size graphic (values of the ordinate's axis)
    list<simtime_t> queueTimePoints; //for queue size graphic (values of the abscissa's axis)
    simtime_t graphicTimeSlot;
    simtime_t lastTimePoint;

    bool taging;
    int base;
    
  private:
    void collectSizePoint();

    // The following redefined virtual function holds the algorithm.
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

// The module class needs to be registered with OMNeT++
Define_Module(TxQueue);

#define QUEUE_MSG 100
#define SENDER_MSG 50

void TxQueue::initialize() {
    timeThresholdForStatistics = par("timeThresholdForStatistics_");
    graphicTimeSlot = par("graphicTimeSlot");
    lastTimePoint = 0;

    taging = par("taging");
    if (taging) base = gate("in",0)->getId();

    queueMsg = new cMessage("QUEUE_MSG_QUEUE_MODULE");
    queueMsg->setKind(QUEUE_MSG);

    senderMsg = new cMessage("SENDER_MSG_QUEUE_MODULE");
    senderMsg->setKind(SENDER_MSG);

    scheduleAt(simTime(), queueMsg);
}

void TxQueue::handleMessage(cMessage *msg) {
    if (msg->isSelfMessage()) {  // channel became free

        switch (msg->getKind()) {

            case QUEUE_MSG: {
                collectSizePoint();
                scheduleAt(simTime() + graphicTimeSlot, msg);

                break;
            }

            case SENDER_MSG: {

                if (!queue.empty()) {  // get packet from queue, if any, and send
                    send(check_and_cast<cPacket *>(queue.pop()), "out");
                    scheduleAt(gate("out")->getTransmissionChannel()->getTransmissionFinishTime(), msg);

                } else {
                    EV << "TxQueue: queue empty: simTime=" << simTime() <<endl;
                }
                break;
            }

            default: {
                EV << "TxQueue: ERROR!!!: received not classified message = " << msg->getFullName() << " !!!" <<endl;
                break;
            }
        }

    } else {    //new packet arrived

        // set packet kind equal to the number of arrival gate
        if (taging) {
            msg->setKind(msg->getArrivalGateId() - base);
        }

        // sending or queuing this packet
        if (!queue.empty()) {
            queue.insert(check_and_cast<cPacket *>(msg));

        } else {
            if (gate("out")->getTransmissionChannel()->isBusy()){
                queue.insert(check_and_cast<cPacket *>(msg));
                if (!senderMsg->isScheduled())
                    scheduleAt(gate("out")->getTransmissionChannel()->getTransmissionFinishTime(), senderMsg);

            } else {
                send(msg, "out");

                if (!senderMsg->isScheduled())
                    scheduleAt(gate("out")->getTransmissionChannel()->getTransmissionFinishTime(), senderMsg);
            }
        }
    }           //new packet arrived
}

void TxQueue::collectSizePoint() {
    lastTimePoint = simTime();
    queueTimePoints.push_back(lastTimePoint);

    if (simTime() >= timeThresholdForStatistics){
        queueSizePoints.push_back(queue.getLength());
    } else {
        queueSizePoints.push_back(0);
    }
}

void TxQueue::finish() {
    if (queueMsg->isScheduled()) cancelAndDelete(queueMsg);
    else delete queueMsg;
    if (senderMsg->isScheduled()) cancelAndDelete(senderMsg);
    else delete senderMsg;

    char outFileName[256];
    DevelopmentHelper *helper = new DevelopmentHelper();
    ofstream out( helper->createFileName(outFileName, par("numberOfExperiment_"), par("queueSizeGraphicName").stringValue(), (int)par("fileSuffix")) );

    out << "<?xml version=\'1.0' ?>" <<endl<<endl;
    out << "<QUEUE-SIZE-GRAPHIC>" <<endl;
    out << "  <TIME-THRESHOLD-FOR-STATISTICS>" << timeThresholdForStatistics << "</TIME-THRESHOLD-FOR-STATISTICS>" <<endl;
    out << "  <RANGE>" << 0 << "  " << lastTimePoint << "</RANGE>" <<endl;
    out << "  <GRAPHIC-TIME-SLOT>" << graphicTimeSlot << "</GRAPHIC-TIME-SLOT>" <<endl;
    out << "  <SIZE-OF-VECTOR>" << queueTimePoints.size() << "</SIZE-OF-VECTOR>" <<endl;
    out << "  <MAX-QUEUE-SIZE>" << helper->countMaxValue(queueSizePoints) << "</MAX-QUEUE-SIZE>" <<endl;

    out << "  <TIME-POINTS>";
    list<simtime_t>::iterator iterTime;
    for (iterTime = queueTimePoints.begin(); iterTime != queueTimePoints.end(); ++iterTime)
        out << *iterTime <<"  ";
    out << "</TIME-POINTS>" <<endl;

    out << "  <SIZE-POINTS>";
    list<int>::iterator iterSize;
    for (iterSize = queueSizePoints.begin(); iterSize != queueSizePoints.end(); ++iterSize)
        out << *iterSize <<"  ";
    out << "</SIZE-POINTS>" <<endl;

    out << "</QUEUE-SIZE-GRAPHIC>" <<endl;
    out.close();
};
