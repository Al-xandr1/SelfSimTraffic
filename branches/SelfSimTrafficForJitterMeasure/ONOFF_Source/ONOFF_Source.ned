// Source of ON-OFF type of selfsimilar traffic. 
// Parameters of generated traffic are given in xml file. 
// Also there is possibility to calculate these parameters from 
// discrete traffic statistics, which this ON-OFF Source 
// will try to simulate.

package ONOFF_Source;

simple ONOFF_Parameters_Calculator
{
    parameters:
        xml data = default(xmldoc("traffic_statistics.xml"));
        string profile = default("onoff_profile.xml");
}

simple Ind_ONOFF_Source
{
    parameters:
        xml profile = default(xmldoc("onoff_profile.xml"));  
}


simple ONOFF_Collector 
{
    gates:
        input  in @directIn;
        output out;
}    

module ONOFF_Source
{
    parameters: 
        int  numSources = default(2);
        bool calcRequired = default(true); 
    gates:
        output out;
    submodules:
        calc[calcRequired ? 1:0] : ONOFF_Parameters_Calculator;
        src[calcRequired ? 0:numSources] : Ind_ONOFF_Source;
        collect  : ONOFF_Collector;
    connections:
        collect.out --> out;       
}