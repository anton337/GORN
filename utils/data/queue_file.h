#ifndef QUEUE_FILE_H
#define QUEUE_FILE_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "boost/filesystem.hpp"
#include "queue_entry.h"

#include "data.h"

#include "distributed_mutex.h"

template < typename QueueEntry >
class QueueFile
{
    std::string m_directory_name;
    DistributedMutex m_distributed_mutex;
public:
    QueueFile ( std::string p_directory_name )
    : m_directory_name ( p_directory_name )
    , m_distributed_mutex ( p_directory_name+"/../unlocked" 
                          , p_directory_name+"/../locked" 
                          )
    {

    }

    ~QueueFile ()
    {

    }

    void Push ( std::string                        filename
              , std::vector < QueueEntry > const & input 
              )
    {
        bool status = false;
        while ( !status )
        {
            if ( push ( m_directory_name + "/" + filename
                      , input 
                      )
                 == 0 
               )
            {
                status = true;
                continue;
            }
            usleep(500000);
        }
    }

    void Pop ( std::vector < QueueEntry > & output )
    {
        bool status = false;
        while ( !status )
        {
            std::vector < std::string > files;
            get_files ( m_directory_name 
                      , files 
                      );
            if ( files . size () > 0 )
            {
                if ( pop ( files[0] 
                         , output 
                         )
                     == 0 
                   )
                {
                    status = true;
                    continue;
                }
            }
            usleep(500000);
        }
    }

    void Peek ( std::vector < QueueEntry > & output )
    {
        bool status = false;
        while ( !status )
        {
            std::vector < std::string > files;
            get_files ( m_directory_name 
                      , files 
                      );
            if ( files . size () > 0 )
            {
                if ( peek ( files[0] 
                          , output 
                          )
                     == 0 
                   )
                {
                    status = true;
                    continue;
                }
            }
            usleep(500000);
        }
    }

private:

    int push ( std::string                        file_name
             , std::vector < QueueEntry > const & input
             )
    {
        // if directory is locked, 
        // we don't allow the thread to write to this file
        if ( m_distributed_mutex.Lock () == 1 ) return 1;
        std::ofstream myfile (file_name.c_str());
        if (myfile.is_open())
        {
            for ( std::size_t k(0)
                ; k < input.size()
                ; ++k
                )
            {
                myfile << input[k].key   << " "
                       << input[k].value << std::endl;
            }
            myfile.close();
        } 
        else
        {
            std::cout << "Unable to open file : " << file_name << std::endl;
        }
        m_distributed_mutex.Unlock();
        return 0;
    }

    int pop ( std::string                  file_name
            , std::vector < QueueEntry > & output
            )
    {
        // since we are going to remove the file, once it is in memory,
        // and we don't want other threads to contain duplicate data,
        // we want to lock the directory before reading this file
        if ( m_distributed_mutex.Lock () == 1 ) return 1;
        std::string line;
        std::ifstream myfile (file_name.c_str());
        if (myfile.is_open())
        {
            while ( getline ( myfile , line ) )
            {
                std::stringstream ss;
                ss << line;
                output . push_back ( QueueEntry ( line ) );
            }
            myfile.close();
        }
        else
        {
            std::cout << "Unable to open file : " << file_name << std::endl; 
        }
        boost::filesystem::remove ( file_name );
        m_distributed_mutex.Unlock();
        return 0;
    }

    int peek ( std::string                  file_name
             , std::vector < QueueEntry > & output
             )
    {
        // since we are going to remove the file, once it is in memory,
        // and we don't want other threads to contain duplicate data,
        // we want to lock the directory before reading this file
        if ( m_distributed_mutex.Lock () == 1 ) return 1;
        std::string line;
        std::ifstream myfile (file_name.c_str());
        if (myfile.is_open())
        {
            while ( getline ( myfile , line ) )
            {
                std::stringstream ss;
                ss << line;
                output . push_back ( QueueEntry ( line ) );
            }
            myfile.close();
        }
        else
        {
            std::cout << "Unable to open file : " << file_name << std::endl; 
        }
        m_distributed_mutex.Unlock();
        return 0;
    }
};

#endif

