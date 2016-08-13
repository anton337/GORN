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

void sort_write_file ( std::string                                         initial_name
                     , std::vector < std::vector < std::string > > const & sorted_data
                     , std::string                                         comprehensive_name
                     )
{
    std::string line;
    std::string file_line;
    std::ifstream input (initial_name.c_str());
    std::ofstream output (comprehensive_name.c_str());
    std::vector < std::vector < std::string > :: const_iterator > it_vec;
    for ( std::size_t k(0)
        ; k < sorted_data.size()
        ; ++k
        )
    {
        it_vec . push_back ( sorted_data[k] . begin () );
    }
    if (input.is_open())
    {
        if (output.is_open())
        {
            if ( !input.eof() )
            {
                input >> file_line;
            }
            while(true)
            {
                bool done ( true );
                bool init ( true );
                std::size_t max_k(-1);
                {
                    for ( std::size_t k(0)
                        ; k < it_vec.size()
                        ; ++k
                        )
                    {
                        if ( it_vec[k] != sorted_data[k] . end () )
                        {
                            if ( init )
                            {
                                line = *(it_vec[k]);
                                max_k = k;
                                done = false;
                                init = false;
                            }
                            else
                            {
                                if ( *(it_vec[k]) < line )
                                {
                                    line = *(it_vec[k]);
                                    max_k = k;
                                    done = false;
                                }
                            }
                        }
                    }
                }
                if ( !input.eof() )
                {
                    if ( file_line < line )
                    {
                        line = file_line;
                        input >> file_line;
                        done = false;
                    }
                    else
                    {
                        if ( max_k < it_vec.size() )
                        {
                            ++(it_vec[max_k]);
                        }
                    }
                }
                else
                {
                    if ( max_k < it_vec.size() )
                    {
                        ++(it_vec[max_k]);
                    }
                }
                if ( done )
                {
                    break;
                }
                output << line << " ";
            }
            output.close();
        } 
        else
        {
            std::cout << "Unable to open file : " << comprehensive_name << std::endl;
        }
        input.close();
    }
    else
    {
        std::cout << "Unable to open file : " << initial_name << std::endl;
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

void rename_file ( std::string src , std::string dest )
{
    try
    {
        boost::filesystem::rename(src,dest);
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
bool sort_files ( std::string dir
                , std::string initial_file
                , std::string comprehensive_file
                , bool remove = false
                )
{
    std::vector < std::string > files;
    get_files ( dir , files );
    if ( files . size () > 0 )
    {
        std::vector < std::vector < std::string > > sorted_data;
        for ( std::size_t k(0)
            ; k < files.size()
            ; ++k
            )
        {
            sorted_data . push_back ( std::vector < std::string > () );
            read_file ( files[k] , sorted_data[k] );
            sort_data ( sorted_data[k] );
            if ( remove )
            {
                remove_file ( files[k] );
            }
        }
        sort_write_file ( initial_file 
                        , sorted_data
                        , comprehensive_file
                        );
        return true;
    }
    return false;
}

#endif

