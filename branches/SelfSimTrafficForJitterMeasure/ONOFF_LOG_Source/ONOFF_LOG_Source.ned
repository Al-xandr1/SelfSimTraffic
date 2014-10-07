// Source of ON-OFF type of selfsimilar traffic. 
// Parameters of generated traffic are given in xml file. 
// Also there is possibility to calculate these parameters from 
// discrete traffic statistics, which this ON-OFF Source 
// will try to simulate.

package ONOFF_LOG_Source;

simple ONOFF_LOG_Par
{
    parameters:
        xml data = default(xmldoc("traffic_statistics.xml"));
        string profile = default("onoff_log_profile.xml");
}

simple Ind_ONOFF_LOG_Source
{
    parameters:
        xml profile = default(xmldoc("onoff_log_profile.xml"));  
}


simple ONOFF_LOG_Collector 
{
    gates:
        input  in @directIn;
        output out;
}    

module ONOFF_LOG_Source
{
    parameters: 
        int  numSources = default(2);
        bool calcRequired = default(true); 
    gates:
        output out;
    submodules:
        calc[calcRequired ? 1:0] : ONOFF_LOG_Par;
        src[calcRequired ? 0:numSources] : Ind_ONOFF_LOG_Source;
        collect  : ONOFF_LOG_Collector;
    connections:
        collect.out --> out;       
}