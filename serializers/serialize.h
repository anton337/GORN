#ifndef SERIALIZE_H
#define SERIALIZE_H

class Serialize
{
    virtual std::string serialize() = 0;
    virtual std::size_t deserialize( std::string str ) = 0;
    virtual std::size_t get_type () = 0;
};

#endif

