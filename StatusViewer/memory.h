#include <sys/types.h>
#include <sys/sysinfo.h>
#include <fstream>
#include <sstream>

struct memoryInfo
{

    struct sysinfo memInfo;

    long long totalVirtualMem;
    
    long long virtualMemUsed;
    
    long long totalPhysMem;
    
    long long physMemUsed;
    
    memoryInfo()
    {
        sysinfo (&memInfo);
        
        // Total Virtual Memory
        totalVirtualMem = memInfo.totalram;
        //Add other values in next statement to avoid int overflow on right hand side...
        totalVirtualMem += memInfo.totalswap;
        totalVirtualMem *= memInfo.mem_unit;
        
        // Virtual Memory currently used
        virtualMemUsed = memInfo.totalram - memInfo.freeram;
        //Add other values in next statement to avoid int overflow on right hand side...
        virtualMemUsed += memInfo.totalswap - memInfo.freeswap;
        virtualMemUsed *= memInfo.mem_unit;
        
        // RAM
        totalPhysMem = memInfo.totalram;
        //Multiply in next statement to avoid int overflow on right hand side...
        totalPhysMem *= memInfo.mem_unit;
        
        // Physical Memory currently used
        physMemUsed = memInfo.totalram - memInfo.freeram;
        //Multiply in next statement to avoid int overflow on right hand side...
        physMemUsed *= memInfo.mem_unit;
    }

    void get_memory_info ( std::size_t & mem_total 
                         , std::size_t & mem_used
                         , std::size_t & mem_cache
                         , std::size_t & mem_buffers
                         )
    {
        std::string line;
        std::ifstream myfile ( "/proc/meminfo" );
        if ( myfile . is_open() )
        {
            std::size_t line_no = 0;
            while ( getline ( myfile , line ) )
            {
                if ( line_no == 0 )
                {
                    std::stringstream ss;
                    ss << line;
                    std::string str;
                    ss >> str;
                    std::size_t mem;
                    ss >> mem;
                    mem_total = mem;
                }
                if ( line_no == 1 )
                {
                    std::stringstream ss;
                    ss << line;
                    std::string str;
                    ss >> str;
                    std::size_t mem;
                    ss >> mem;
                    mem_cache = mem_total - mem;
                }
                if ( line_no == 3 )
                {
                    std::stringstream ss;
                    ss << line;
                    std::string str;
                    ss >> str;
                    std::size_t mem;
                    ss >> mem;
                    mem_buffers = mem;
                }
                if ( line_no == 4 )
                {
                    std::stringstream ss;
                    ss << line;
                    std::string str;
                    ss >> str;
                    std::size_t mem;
                    ss >> mem;
                    mem_used = mem;
                    mem_used -= mem_buffers;
                }
                line_no ++;
            }
        }
    }

};

