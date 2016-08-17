#ifndef STORE_MESSAGE_SERIALIZE_H
#define STORE_MESSAGE_SERIALIZE_H

#include "serialize.h"

class StoreMessage : public Serialize
{
    std::vector < std::string > m_data;
    std::string serialize( std::size_t offset , std::size_t num )
    {
        std::stringstream ss;
        ss << get_type () << " ";
        std::size_t size ( std::min ( m_data . size () , offset + num ) );
        ss << size << " ";
        for ( std::size_t k ( offset )
            ; k < size
            ; ++k
            )
        {
            ss << m_data [ k ] << " ";
        }
    }
    std::size_t deserialize( std::string str )
    {
        std::stringstream ss;
        ss << str;
        std::size_t type;
        if ( type != get_type () )
        {
            return 1;
        }
        ss >> type;
        std::size_t num;
        ss >> num;
        m_data . clear ();
        for ( std::size_t k(0)
            ; k < num
            ; ++k
            )
        {
            std::string value;
            ss >> value;
            m_data . push_back ( value );
        }
        if ( !ss.str().empty() )
        {
            std::cout << "Data still left in serialized input, an error has occurred" << std::endl;
            return 1;
        }
        return 0;
    }
    std::size_t get_type ()
    {
        return 1234589;
    }
};

#endif

