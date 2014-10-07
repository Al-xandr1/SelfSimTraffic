// Individual source, generated by MGI_Generator class object.
// Works finite time and then disappears.

#include <string>
#include <vector>
#include <math.h>
#include <omnetpp.h>

class Ind_MGI_LOG_Source : public cSimpleModule
{
  protected:
    simtime_t timeslot;      // time between packets
    int cellsize;            // cell size in bytes
    int pktsize;             // packet size in bytes
    simtime_t timeleft;      // time of working before disappirance

    cXMLElement *doc;        // file with source parameters

    cGate *dst;              // gate for packet direct sending 
    
    // Source speed parameters and functions       
    std::vector<double> dstS;
    int setPacketSize();
 
    // timeleft distribution parameters and functions
    std::vector<double> pA;   
    simtime_t setTimeLeft();     

    // The following redefined virtual function holds the algorithm.
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};


// The module class needs to be registered with OMNeT++
Define_Module(Ind_MGI_LOG_Source);


#define gDEBP(A,B) (A->getDocumentElementByPath(A,B))->getNodeValue()


void Ind_MGI_LOG_Source::initialize()
{
    // Initialize is called at the beginning of the simulation.
    

    doc=par("profile");

    // basic parameters initialization
    timeslot = atof(gDEBP(doc,"/TIMESLOT"));
    cellsize  = atoi(gDEBP(doc,"/CELLSIZE"));

    pktsize = setPacketSize();
    timeleft= setTimeLeft();  

    dst= getParentModule()->getSubmodule("collect")->gate("in");

    // Starting message scheduling
    cMessage *msg = new cMessage("next");
    scheduleAt( simTime(), msg);   
}


void Ind_MGI_LOG_Source::handleMessage(cMessage *msg)
{
    // The handleMessage() method is called whenever a message arrives
    // at the module.
    
    if ( timeleft>0) {  // generate new paket
       cPacket *pkt = new cPacket("packet");
       pkt->setByteLength(pktsize);
       sendDirect(pkt, dst);
       scheduleAt(simTime()+timeslot, msg);
       timeleft-=timeslot;
       }
    else {   // stop working and disappire
       delete msg;
       callFinish();
       deleteModule();
       }
}


int Ind_MGI_LOG_Source::setPacketSize()
{ 
    return cellsize;
}

simtime_t Ind_MGI_LOG_Source::setTimeLeft()
{
    
    // preparing data for timeleft calculation
    cStringTokenizer tok(gDEBP(doc,"/ACF-SHAPE-PARAMETERS"));
    pA = tok.asDoubleVector();
    double alpha= pA[0];  
    double px   = pA[1];
    double py   = pA[2];
    double A    = pA[3];
    double imax = pA[4]+0.5;
    
    // timeleft random value generation
    int i;
    double s, x=uniform(0,1);
    for(s=py,i=2; x>=s && i<imax; i++) s+=A*pow(px+i,-alpha); 
    return timeslot*(i-1);
}
