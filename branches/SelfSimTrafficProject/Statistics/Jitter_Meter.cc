// Node to measure jitter of tagged packet stream going through it. 
// Tagged stream is designated by given value of "Kind" packet property.
// The node collects jitter data into histogramm and saves it to 
// OMNET++ histogramm file.

#include <omnetpp.h>
#include <math.h>
#include <fstream>
#include <string>
#include <vector>

class Jitter_Meter : public cSimpleModule
{

  protected:
    // Basic parameters
    int tagKind;
    
    // Field for PDF calculation
    cDoubleHistogram jitHist;
    int numHistCells;
    double leftBound, rightBound;
    simtime_t last_arr;
    
    // The following redefined virtual function holds the algorithm.
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

// The module class needs to be registered with OMNeT++
Define_Module(Jitter_Meter);

void Jitter_Meter::initialize()
{
    // Initialize is called at the beginning of the simulation.
    tagKind = par("tagKind");
    numHistCells = par("numHistCells");
    rightBound = par("maxAccountingJitter");

    leftBound = -0.0005000;

    jitHist.setNumCells(numHistCells);
    jitHist.setRange(leftBound, rightBound);
    last_arr = -1;

    EV << "Jitter_Meter: jitter histogram range = [" << leftBound << ", "
            << rightBound << "], widthOfCell = " << (rightBound - leftBound)/numHistCells;
}

void Jitter_Meter::handleMessage(cMessage *msg)
{
    // The handleMessage() method is called whenever a message arrives
    // at the module.
     
    if ( msg->getKind() == tagKind ) {
       if( last_arr >= 0 ) jitHist.collect(simTime()-last_arr);
       last_arr = simTime();
    }
    send(msg,"out");
}

void Jitter_Meter::finish()
{    
    int i;
    // FILE *histOut = fopen(par("histFileName"),"w");
    
    // jitHist.saveToFile(histOut);
   
    // fclose(histOut);
    
    std::ofstream out( par("pdfFileName").stringValue() );
    out << "<?xml version=\'1.0' ?>" <<endl<<endl;
    out << "<JITTERDISTRIBUTION>" <<endl;
    out << "<RANGE>" << leftBound << "  " << rightBound << "</RANGE>" <<endl;
    out << "<NUM-HIST-CELLS>" << numHistCells << "</NUM-HIST-CELLS>" <<endl;
    out << "<WIDTH-OF-CELL>" << (rightBound - leftBound) / numHistCells << "</WIDTH-OF-CELL>" <<endl<<endl;

    out << "<CELLCENTERPOINTS>" <<endl;
    for (i=0; i<jitHist.getNumCells(); i++)
       out << 0.5*(jitHist.getBasepoint(i)+jitHist.getBasepoint(i+1)) <<"  ";
    out << "</CELLCENTERPOINTS>" <<endl;

    out << "<CELLVALUES>" <<endl;
    for (i=0; i<jitHist.getNumCells(); i++)
       out<<jitHist.getCellValue(i)<<"  ";
    out << "</CELLVALUES>" <<endl;

    long n = jitHist.getCount();
    out << "<COUNTER>  " << n << "</COUNTER>" <<endl;
    out << "<PDFVALUES>" <<endl;
    for (i=0; i<jitHist.getNumCells(); i++)
       out<<(double)jitHist.getCellValue(i)/n<<"  ";
    out << "</PDFVALUES>" <<endl;

    out << " <OUT-OF-RANGE> " << jitHist.getOverflowCell()+jitHist.getUnderflowCell() << " </OUT-OF-RANGE>" <<endl;
    out << "</JITTERDISTRIBUTION>" <<endl;
    out.close();
}
