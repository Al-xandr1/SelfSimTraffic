// FIFO Queue of packets for transmission through output channel. 
// If flag "taging" is set, property "Kind" of each transmitted packet 
// will be set to the number of gate, from which this packet arrived to
// the queue. 

package Common_Modules;

simple TxQueue
{
   parameters:
      int numberOfExperiment_ = default(-1);
       
      int insize = default(2);
      bool taging = default(true);
         
      double timeThresholdForStatistics_ @unit(s) = default(30 s);	// time threoshold, after that begin statistical acounting (inclding this moment)
      int fileSuffix = default(-1);
      string queueSizeGraphicName = default("QueueSize");
      double graphicTimeSlot @unit(s)= default(1 s); 				// minimum time slot for the size graphic of queues
   gates:
      input  in[insize];
      output out;
}

