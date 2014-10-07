import Common_Modules.*;
import ONOFF_Source.*;
import Statistics.*;

network ONOFF_Traffic_sim
{
    parameters:
        double channelDatarate @unit(Mbps) = default(3 Mbps); // Mbits per second; 0=infinite
        
    submodules:
        tag:   Simple_CBR_Source;
        src:   ONOFF_Source;
        stat:  Traffic_Stats;
        queue: TxQueue { insize = 2;}
        jitt:  Jitter_Meter;
        dst:   Sink;
        
    connections:
        src.out   --> stat.in;
        stat.out  --> queue.in[0];
        tag.out   --> queue.in[1];
        queue.out --> { datarate = channelDatarate; } --> jitt.in;
        jitt.out  --> dst.in;
}

