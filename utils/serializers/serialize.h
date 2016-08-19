#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <sstream>

#define      STORE_TYPE     1234589
#define FIND_REPLY_TYPE     9387534
#define       FIND_TYPE     4389728

class Serialize
{
public:
    Serialize ()
    {

    }
    virtual std::string serialize( std::size_t offset , std::size_t num ) = 0;
    virtual std::size_t deserialize( std::string str ) = 0;
    virtual std::size_t get_type () = 0;
    static std::size_t extract_type ( std::string str )
    {
        std::stringstream ss;
        ss << str;
        std::size_t type;
        ss >> type;
        return type;
    }
};

#endif

