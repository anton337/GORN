#ifndef DATA_H
#define DATA_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "boost/filesystem.hpp"

#include "sorting.h"

void read_file ( std::string                   file_name 
               , std::vector < std::string > & output
               )
{
    std::string line;
    std::ifstream myfile (file_name.c_str());
    if (myfile.is_open())
    {
        while ( myfile >> line )
        {
            output . push_back ( line );
        }
        myfile.close();
    }
    else
    {
        std::cout << "Unable to open file : " << file_name << std::endl; 
    }
}

void sort_data ( std::vector < std::string > & input
               )
{
    std::sort ( input . begin () , input . end () );
}

void write_file ( std::string                         file_name
                , std::vector < std::string > const & input
                )
{
    std::ofstream myfile (file_name.c_str());
    if (myfile.is_open())
    {
        for ( std::size_t k(0)
            ; k < input.size()
            ; ++k
            )
        {
            myfile << input[k] << " ";
        }
        myfile.close();
    } 
    else
    {
        std::cout << "Unable to open file : " << file_name << std::endl;
    }
}

void get_files ( std::string                   dir
               , std::vector < std::string > & files
               )
{
    // list all files in current directory.
    //You could put any file path in here, e.g. "/home/me/mwah" to list that directory
    boost::filesystem::path p (dir);

    boost::filesystem::directory_iterator end_itr;

    // cycle through the directory
    for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr)
    {
        // If it's not a directory, list it. If you want to list directories too, just remove this check.
        if (is_regular_file(itr->path())) 
        {
            // assign current file name to current_file and echo it out to the console.
            std::string current_file = itr->path().string();
            files . push_back ( current_file );
        }
    }
}

void remove_file ( std::string file )
{
    try
    {
        boost::filesystem::remove(file);
    }
    catch ( boost::filesystem::filesystem_error & e )
    {
        std::cout << e.what() << std::endl;
    }
}

void sort_directory ( std::string dir 
                    , std::string output_file 
                    )
{
    std::vector < std::string > files;
    get_files ( dir , files );
    if ( files . size () > 0 )
    {
        std::vector < std::string > sorted_data;
        read_file ( files[0] , sorted_data );
        sort_data ( sorted_data );
        for ( std::size_t k(1)
            ; k < files.size()
            ; ++k
            )
        {
            std::vector < std::string > file_data;
            read_file ( files[k] , file_data );
            sort_data ( file_data );
            sorted_data = sorted_data + file_data;
        }
        write_file ( output_file 
                   , sorted_data
                   );
    }
}

// first input is a huge file
// it is assumed to already contain sorted data
// the second input is a smaller vector in memory
// the smaller vector is sorted in memory
void sort_files ( std::string                         comprehensive_file
                , std::vector < std::string > const & merging_input
                )
{

}

#endif

