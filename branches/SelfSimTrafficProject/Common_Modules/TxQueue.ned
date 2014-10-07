// FIFO Queue of packets for transmission through output channel. 
// If flag "taging" is set, property "Kind" of each transmitted packet 
// will be set to the number of gate, from which this packet arrived to
// the queue. 

package Common_Modules;

simple TxQueue
{
   parameters:
      int insize = default(2);
      bool taging = default(true);
   gates:
      input  in[insize];
      output out;
}

