#ifndef QUEUE_FILE_H
#define QUEUE_FILE_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "boost/filesystem.hpp"

#include "data.h"

struct QueueEntry
{
    std::size_t key;
    std::string value;
    QueueEntry ( std::size_t _key
               , std::string _value
               )
    : key   ( _key   )
    , value ( _value )
    {

    }
};

class QueueFile
{
    std::string m_directory_name;
    std::string m_unlock_name;
    std::string m_lock_name;
public:
    QueueFile ( std::string p_directory_name )
    : m_directory_name ( p_directory_name )
    , m_unlock_name ( p_directory_name+"/../unlocked" )
    , m_lock_name ( p_directory_name+"/../locked" )
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

private:

    int lock ()
    {
        if ( boost::filesystem::exists ( m_lock_name ) )
        {
            // directory locked, move along
            return 1;
        }
        else
        {
            // lock directory
            // boost::filesystem::rename(src,target)
            // should be atomic
            // so this should be equivalent to locking a mutex
            try
            {
                boost::filesystem::rename ( m_unlock_name , m_lock_name );
            }
            catch ( ... )
            {
                return 1;
            }
            return 0;
        }
    }

    int unlock ()
    {
        if ( boost::filesystem::exists ( m_unlock_name ) )
        {
            // can't unlock a mutex that is not locked, 
            // this should be an error,
            // it seems the developer messed up
            std::cout << "can't unlock a mutex that is not locked" << std::endl;
            return 1;
        }
        else
        {
            // unlock directory
            // boost::filesystem::rename(src,target)
            // should be atomic
            // so this should be equivalent to unlocking a mutex
            try
            {
                boost::filesystem::rename ( m_lock_name , m_unlock_name );
            }
            catch ( ... )
            {
                return 1;
            }
            return 0;
        }
    }

    int push ( std::string                        file_name
             , std::vector < QueueEntry > const & input
             )
    {
        // if directory is locked, 
        // we don't allow the thread to write to this file
        if ( lock () == 1 ) return 1;
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
        Unlock();
        return 0;
    }

    int pop ( std::string                  file_name
            , std::vector < QueueEntry > & output
            )
    {
        // since we are going to remove the file, once it is in memory,
        // and we don't want other threads to contain duplicate data,
        // we want to lock the directory before reading this file
        if ( lock () == 1 ) return 1;
        std::string line;
        std::ifstream myfile (file_name.c_str());
        if (myfile.is_open())
        {
            while ( getline ( myfile , line ) )
            {
                std::stringstream ss;
                ss << line;
                std::size_t key;
                ss >> key;
                std::string value;
                ss >> value;
                output . push_back ( QueueEntry ( key , value ) );
            }
            myfile.close();
        }
        else
        {
            std::cout << "Unable to open file : " << file_name << std::endl; 
        }
        boost::filesystem::remove ( file_name );
        Unlock();
        return 0;
    }

    void Unlock()
    {
        while ( unlock () == 1 ) { usleep(500000); }
    }
};

#endif

