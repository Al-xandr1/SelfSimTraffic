import Common_Modules.*;
import Statistics.*;

network Compound_Jitter_sim
{
    parameters:
        int numberOfExperiment = default(-1);
        
        int numberOfCompoundQueues = default(1);
        double timeThresholdForStatistics @unit(s) = default(30 s);	// time threoshold, after that begin statistical acounting (inclding this moment)
        
        int jitterBufferSize = default(-1);							// size of jitter buffer
        bool jitterWithResetCounter = default(true);				// for different mode of JitterBuffer
        
        double firstDelayBuffer @unit(s) = default(0.03 ms);		// first delay for JitterBuffer and for Jitter_Metter
        double intertime_ @unit(s) = default(0.025 ms); 			// time between packets of deterministic flow
        double sDelay @unit(s) = default(0 s);						// delay between moduls for correct work synchronisation algorithm while parallel execution

        @display("bgb=597,114");
    submodules:
        tag: Simple_CBR_Source {
            _intertime = intertime_;
            @display("p=31,46");
        }


        compQueue[numberOfCompoundQueues]: CompoundQueue {
            _numberOfExperiment = numberOfExperiment;
            
            _intertime = intertime_;
            ownIndex = index;
            _sDelay = (sDelay * (index + 1));
            _timeThresholdForStatistics = timeThresholdForStatistics;
            @display("p=195,46");
        }


        jittBuffer: JitterBuffer {
        	numberOfExperiment_ = numberOfExperiment;
        	
            numberOfCompoundQueues_ = numberOfCompoundQueues;
            
            bufferSize = jitterBufferSize;
            needResetCounter = jitterWithResetCounter;
            
            firstDelay = firstDelayBuffer;
            timeThresholdForStatistics_ = timeThresholdForStatistics;
            @display("p=359,46");
        }


        smoothedJitt: Jitter_Meter {
        	numberOfExperiment_ = numberOfExperiment;
        	
            outSize = 1;
            numberOfCompoundQueues_ = numberOfCompoundQueues;

            jitterBufferSize_ = jitterBufferSize;
            withResetCounter = jitterWithResetCounter;
            
            firstDelay = firstDelayBuffer;
            intertime = intertime_;
            timeThresholdForStatistics_ = timeThresholdForStatistics;
            @display("p=457,46");
        }


        dst: Sink {
            @display("p=547,46");
        }

    connections:
        for s=0..numberOfCompoundQueues-1 {
            tag.out --> 		 {delay = sDelay;} --> compQueue[s].in 	 if s==0;
            compQueue[s].out --> {delay = sDelay;} --> compQueue[s+1].in if s<=numberOfCompoundQueues-2;
            compQueue[s].out --> {delay = sDelay;} --> jittBuffer.in 	 if s==numberOfCompoundQueues-1;
        }

        jittBuffer.out --> smoothedJitt.in;
        smoothedJitt.out[0] --> dst.in;
}
