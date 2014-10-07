import Common_Modules.*;
import MGI_Source.*;
import Statistics.*;

network MGI_Traffic_sim
{
    submodules:
        tag:   Simple_CBR_Source;
        src:   MGI_Source;
        stat:  Traffic_Stats;
        queue: TxQueue { insize = 2;}
        jitt:  Jitter_Meter;
        dst:   Sink;
    connections:
        src.out --> stat.in;
        stat.out  --> queue.in[0];
        tag.out   --> queue.in[1];
        queue.out --> { datarate = 3 Mbps; } --> jitt.in;
        jitt.out  --> dst.in;
}

