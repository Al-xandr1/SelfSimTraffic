// Simplest constant bit rate source. It generates packets (cells) with 
// constant time between them. Size of packets (cells) is also constant.
// Flow packet intensity should be constant in a series of experiment 

package Common_Modules;

simple Simple_CBR_Source
{
     parameters:
         double intertime @unit(s) = default(1 ms);
         double flowIntensity @unit(bps) = default(20480 bps);
     gates:
         output out;
}