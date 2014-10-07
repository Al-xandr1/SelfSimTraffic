// Simplest constant bit rate source. It generates packets (cells) with 
// constant time between them. Size of packets (cells) is also constant.
// Flow packet intensity should be constant in a series of experiment 

package Common_Modules;

simple Simple_CBR_Source
{
     parameters:
         double _intertime @unit(s) = default(0.025 ms);
         double flowIntensity @unit(bps) = default(320000 bps);
     gates:
         output out;
}