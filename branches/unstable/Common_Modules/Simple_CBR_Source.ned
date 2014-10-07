// Simplest constant bit rate source. It generates packets (cells) with 
// constant time between them. Size of packets (cells) is also constant.

package Common_Modules;

simple Simple_CBR_Source
{
     parameters:
         double intertime @unit(s) = default(25ms);
         int bytesize @unit(byte) = default(64B);
     gates:
         output out;
}