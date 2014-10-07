// JitterBuffer is used for smoothing jitter of deterministic traffic
//

package Statistics;

simple JitterBuffer
{
	parameters:
        int numberOfExperiment_ = default(-1);
	    
        int tagKind = default(0); 							// this tag 0 for the accountable deterministic traffic
        
        int bufferSize = default(-1);						// maximal size of buffer for storing packets
        bool needResetCounter = default(true);				// indicate needing for reset planning or not
        
	    double firstDelay @unit(s) = default(0.03 ms);		// delay time of first packet
	    double intertime @unit(s) = default(0.025 ms);  	// time between packets
	    
	    int numberOfCompoundQueues_ = default(1);
	    	  
        double timeThresholdForStatistics_ @unit(s) = default(30 s);	// time threoshold, after that begin statistical acounting (inclding this moment)
        string bufferSizeGraphicName = default("BufferSize");
        double graphicTimeSlot @unit(s) = default(1 s); 				// minimum time slot for the size graphic of queues
        
	gates:
	    input in;
	    output out;
}