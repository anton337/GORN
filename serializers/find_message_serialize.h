#ifndef FIND_MESSAGE_SERIALIZE_H
#define FIND_MESSAGE_SERIALIZE_H

#include "serialize.h"

class FindMessage : public Serialize
{
    std::string serialize()
    {

    }
    std::size_t deserialize( std::string str )
    {

    }
    std::size_t get_type ()
    {
        return 4389728;
    }
};

#endif

