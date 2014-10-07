
#include <string>
#include <vector>
#include <math.h>
#include <list>
#include <sstream>
#include <string>
#include <omnetpp.h>
using namespace std;

class DevelopmentHelper{

    public:
        char* createFileName(char* buffer, int numberOfExperiment, const char* rawName, int index){
            ostringstream prefix, postfix;
            char *result;

            if (numberOfExperiment > 0){
                prefix << numberOfExperiment;

                result = strcat(strcat(strcpy(buffer, prefix.str().c_str()), "_"), rawName);
            } else {
                result = strcpy(buffer,  rawName);
            }

            if (index >= 0) {
                postfix << index;

                result = strcat(strcat(strcat(result, "_"), ((postfix.str()).c_str())), ".xml");
            } else {
                result = strcat(result, ".xml");
            }

            return result;
        };

        int countMaxValue(list<int> queueSizePoints){
            int maxQueueSize = 0;
            list<int>::iterator iterSize;
            for (iterSize = queueSizePoints.begin(); iterSize != queueSizePoints.end(); ++iterSize){
                if (maxQueueSize < (*iterSize))  maxQueueSize = (*iterSize);
            }
            return maxQueueSize;
        }
};
