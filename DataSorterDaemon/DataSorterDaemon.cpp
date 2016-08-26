#include "data/data.h"

#include "data/distributed_mutex.h"

#include <iostream>

int main()
{
    std::cout << "Welcome to DataSorterDaemon!" << std::endl;
    std::string input_directory = "batch_output/";
    std::string initial_file = "sorted_output/comprehensive.out";
    std::string comprehensive_file = "sorted_output/comprehensive.tmp";
    DistributedMutex c_mutex ( "sorted_output/unlocked"
                             , "sorted_output/locked"
                             );
    while ( true )
    {
        sleep(5);
        if ( c_mutex . Lock () == 1 ) continue;
        if ( sort_files ( input_directory
                        , initial_file
                        , comprehensive_file
                        , true
                        )
           )
        {
            remove_file ( initial_file );
            rename_file ( comprehensive_file 
                        , initial_file 
                        );
        }
        c_mutex . Unlock ();
    }
    return 0;
}

