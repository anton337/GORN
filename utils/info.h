#ifndef INFO_H
#define INFO_H

#include <iostream>
#include <fstream>

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

void parse_config_file ( std::string file_name , host_info & host , std::vector < connection_info > & connections )
{
    std::string line;
    std::string token;
    std::ifstream myfile ( file_name.c_str() );
    bool get_host = false;
    std::string host_name ( "" );
    std::size_t port_no ( -1 );
    if ( myfile . is_open () )
    {
        while ( getline ( myfile , line ) )
        {
            std::stringstream ss;
            ss << line;
            ss >> token;
            if ( token . find ( "url" ) != std::string::npos )
            {
                while ( ss >> token )
                {
                    if ( token . find ( "host" ) != std::string::npos )
                    {
                        std::size_t start_id = token . find ( "\'" );
                        std::size_t end_id = token . find ( "\'" , start_id+1 );
                        host_name = token.substr(start_id+1,end_id-start_id-1);
                    }
                    if ( token . find ( "port" ) != std::string::npos )
                    {
                        std::size_t start_id = token . find ( "\'" );
                        std::size_t end_id = token . find ( "\'" , start_id+1 );
                        port_no = (std::size_t)atoi(token.substr(start_id+1,end_id-start_id-1).c_str());
                    }
                }
                if ( !get_host )
                {
                    get_host = true;
                    host = host_info ( host_name , port_no );
                }
                else
                {
                    connections . push_back ( connection_info ( host_name , port_no ) );
                }
            }
        }
        myfile . close ();
    }
    else
    {
        std::cout << "Unable to open file : " << file_name << std::endl;
    }
}


#endif

