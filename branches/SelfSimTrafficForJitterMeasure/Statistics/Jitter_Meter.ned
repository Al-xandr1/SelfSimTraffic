// Node to measure jitter of tagged packet stream going through it. 
// Tagged stream is designated by given value of "Kind" packet property.
// The node collects jitter data into histogramm and saves it to 
// OMNET++ histogramm file.

package Statistics;

simple Jitter_Meter
{
   parameters:
      int numberOfExperiment_ = default(-1);
        
      int tagKind = default(0); 									// now tag 0 for the deterministic traffic
      int outSize = default(2);										// permitted values 1 & 2
      string histFileName = default("Jitter.stt");
      string pdfFileName = default("Jitter");
      int fileSuffix = default(-1);
      
      int numberOfCompoundQueues_ = default(-1);
      
      // for jitter distribution histogram collection
      double timeThresholdForStatistics_ @unit(s) = default(30 s);	// time threoshold, after that begin statistical acounting (inclding this moment)
      double firstDelay @unit(s) = default(-1 s);   
      double intertime @unit(s) = default(0.025 ms);
      int jitterBufferSize_ = default(-1);
      bool withResetCounter = default(true);  	
      			
      int numHistCells = default(400);  						
      double leftBoundJitterHist = default(-10);			
      double rightBoundJitterHist @unit(s)= default(10 s);
      
      // for threshold probability of jitter
      double thresholdProbabilityTheory = default(0.00001);     
      
      // for ACF calculation
      int num_of_points = default(50);
           
   gates:
      input in;
      output out[outSize]; 
}