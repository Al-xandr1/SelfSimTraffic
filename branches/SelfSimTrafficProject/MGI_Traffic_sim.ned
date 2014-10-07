import Common_Modules.*;
import MGI_Source.*;
import Statistics.*;

network MGI_Traffic_sim
{
    parameters:        
        double channelDatarate @unit(Mbps) = default(3 Mbps); // Mbits per second; 0=infinite
    	
    @display("bgb=365,244");
    submodules:
        tag: Simple_CBR_Source {
            @display("p=173,208");
        }
        src: MGI_Source {
            @display("p=34,42");
        }
        stat: Traffic_Stats {
            @display("p=99,81");
        }
        queue: TxQueue {
            insize = 2;
            @display("p=173,123");
        }
        jitt: Jitter_Meter {
            @display("p=246,81");
        }
        dst: Sink {
            @display("p=308,49");
        }
        
    connections:
        src.out   --> stat.in;
        stat.out  --> queue.in[0];
        tag.out   --> queue.in[1];
        queue.out --> {  datarate = channelDatarate; } --> jitt.in;
        jitt.out  --> dst.in;
}

