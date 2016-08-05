#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <string>
#include "info.h"

void write_config_file ( std::string file_name , host_info host , std::vector < connection_info > const & connections )
{
    std::ofstream myfile;
    myfile.open(file_name.c_str());
    myfile << "<config>"                                            << std::endl;
    myfile << "    <node>"                                          << std::endl;
    std::string local_host_name = host.host_name;
    std::size_t local_host_port_no = host.port_no;
    myfile << "        <url host='" << local_host_name << "' port='" << local_host_port_no << "' />"       << std::endl;
    myfile << "    </node>"                                         << std::endl;
    myfile << "    <links>"                                         << std::endl;
    myfile << "        <link>"                                      << std::endl;
    for ( std::size_t k(0)
        ; k < connections.size()
        ; ++k
        )
    {
        std::string remote_host_name = connections[k].host_name;
        std::size_t remote_host_port_no = connections[k].port_no;
        myfile << "            <url host='" << remote_host_name << "' port='" << remote_host_port_no << "' />"       << std::endl;
    }
    myfile << "        </link>"                                     << std::endl;
    myfile << "    </links>"                                        << std::endl;
    myfile << "</config>"                                           << std::endl;
    myfile.close();
}

int main(int argc,char * argv[])
{
    std::cout << "Welcome to GenerateClusterConfigs!" << std::endl;
    std::size_t num_nodes ( -1 );
    std::size_t port_root ( -1 );
    if ( argc != 3 )
    {
        std::cout << "try ./GenerateClusterConfigs <num_nodes> <port_root>" << std::endl;
        return 1;
    }
    num_nodes = atoi(argv[1]);
    port_root = atoi(argv[2]);
    std::string host = "localhost";
    for ( std::size_t k(0)
        ; k < num_nodes
        ; ++k
        )
    {
        std::stringstream node_ss;
        node_ss << "node" << k << ".xml";
        std::size_t node_port = port_root + k;
        std::vector < connection_info > connections;
        for ( std::size_t offset(1)
            ; offset < num_nodes
            ; offset *= 2
            )
        { 
            std::size_t offset_port = port_root + ( k + offset ) % num_nodes;
            connections . push_back ( connection_info ( host , offset_port ) );
        }
        write_config_file ( node_ss.str() , host_info ( host , node_port ) , connections );
    }
    return 0;
}

