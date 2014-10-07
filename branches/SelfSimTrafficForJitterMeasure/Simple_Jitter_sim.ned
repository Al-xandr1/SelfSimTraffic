import Common_Modules.*;
import Statistics.*;
import MGI_Source.*;
import FromFile_Source.*;

//!!!ONLY FOR CONSTRUCT PICTURE!!! NOT FOR EXECUTING!!!
network Simple_Jitter_sim
{
    parameters:
        bool simulateMGI = default(true);
        double channelDatarate @unit(Mbps) = default(3 Mbps);

    submodules:
        tag: Simple_CBR_Source {

            @display("p=34,58");
        }
        queue: TxQueue {

            @display("p=137,58");
        }
        stat: Traffic_Stats {

            @display("p=137,172");
        }
        srcFF: FromFile_Source if !simulateMGI {

            @display("p=67,172");
        }
        srcMGI: MGI_Source if simulateMGI {

            @display("p=206,172");
        }
        jitt: Jitter_Meter {

            @display("p=221,58");
        }
        jittBuffer: JitterBuffer {

            @display("p=317,58");
        }
        smoothedJitt: Jitter_Meter {

            @display("p=397,58");
        }
        dst: Sink {

            @display("p=397,172");
        }
    connections:
        tag.out --> queue.in[0];

        srcFF.out --> stat.in if !simulateMGI;

        srcMGI.out --> stat.in if simulateMGI;

        stat.out --> queue.in[1];

        queue.out --> {  datarate = channelDatarate; } --> jitt.in;
        jitt.out[0] --> jittBuffer.in;
        jitt.out[1] --> dst.in;

        jittBuffer.out --> smoothedJitt.in;

        smoothedJitt.out[0] --> dst.in;
}
