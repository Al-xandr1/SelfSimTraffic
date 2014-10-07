import Common_Modules.*;
import MGI_Source.*;
import FromFile_Source.*;
import Statistics.*;

module CompoundQueue
{
    parameters:
        int _numberOfExperiment = default(-1);
        
        bool simulateMGI = default(true); 								// make choice beetwen FromFile and MGI sources
        int ownIndex = default(-1);
        double _sDelay @unit(s) = default(0 s);

        double _timeThresholdForStatistics @unit(s) = default(30 s);	// time threoshold, after that begin statistical acounting (inclding this moment)    
        double _intertime @unit(s) = default(0.025 ms); 				// time between packets of deterministic flow

        bool statEnabled = default(false); 								// turn on/off statistic module
        double channelDatarate @unit(Mbps) = default(3 Mbps); 			// Mbits per second; 0=infinite

        @display("bgl=39;bgb=376,275");
    gates:
        input in;
        output out;

    submodules:
        srcFF: FromFile_Source if !simulateMGI {
            @display("p=34,218");
        }

        srcMGI: MGI_Source if simulateMGI {
            @display("p=202,218");
        }

        stat: Traffic_Stats if statEnabled {
            numberOfExperiment_ = _numberOfExperiment;
            
            fileSuffix = ownIndex;
            @display("p=119,218");
        }

        queue: TxQueue {
            numberOfExperiment_ = _numberOfExperiment;
            
            insize = 2;
            timeThresholdForStatistics_ = _timeThresholdForStatistics;
            fileSuffix = ownIndex;
            @display("p=119,120");
        }

        jitt: Jitter_Meter {
            numberOfExperiment_ = _numberOfExperiment;
            
            outSize = 2;
            timeThresholdForStatistics_ = _timeThresholdForStatistics;
            intertime = _intertime;
            fileSuffix = ownIndex;
            @display("p=290,120");
        }

        localDst: Sink {
            @display("p=290,221");
        }

    connections allowunconnected:
        in --> queue.in[0]; 							// through this gate arrives deterministic traffic

        // connect FromFile_Source if neened
        if !simulateMGI {
            srcFF.out --> stat.in if statEnabled;
            stat.out  --> queue.in[1] if statEnabled; 	// through this queue's gate arrives selfsimilar traffic
            srcFF.out --> queue.in[1] if !statEnabled;
        }

        // connect MGI_Source if neened
        if simulateMGI {
            srcMGI.out --> stat.in if statEnabled;
            stat.out   --> queue.in[1] if statEnabled; 	// through this queue's gate arrives selfsimilar traffic
            srcMGI.out --> queue.in[1] if !statEnabled;
        }

        queue.out 	--> {  datarate = channelDatarate; } --> jitt.in;
        jitt.out[0] --> out; 			// deterministic traffic comes out from this module
        jitt.out[1] --> localDst.in; 	// selfsimilar traffic destroying here
}
