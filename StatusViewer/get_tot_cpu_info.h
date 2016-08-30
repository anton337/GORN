#include "stdlib.h"
#include "stdio.h"
#include <string>

static unsigned long long * lastTotalUser    , 
                          * lastTotalUserLow , 
                          * lastTotalSys     , 
                          * lastTotalIdle    ;
static unsigned long long * totalUser        , 
                          * totalUserLow     , 
                          * totalSys         , 
                          * totalIdle        , 
                          * total            ;

int num_cpu = 4 + 1;

void init()
{
    lastTotalUser       = new unsigned long long [num_cpu];
    lastTotalUserLow    = new unsigned long long [num_cpu];
    lastTotalSys        = new unsigned long long [num_cpu];
    lastTotalIdle       = new unsigned long long [num_cpu];

    totalUser           = new unsigned long long [num_cpu];
    totalUserLow        = new unsigned long long [num_cpu];
    totalSys            = new unsigned long long [num_cpu];
    totalIdle           = new unsigned long long [num_cpu];
    total               = new unsigned long long [num_cpu];
}

void getCurrentValue( double * percent )
{
    FILE* file;
    unsigned long long dummy;
    file = fopen("/proc/stat", "r");
    fscanf(file, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu\n", &totalUser[0], &totalUserLow[0], &totalSys[0], &totalIdle[0], &dummy, &dummy, &dummy, &dummy, &dummy, &dummy);
    fscanf(file, "cpu0 %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu\n", &totalUser[1], &totalUserLow[1], &totalSys[1], &totalIdle[1], &dummy, &dummy, &dummy, &dummy, &dummy, &dummy);
    fscanf(file, "cpu1 %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu\n", &totalUser[2], &totalUserLow[2], &totalSys[2], &totalIdle[2], &dummy, &dummy, &dummy, &dummy, &dummy, &dummy);
    fscanf(file, "cpu2 %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu\n", &totalUser[3], &totalUserLow[3], &totalSys[3], &totalIdle[3], &dummy, &dummy, &dummy, &dummy, &dummy, &dummy);
    fscanf(file, "cpu3 %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu\n", &totalUser[4], &totalUserLow[4], &totalSys[4], &totalIdle[4], &dummy, &dummy, &dummy, &dummy, &dummy, &dummy);
    fclose(file);

    for ( int k(0)
        ; k < num_cpu
        ; ++k
        )
    {
        if (totalUser[k] < lastTotalUser[k] || totalUserLow[k] < lastTotalUserLow[k] ||
            totalSys[k] < lastTotalSys[k] || totalIdle[k] < lastTotalIdle[k]){
            //Overflow detection. Just skip this value.
            percent[k] = -1.0;
        }
        else{
            total[k] = (totalUser[k] - lastTotalUser[k]) + (totalUserLow[k] - lastTotalUserLow[k]) +
                (totalSys[k] - lastTotalSys[k]);
            percent[k] = total[k];
            total[k] += (totalIdle[k] - lastTotalIdle[k]);
            percent[k] /= total[k];
            percent[k] *= 100;
        }

        lastTotalUser[k] = totalUser[k];
        lastTotalUserLow[k] = totalUserLow[k];
        lastTotalSys[k] = totalSys[k];
        lastTotalIdle[k] = totalIdle[k];
    }
}
