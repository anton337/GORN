#include "data/data.h"

#include <iostream>

int main()
{
    std::cout << "Welcome to DataSorterDaemon!" << std::endl;
    std::string input_directory = "batch_output/";
    std::string initial_file = "sorted_output/comprehensive.out";
    std::string comprehensive_file = "sorted_output/comprehensive.tmp";
    while ( true )
    {
        sleep(5);
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
    }
    return 0;
}

