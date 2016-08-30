#include "get_tot_cpu_info.h"
#include "memory.h"
#include <unistd.h>
#include <iostream>
#include <string>

int main()
{
    init();
    double percent[5];
    while (1)
    {
        memoryInfo c_memoryInfo;
        getCurrentValue(percent);
        for ( std::size_t k(0)
            ; k < 5
            ; ++k
            )
        {
            std::cout << percent[k] << "\t";
        }
        // std::cout << c_memoryInfo.virtualMemUsed << "\t";
        // std::cout << c_memoryInfo.totalVirtualMem << "\t";
        // std::cout << c_memoryInfo.physMemUsed /(1024*1024) << "MB\t";
        // std::cout << c_memoryInfo.totalPhysMem/(1024*1024) << "MB\t";
        // std::cout << 100*(float)c_memoryInfo.physMemUsed/(float)c_memoryInfo.totalPhysMem << "%\t";
        std::size_t mem_total;
        std::size_t mem_used;
        std::size_t mem_cache;
        std::size_t mem_buffers;
        c_memoryInfo . get_memory_info ( mem_total 
                                       , mem_used
                                       , mem_cache
                                       , mem_buffers
                                       );
        std::cout << mem_used / (1024) << "MB\t";
        std::cout << mem_buffers / (1024) << "MB\t";
        std::cout << mem_cache / (1024) << "MB\t";
        std::cout << mem_total / (1024) << "MB\t";
        std::cout << 100*(float)mem_used/(float)mem_total << "%\t";
        std::cout << std::endl;
        sleep(1);
    }
    return 0;
}

