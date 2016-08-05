#ifndef INFO_H
#define INFO_H

struct host_info
{
    std::string host_name;
    std::size_t port_no;
    host_info ( std::string _host_name
              , std::size_t _port_no
              )
    : host_name ( _host_name )
    , port_no   ( _port_no   )
    {

    }
    host_info ()
    : host_name ( "" )
    , port_no ( -1 )
    {

    }
};

struct connection_info
{
    std::string host_name;
    std::size_t port_no;
    connection_info ( std::string _host_name
                    , std::size_t _port_no
                    )
    : host_name ( _host_name )
    , port_no   ( _port_no   )
    {

    }
    connection_info ()
    : host_name ( "" )
    , port_no ( -1 )
    {

    }
};

#endif

