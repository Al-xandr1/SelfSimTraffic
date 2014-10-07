
#include <omnetpp.h>
#include <math.h>
#include <fstream>
#include <string>
#include <list>
#include "DevelopmentHelper.cc"
using namespace std;

class JitterBuffer : public cSimpleModule {
    protected:
        int ACCOUNTABLE_TAG_KIND;               // this tag 0 for the accountable deterministic traffic

        int numberOfCompoundQueues;

        cMessage *queueMsg;
        cMessage *senderMsg;

        simtime_t firstDelay;      // delay time of first packet
        simtime_t intertime;       // time between packets
        cQueue buffer;
        int bufferSize;
        bool needResetCounter;

        simtime_t lastArriveTime;
        simtime_t lastSendTime;

        simtime_t timeThresholdForStatistics;
        list<int> queueSizePoints;          //for buffer size graphic (values of the ordinate's axis)
        list<simtime_t> queueTimePoints;    //for buffer size graphic (values of the abscissa's axis)
        simtime_t graphicTimeSlot;
        simtime_t lastQueueTimePoint;

    private:
        void sendPacket(cPacket *pkt);
        void recievePacket(cPacket *pkt);
        void scheduleNextPacket();
        void collectSizePoint();

    // The following redefined virtual function holds the algorithm.
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

Define_Module(JitterBuffer);

#define QUEUE_MSG 100
#define SENDER_MSG 50

void JitterBuffer::initialize() {
    ACCOUNTABLE_TAG_KIND = par("tagKind");
    numberOfCompoundQueues= par("numberOfCompoundQueues_");

    firstDelay = par("firstDelay");
    intertime = par("intertime");

    bufferSize = par("bufferSize");
    needResetCounter = par("needResetCounter");

    lastSendTime = 0;
    lastArriveTime = 0;

    timeThresholdForStatistics = par("timeThresholdForStatistics_");
    graphicTimeSlot = par("graphicTimeSlot");
    lastQueueTimePoint = 0;

    EV << "JitterBuffer: firstDelay = " << firstDelay << ", intertime = " << intertime
            << ", graphicTimeSlot = " << graphicTimeSlot <<endl;

    queueMsg = new cMessage("QUEUE_MSG");
    queueMsg->setKind(QUEUE_MSG);

    senderMsg = new cMessage("SENDER_MSG");
    senderMsg->setKind(SENDER_MSG);

    scheduleAt(simTime(), queueMsg);
}

void JitterBuffer::handleMessage(cMessage *msg){
    if (msg->isSelfMessage()) {

        switch (msg->getKind()) {

            case QUEUE_MSG: {
                collectSizePoint();
                scheduleAt(simTime() + graphicTimeSlot, msg);

                break;
            }

            case SENDER_MSG: {
                if (!buffer.empty()) { //get packet from buffer, if any, and send
                    sendPacket(check_and_cast<cPacket *>(buffer.pop()));
                    scheduleNextPacket();

                } else {
                    EV << "JitterBuffer: buffer empty: simTime=" << simTime() <<endl;
                }
                break;
            }

            default: {
                EV << "JitterBuffer: ERROR!!!: get not classified message = " << msg->getFullName() << " !!!" <<endl;
                break;
            }
        }

    } else {    //new packet arrived
        if (msg->getKind() == ACCOUNTABLE_TAG_KIND) {

            if (lastArriveTime == 0) {// first packet arrived
                recievePacket(check_and_cast<cPacket *>(msg));
                scheduleAt(simTime() + firstDelay, senderMsg);

            } else {    //another packets arrived

                if (buffer.empty() && ((simTime()-lastSendTime) >= intertime)) {
                    EV << "JitterBuffer: SEND PACKET WITH EMPTY BUFFER" <<endl;
                    sendPacket(check_and_cast<cPacket *>(msg));

                } else {
                    recievePacket(check_and_cast<cPacket *>(msg));
                    if (!senderMsg->isScheduled()){
                        scheduleNextPacket();
                    }
                }

            }           //another packets arrived

        } else {
            EV << "JitterBuffer: WARNING!!!: unknown tag" << msg->getKind() << " !!!" <<endl;
        }
    }           //new packet arrived
}

void JitterBuffer::sendPacket(cPacket *pkt) {
    send(pkt, "out");
    lastSendTime = simTime();
}

void JitterBuffer::recievePacket(cPacket *pkt) {
    if (bufferSize > 0) {

        if (buffer.getLength() < bufferSize) {
            buffer.insert(pkt);
        } else {
            sendPacket(check_and_cast<cPacket *>(buffer.pop()));
            buffer.insert(pkt);

            if (needResetCounter) {
                cancelEvent(senderMsg);
                scheduleNextPacket();
            }
        }

    } else {
        buffer.insert(pkt);
    }
    lastArriveTime = simTime();
}

void JitterBuffer::scheduleNextPacket(){
	scheduleAt(lastSendTime + intertime, senderMsg);

}

void JitterBuffer::collectSizePoint(){
    lastQueueTimePoint = simTime();
    queueTimePoints.push_back(lastQueueTimePoint);

    if (simTime() >= timeThresholdForStatistics){
        queueSizePoints.push_back(buffer.getLength());
    } else {
        queueSizePoints.push_back(0);
    }
}

void JitterBuffer::finish(){
    if (queueMsg->isScheduled()) cancelAndDelete(queueMsg);
    else delete queueMsg;
    if (senderMsg->isScheduled()) cancelAndDelete(senderMsg);
    else delete senderMsg;

    char outFileName[256];
    DevelopmentHelper *helper = new DevelopmentHelper();
    ofstream out( helper->createFileName(outFileName, par("numberOfExperiment_"), par("bufferSizeGraphicName").stringValue(), -1) );

    out << "<?xml version=\'1.0' ?>" <<endl<<endl;
    out << "<BUFFER-SIZE-GRAPHIC>" <<endl;
    out << "  <TIME-THRESHOLD-FOR-STATISTICS>" << timeThresholdForStatistics << "</TIME-THRESHOLD-FOR-STATISTICS>" <<endl;
    out << "  <NUMBER-OF-COMPOUND-QUEUES>" << numberOfCompoundQueues << "</NUMBER-OF-COMPOUND-QUEUES>" <<endl;

    out << "  <BUFFER-PARAMETERS>" <<endl;
    out << "    <BUFFER-SIZE>" << bufferSize << "</BUFFER-SIZE>" <<endl;
    out << "    <WITH-COUNTER-RESET>" << needResetCounter << "</WITH-COUNTER-RESET>" <<endl;
    out << "    <FIRST-DELAY>" << firstDelay << "</FIRST-DELAY>" <<endl;
    out << "    <INTER-TIME>" << intertime << "</INTER-TIME>" <<endl;
    out << "    <FIRSTDELAY-DIV-INTERTIME>" << firstDelay / intertime << "</FIRSTDELAY-DIV-INTERTIME>" <<endl;
    out << "  </BUFFER-PARAMETERS>" <<endl;

    out << "  <RANGE>" << 0 << "  " << lastQueueTimePoint << "</RANGE>" <<endl;
    out << "  <GRAPHIC-TIME-SLOT>" << graphicTimeSlot << "</GRAPHIC-TIME-SLOT>" <<endl;
    out << "  <SIZE-OF-VECTOR>" << queueTimePoints.size() << "</SIZE-OF-VECTOR>" <<endl;
    out << "  <MAX-BUFFER-SIZE>" << helper->countMaxValue(queueSizePoints) << "</MAX-BUFFER-SIZE>" <<endl;

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

    out << "</BUFFER-SIZE-GRAPHIC>" <<endl;
    out.close();
}
