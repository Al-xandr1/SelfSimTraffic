// Individual source for construction ON-OFF Source with several 
// such sources

#include <cstdlib>
#include <string>
#include <vector>
#include <math.h>
#include <fstream>
#include <omnetpp.h>

enum SourceState {ACTIVE, SLEEP};

class Ind_ONOFF_Source : public cSimpleModule
{
  protected:
    simtime_t timeslot;      // time between packets
    int cellsize;            // cell size in bytes
    int pktsize;             // packet size in bytes
    simtime_t timeleft;      // time of active period left
     
    cGate *dst;              // gate for packet direct sending 
    
    SourceState state;       // state of trivial ON-OFF FSM

    cXMLElement *doc;        // XML file with source parameters

    // Source speed parameters and functions    
    std::vector<double> dstS;  // distribution
    void initSpeedData();
    int  setPacketSize();   
    
    // Active period parameters and functons
    std::vector<double> pA;    // parameters (alpha, px, py, A, imax) 
    std::vector<double> cpfA;  // initial part of cumulative prob fun (for saving calculation time)
    void  initActiveData();  
    simtime_t setActiveTime();    

    // Sleep period parameters and functions
    std::vector<double> pB;    // parameters (sleepLambda) 
    void  initSleepData();
    simtime_t setSleepTime();

    // The following redefined virtual function holds the algorithm.
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};


// The module class needs to be registered with OMNeT++
Define_Module(Ind_ONOFF_Source);

#define gDEBP(A,B) (A->getDocumentElementByPath(A,B))->getNodeValue()
 
void Ind_ONOFF_Source::initialize()
{
    // Initialize is called at the beginning of the simulation.
   
    doc=par("profile");

    // basic parameters initialization
    timeslot = atof(gDEBP(doc,"/TIMESLOT"));
    cellsize = atoi(gDEBP(doc,"/CELLSIZE"));

    dst= getParentModule()->getSubmodule("collect")->gate("in");

    // other prameters initialization
    initSpeedData();
    initActiveData();
    initSleepData();

    // Starting message scheduling
    state = SLEEP;
    cMessage *msg = new cMessage("next");
    scheduleAt( simTime(), msg);
}


void Ind_ONOFF_Source::handleMessage(cMessage *msg)
{
    // The handleMessage() method is called whenever a message arrives
    // at the module.

    if (state == SLEEP) {  // wake up and start new active period
       
       state = ACTIVE;
       
       // set up parameters of this active period
       pktsize = setPacketSize();
       timeleft= setActiveTime();

       // generate first packet in this active period
       cPacket *pkt = new cPacket("packet");
       pkt->setByteLength(pktsize);
       sendDirect(pkt, dst);
       scheduleAt(simTime()+timeslot, msg);

       timeleft-=timeslot;
    }
    else if ( timeleft>0) {  // inside current active period generate new paket
       
       cPacket *pkt = new cPacket("packet");
       pkt->setByteLength(pktsize);
       sendDirect(pkt, dst);
       scheduleAt(simTime()+timeslot, msg);

       timeleft-=timeslot;
    }
    else {                  // active period expired, go to sleep
       state = SLEEP;

       // set up length of this sleep period
       scheduleAt(simTime()+setSleepTime(), msg);
    }
}


void Ind_ONOFF_Source::initSpeedData()
{
    // reading data for source speed probability distribution
    cStringTokenizer tok(gDEBP(doc,"//SPEED-DISTRIBUTION"));  
    dstS = tok.asDoubleVector();
   
    // removing zero values from end of vector, if any
    int i;
    for(i=(int)dstS.size()-1; i>0 ;i--) if(dstS[i]!=0) break;
    dstS.resize(i+1);
}

int Ind_ONOFF_Source::setPacketSize()
{
   // pktsize random value generation
   int i;
   double s=0, x=uniform(0,1);
   for (i=0; i<(int)dstS.size(); i++) if( (s+=dstS[i])>x ) break;  
   return i*cellsize;
}


void Ind_ONOFF_Source::initActiveData()
{
    // preparing data for timeleft calculation
    cStringTokenizer tok(gDEBP(doc,"/ACF-SHAPE-PARAMETERS"));
    pA = tok.asDoubleVector();
    double alpha= pA[0];  
    double px   = pA[1];
    double py   = pA[2];
    
    int i;
    double x,s;
    for(s=py, i=2; (x=pow( px+i,-alpha ))/s > 1.0e-9; i++) s+=x;
    pA.resize(5);
    pA[3]=(1-py)/(s-py);   // A - normalizing constant of probability distributon
    pA[4]=i+0.5;           // imax - maximal lenght of active period (instead infinity)
    cpfA.resize(100);      // first values of cumulative prob fun of active period
    for(cpfA[0]=0, cpfA[1]=py, i=2; i<100; i++) 
       cpfA[i]=cpfA[i-1]+pA[3]*pow(px+i,-alpha); 
}


simtime_t Ind_ONOFF_Source::setActiveTime()
{
    // timeleft random value generation

    // reading active time distribution parameters
    double alpha=pA[0], px=pA[1], A=pA[3], imax=pA[4];  // py=pA[2]
    
    int i;
    double x=uniform(0,1);
    for(i=1; x>=cpfA[i] && i<(int)cpfA.size(); i++);
    if (i<(int)cpfA.size()) return timeslot*i;
    else {   // calcuation for big active time values
       int k;
       double s; 
       for(s=cpfA[i-1], k=i; x>=s && k<imax ; k++) s+=A*pow(px+k,-alpha); 
       return timeslot*(k-1);
	}
}


void Ind_ONOFF_Source::initSleepData()
{  
   cStringTokenizer tok(gDEBP(doc,"//SLEEP-PARAMETERS"));
   pB=tok.asDoubleVector();
}

simtime_t Ind_ONOFF_Source::setSleepTime()
{
   return timeslot*poisson(pB[0]); 
}



/* simple test
void Ind_ONOFF_Source::initSpeedData()
{
}

void Ind_ONOFF_Source::setPacketSize()
{
   pktsize = cellsize;
}

void Ind_ONOFF_Source::initActiveData()
{
}

void Ind_ONOFF_Source::setActivTime()
{
   timeleft = timeslot * intuniform(1,9);
}

void Ind_ONOFF_Source::initSleepData()
{
}

void Ind_ONOFF_Source::setSleepTime()
{
   sleeptime = timeslot * intuniform(0,6);
}
*/

