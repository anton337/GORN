#include "data/data.h"

#include "data/distributed_mutex.h"

#include <iostream>

#include <set>

bool sort_files_map ( std::string input_directory
                    , std::string initial_file
                    , std::string comprehensive_file
                    )
{
    std::vector < std::string > file_names;
    get_files ( input_directory , file_names );
    if ( file_names . size () > 0 )
    {
        std::set < std::string > unique_set;
        std::vector < std::string > data_vec;
        std::vector < std::string > unique_vec;
        read_file ( initial_file , data_vec );
        for ( std::size_t k(0)
            ; k < file_names . size ()
            ; ++k
            )
        {
            read_file ( file_names[k] , data_vec );
            remove_file ( file_names[k] );
        }
        for ( std::size_t k(0)
            ; k < data_vec . size ()
            ; ++k
            )
        {
            unique_set . insert ( data_vec[k] );
        }
        std::set < std::string > :: iterator it = unique_set . begin ();
        while ( it != unique_set . end () )
        {
            unique_vec . push_back ( *it );
            ++it;
        }
        write_file ( comprehensive_file , unique_vec );
        return true;
    }
    return false;
}

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
        if ( sort_files_map ( input_directory
                            , initial_file
                            , comprehensive_file
                            )
           )
        {
            std::cout << "updating data ... " << std::endl;
            remove_file ( initial_file );
            rename_file ( comprehensive_file 
                        , initial_file 
                        );
        }
        c_mutex . Unlock ();
    }
    return 0;
}

