#ifndef SERIALIZE_H
#define SERIALIZE_H

class Serialize
{
public:
    Serialize ()
    {

    }
    virtual std::string serialize( std::size_t offset , std::size_t num ) = 0;
    virtual std::size_t deserialize( std::string str ) = 0;
    virtual std::size_t get_type () = 0;
};

#endif

