#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include "data/data.h"
#include "data/distributed_mutex.h"

std::string   non_unique_file_directory = "queue_data_non_unique/" ;

std::string       unique_file_directory = "sorted_output/"         ;

std::string            output_directory = "queue_data/"            ;

DistributedMutex c_mutex ( "unlocked"
                         , "locked"
                         );

bool verify_uniqueness ( std::vector < std::vector < std::string > > & vec )
{
    std::map < std::string , std::size_t > unique_map;
    for ( std::size_t k(0)
        ; k < vec.size()
        ; ++k
        )
    {
        for ( std::size_t i(0)
            ; i < vec[k].size()
            ; ++i
            )
        {
            if ( unique_map.find(vec[k][i]) == unique_map.end() )
            {
                unique_map[vec[k][i]] = 1;
            }
            else
            {
                ++unique_map[vec[k][i]];
            }
        }
    }
    std::map < std::string , std::size_t >::iterator it = unique_map . begin ();
    while ( it != unique_map . end () )
    {
        if ( it->second > 1 ) return false;
        ++it;
    }
    return true;
}

void enforce_uniqueness ( std::vector < std::vector < std::string > > & vec )
{
    std::map < std::string , std::size_t > unique_map;
    for ( std::size_t k(0)
        ; k < vec.size()
        ; ++k
        )
    {
        for ( std::size_t i(0)
            ; i < vec[k].size()
            ; ++i
            )
        {
            while ( 1 )
            {
                if ( unique_map.find(vec[k][i]) == unique_map.end() )
                {
                    unique_map[vec[k][i]] = 1;
                    break;
                }
                else
                {
                    std::size_t v = atoi(vec[k][i].c_str())+1;
                    std::stringstream ss;
                    ss << v;
                    vec[k][i] = ss.str();
                }
            }
        }
    }
}

void remove_non_unique ( std::vector < std::vector < std::string > > const & unique
                       , std::vector < std::vector < std::string > > const & non_unique_input
                       , std::vector < std::vector < std::string > >       & unique_output
                       , std::size_t                                         n_numbers
                       )
{
    std::map < std::string , std::size_t > unique_map;
    for ( std::size_t k(0)
        ; k < unique.size()
        ; ++k
        )
    {
        for ( std::size_t i(0)
            ; i < unique[k].size()
            ; ++i
            )
        {
            while ( 1 )
            {
                if ( unique_map.find(unique[k][i]) == unique_map.end() )
                {
                    unique_map[unique[k][i]] = 1;
                    break;
                }
                else
                {
                    std::cout << "Unique vector contains duplicates : error " << std::endl;
                    return;
                }
            }
        }
    }
    unique_output . clear ();
    std::vector < std::string > vec;
    for ( std::size_t k(0)
        ; k < non_unique_input.size()
        ; ++k
        )
    {
        for ( std::size_t i(0)
            ; i < non_unique_input[k].size()
            ; ++i
            )
        {
            if ( unique_map.find(non_unique_input[k][i]) == unique_map.end() )
            {
                unique_map[non_unique_input[k][i]] = 1;
                vec . push_back ( non_unique_input[k][i] );
                if ( vec . size () >= n_numbers )
                {
                    unique_output . push_back ( vec );
                    vec . clear ();
                }
            }
            else
            {

            }
        }
    }
    unique_output . push_back ( vec );
    vec . clear ();
}

void obtain_non_unique_data ( std::string non_unique_file_directory 
                            , std::vector < std::vector < std::string > > & non_unique_data
                            )
{
    std::vector < std::string > files;
    get_files ( non_unique_file_directory
              , files
              );
    std::size_t n_non_unique = files . size ();
    for ( std::size_t k(0)
        ; k < n_non_unique
        ; ++k
        )
    {
        std::vector < std::string > vec;
        read_file ( files[k] , vec );
        non_unique_data . push_back ( vec );
        remove_file ( files[k] );
    }
    std::cout << "Non Unique : " << verify_uniqueness ( non_unique_data ) << std::endl;
}

void obtain_unique_data ( std::string unique_file_directory 
                        , std::vector < std::vector < std::string > > & unique_data
                        )
{
    std::vector < std::string > files;
    get_files ( unique_file_directory
              , files
              );
    std::size_t n_non_unique = files . size ();
    for ( std::size_t k(0)
        ; k < n_non_unique
        ; ++k
        )
    {
        std::vector < std::string > vec;
        read_file ( files[k] , vec );
        unique_data . push_back ( vec );
    }
    enforce_uniqueness ( unique_data );
    std::cout << "Unique : " << verify_uniqueness ( unique_data ) << std::endl;
}

void filter_out_non_unique_values ( std::vector < std::vector < std::string > > & unique_data
                                  , std::vector < std::vector < std::string > > & non_unique_data
                                  , std::vector < std::vector < std::string > > & output_data
                                  , std::string output_directory
                                  , std::size_t file_size = 1000 
                                  )
{
    std::size_t n_numbers = file_size;
    remove_non_unique ( unique_data
                      , non_unique_data
                      , output_data
                      , n_numbers
                      );
    std::cout << "Output : " << output_data . size () << "  " << output_data[output_data.size()-1] . size () << "  " << verify_uniqueness ( output_data ) << std::endl;
    for ( std::size_t k(0)
        ; k < output_data . size ()
        ; ++k
        )
    {
        std::stringstream file_name;
        file_name << output_directory << "/" << "unique_data_" << rand() << ".que";
        write_file ( file_name . str ()
                   , output_data[k]
                   , '\n'
                   );
    }
}

int main()
{
    srand(time(0));
    std::cout << "Welcome to DistributedQueue!" << std::endl;

    while ( 1 )
    {

        sleep(5);

        if ( c_mutex . Lock () == 1 ) continue;

        std::vector < std::vector < std::string > > non_unique_data;

        std::vector < std::vector < std::string > > unique_data;

        std::vector < std::vector < std::string > > output_data;

        // obtain non_unique_data
        {
            obtain_non_unique_data ( non_unique_file_directory 
                                   , non_unique_data
                                   );
        }

        // obtain unique_data
        {
            obtain_unique_data ( unique_file_directory 
                               , unique_data
                               );
            obtain_unique_data ( non_unique_file_directory
                               , unique_data
                               );
        }

        // filter out non unique values
        {
            filter_out_non_unique_values ( unique_data
                                         , non_unique_data
                                         , output_data
                                         , output_directory
                                         );
        }

        c_mutex . Unlock ();

    }

    return 0;
}

