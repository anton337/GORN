#ifndef QUEUE_ENTRY_H
#define QUEUE_ENTRY_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

struct QueueEntryKeyValue
{
    std::size_t key;
    std::string value;
    QueueEntryKeyValue ( std::size_t _key
                       , std::string _value 
                       )
    : key   ( _key   )
    , value ( _value )
    {

    }
    QueueEntryKeyValue ( std::string line )
    {
        get ( line );
    }
    void set ( std::ofstream & ss )
    {
        ss << key << " " << value << std::endl;
    }
    void get ( std::string line )
    {
        std::stringstream ss;
        ss << line;
        ss >> key;
        ss >> value;
    }
};

struct QueueEntryValue
{
    std::string value;
    QueueEntryValue ( std::string line )
    {
        get ( line );
    }
    void set ( std::ofstream & ss )
    {
        ss << value << std::endl;
    }
    void get ( std::string line )
    {
        std::stringstream ss;
        ss << line;
        ss >> value;
    }
};

#endif

