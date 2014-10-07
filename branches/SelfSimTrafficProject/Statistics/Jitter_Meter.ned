// Node to measure jitter of tagged packet stream going through it. 
// Tagged stream is designated by given value of "Kind" packet property.
// The node collects jitter data into histogramm and saves it to 
// OMNET++ histogramm file.

package Statistics;

simple Jitter_Meter
{
   parameters:
      int tagKind = default(1);
      string histFileName = default("Jitter.stt");
      string pdfFileName = default("Jitter.xml");
      
      int numHistCells = default(200);
      double maxAccountingJitter @unit(s) = default(0.5 s);
   gates:
      input in;
      output out; 
}