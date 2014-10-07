// Node to measure jitter of tagged packet stream going through it. 
// Tagged stream is designated by given value of "Kind" packet property.
// The node collects jitter data into histogramm and saves it to 
// OMNET++ histogramm file.

#include <omnetpp.h>
#include <math.h>
#include <fstream>
#include <string>
#include <vector>
#include <deque>
#include "DevelopmentHelper.cc"

class Jitter_Meter : public cSimpleModule {

  private:
    void collectStatistics();

  protected:
    // Basic parameters
    int tagKind;
    
    int numberOfCompoundQueues;

    simtime_t timeThresholdForStatistics;

    simtime_t firstDelay;
    simtime_t intertime;
    int jitterBufferSize;
    bool withResetCounter;

    // Field for PDF calculation
    cDoubleHistogram jittHist;
    int numHistCells;
    double leftBound, rightBound;
    simtime_t last_arr;

    // For threshold probability of jitter
    double thresholdProbabilityTheory;

    // Fields for ACF calculation
    unsigned int num_of_points;
    std::vector<cStdDev> covar;
    std::deque<simtime_t> cashedJittValues;

    // The following redefined virtual function holds the algorithm.
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

// The module class needs to be registered with OMNeT++
Define_Module(Jitter_Meter);

void Jitter_Meter::initialize() {
    tagKind = par("tagKind");

    numberOfCompoundQueues = par("numberOfCompoundQueues_");

    // For PDF calculation
    timeThresholdForStatistics = par("timeThresholdForStatistics_");
    numHistCells = par("numHistCells");
    firstDelay = par("firstDelay");
    intertime = par("intertime");
    jitterBufferSize = par("jitterBufferSize_");
    withResetCounter = par("withResetCounter");

    leftBound = par("leftBoundJitterHist");
    rightBound = par("rightBoundJitterHist");

    jittHist.setNumCells(numHistCells);
    jittHist.setRange(leftBound, rightBound);
    last_arr = -1;

    // For threshold probability of jitter
    thresholdProbabilityTheory = par("thresholdProbabilityTheory");

    // For ACF calculation
    num_of_points = (unsigned int) par("num_of_points");
    covar.resize(num_of_points);

    EV << "Jitter_Meter: jitter histogram range = [" << leftBound << ", "
            << rightBound << "], widthOfCell = " << (rightBound - leftBound)/numHistCells << endl;
}

void Jitter_Meter::handleMessage(cMessage *msg) {
    if (msg->getKind() == tagKind) {

        if (simTime() >= timeThresholdForStatistics){
            collectStatistics();
        }

        send(msg, "out", 0);
    } else {
        if (gateSize("out") > 1) {
            send(msg,"out", 1);
        } else {
            EV << "WARNING!!! in case of gate size =< 1  arrived packet:" << msg->getKind() << ", " << msg->getName() <<endl;
            delete msg;
        }
    }
}

void Jitter_Meter::collectStatistics() {
    if (last_arr >= 0) {
        simtime_t jittValue = ((simTime()-last_arr) - intertime) / intertime;

        // for PDF calculation
        jittHist.collect(jittValue);

        // for ACF calculation
        cashedJittValues.push_back(jittValue);
        if (cashedJittValues.size() == num_of_points) {
            for(unsigned int i=0; i < num_of_points; i++) {
                covar[i].collect(cashedJittValues[0].dbl() * cashedJittValues[i].dbl());
            }
            cashedJittValues.pop_front();
        }
    }

    last_arr = simTime();
}

void Jitter_Meter::finish() {
    int i;

    char outFileName[256];
    DevelopmentHelper *helper = new DevelopmentHelper();
    std::ofstream out( helper->createFileName(outFileName, par("numberOfExperiment_"), par("pdfFileName").stringValue(), (int)par("fileSuffix")) );
    out << "<?xml version=\'1.0' ?>" <<endl<<endl;
    out << "<JITTER-STATISTICS>" <<endl;
    out << "  <TIME-THRESHOLD-FOR-STATISTICS>" << timeThresholdForStatistics << "</TIME-THRESHOLD-FOR-STATISTICS>" <<endl;
    if (numberOfCompoundQueues > 0)
        out << "  <NUMBER-OF-COMPOUND-QUEUES>" << numberOfCompoundQueues << "</NUMBER-OF-COMPOUND-QUEUES>" <<endl;

    out << "  <JITTER-BUFFER-PARAMETERS>" <<endl;
    if (jitterBufferSize > 0) out << "    <BUFFER-SIZE>" << jitterBufferSize << "</BUFFER-SIZE>" <<endl;
    if (jitterBufferSize > 0) out << "    <WITH-COUNTER-RESET>" << withResetCounter << "</WITH-COUNTER-RESET>" <<endl;
    if (firstDelay > 0)       out << "    <FIRST-DELAY>" << firstDelay << "</FIRST-DELAY>" <<endl;
    out << "    <INTER-TIME>" << intertime << "</INTER-TIME>" <<endl;
    if (firstDelay > 0)       out << "    <FIRSTDELAY-DIV-INTERTIME>" << firstDelay / intertime << "</FIRSTDELAY-DIV-INTERTIME>" <<endl;
    out << "  </JITTER-BUFFER-PARAMETERS>" <<endl;

    out << "  <RANGE>" << leftBound << "  " << rightBound << "</RANGE>" <<endl;
    out << "  <NUM-HIST-CELLS>" << numHistCells << "</NUM-HIST-CELLS>" <<endl;
    out << "  <WIDTH-OF-CELL>" << (rightBound - leftBound) / numHistCells << "</WIDTH-OF-CELL>" <<endl;
    out << "  <JITTER-MEAN>" << jittHist.getMean() << "</JITTER-MEAN>" <<endl;
    out << "  <JITTER-VARIANCE>" << jittHist.getVariance() << "</JITTER-VARIANCE>" <<endl<<endl;


    out << "  <CELL-CENTER-POINTS>";
    for (i=0; i<jittHist.getNumCells(); i++)
       out << 0.5*(jittHist.getBasepoint(i)+jittHist.getBasepoint(i+1)) <<"  ";
    out << "</CELL-CENTER-POINTS>" <<endl;


    out << "  <CELL-VALUES>";
    for (i=0; i<jittHist.getNumCells(); i++)
       out << static_cast<long>(jittHist.getCellValue(i)) <<"  ";
    out << "</CELL-VALUES>" <<endl;


    long n = jittHist.getCount();
    out << "  <COUNTER>" << n << "</COUNTER>" <<endl;
    out << "  <PDF-VALUES>";
    for (i=0; i<jittHist.getNumCells(); i++){
       out<<(double)jittHist.getCellValue(i)/n<<"  ";
    }
    out << "</PDF-VALUES>" <<endl;
    out << "  <OUT-OF-RANGE> " << jittHist.getUnderflowCell() << "  " << jittHist.getOverflowCell() << " </OUT-OF-RANGE>" <<endl;


    out << "  <THRESHOLD-PROBABILITY-INFO> " <<endl;
    out << "    <THEORY-PROB> " <<  thresholdProbabilityTheory << " </THEORY-PROB>" <<endl;
    double realThresholdProb = static_cast<double>(jittHist.getOverflowCell()) / static_cast<double>(n);
    double boundaryByProbability = rightBound;
    if (realThresholdProb < thresholdProbabilityTheory) {
        for (i = jittHist.getNumCells()-1; i >= 0; i--){
             realThresholdProb += ((double)jittHist.getCellValue(i)/n);
             if (realThresholdProb > thresholdProbabilityTheory) {
                realThresholdProb -= ((double)jittHist.getCellValue(i)/n);
                //todo center of cell VS right bound???
                boundaryByProbability = 0.5*(jittHist.getBasepoint(i)+jittHist.getBasepoint(i+1));
                break;
             }
        }
    }
    out << "    <REAL-PROB> " <<  realThresholdProb << " </REAL-PROB>" <<endl;
    out << "    <BOUNDARY-BY-PROB> " <<  boundaryByProbability << " </BOUNDARY-BY-PROB>" <<endl; // allowed jitter by threshold of probability
    out << "  </THRESHOLD-PROBABILITY-INFO> " <<endl;


    cDensityEstBase::Cell cellWithZero;
    for (i = 0; i < jittHist.getNumCells(); i++) {
        cellWithZero = jittHist.getCellInfo(i);
        if (cellWithZero.lower <= 0 && 0 < cellWithZero.upper) break; //find zero point - point with packet's smoothed jitter
    }
    out << "  <VALUE-OF-CELL-WITH-ZERO> " << static_cast<long>(cellWithZero.value) << " </VALUE-OF-CELL-WITH-ZERO>" <<endl;
    out << "  <PROBABILITY-OF-UNSMOOTHED-JITTER> " << (static_cast<double>(n) - cellWithZero.value) / static_cast<double>(n) << " </PROBABILITY-OF-UNSMOOTHED-JITTER>" <<endl;


    out << "  <NORM-ACF>" <<endl;
    out << "    <ACF-RANGE>" << num_of_points << "</ACF-RANGE>" <<endl;
    out << "    <ACF-VALUES>";
    double ACF0 = covar[0].getMean() - jittHist.getMean()*jittHist.getMean();
    for (i=0; i < (int) num_of_points; i++) {
        out << ((covar[i].getMean() - jittHist.getMean()*jittHist.getMean())/ACF0) <<"  ";
    }
    out << "</ACF-VALUES>" <<endl;
    out << "  </NORM-ACF>" <<endl;

    out << "</JITTER-STATISTICS>" <<endl;
    out.close();
}
