#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include "multithreading/semaphore.h"
#include "asio/client.h"
#include "asio/server.h"
#include "info.h"

void read_file ( std::string file_name 
               , std::stringstream * output_ss 
               )
{
    std::string line;
    std::string token;
    std::ifstream myfile ( file_name.c_str() );
    if ( myfile . is_open () )
    {
        std::cout << "File " << file_name << " is open." << std::endl;
        std::size_t num_print = 0;
        while ( getline ( myfile , line ) )
        {
            std::stringstream ss;
            ss << line;
            while ( ss >> token )
            {
                {
                    num_print++;
                    *output_ss << token << " ";
                }
            }
        }
        myfile . close ();
        std::cout << num_print << std::endl;
    }
    else
    {
        std::cout << "Unable to open file : " << file_name << std::endl;
    }
}

void ping_thread ( std::string host , std::size_t port ) 
{
    boost::asio::io_service svc;
    Client client(svc, host, std::to_string(port));

    client.send("Requesting Ping ... \n");
}

void push_data_thread ( std::string host , std::size_t port , std::stringstream * ss ) 
{
    boost::asio::io_service svc;
    Client client(svc, host, std::to_string(port));
    int batch_count = 100;
    while ( true )
    {
        int count = 0;
        bool done = true;
        std::stringstream ss_cpy;
        std::string line;
        while ( *ss >> line )
        {
            ss_cpy << line << " ";
            count++;
            if ( batch_count == count )
            {
                client . send ( ss_cpy . str() );
                usleep(100);
                done = false;
                break;
            }
        }
        if ( !done ) continue;
        client . send ( ss_cpy . str() );
        usleep(100);
        break;
    }
    delete ss;
}

int main(int argc,char * argv[])
{
    std::cout << "Welcome to ClientInterface!" << std::endl;
    std::string config_file;
    if ( argc != 2 )
    {
        std::cout << "try ./ClientInterface <config_file>" << std::endl;
        return 1;
    }
    config_file = std::string(argv[1]);

    host_info host;
    std::vector < connection_info > connections;
    parse_config_file ( config_file 
                      , host 
                      , connections 
                      );

    std::cout << "port no : " << host . port_no << std::endl;



    std::string line;
    while(1)
    {
        std::cout << ">>> ";
        getline(std::cin,line);
        std::cout << line << std::endl;
        std::stringstream ss;
        ss << line;
        std::string command;
        ss >> command;
        if ( command == "load" )
        {
            std::string file_name;
            ss >> file_name;
            std::cout << "Loading " << file_name << " ... " << std::endl;
            std::stringstream ss;
            read_file ( file_name , &ss );
            std::cout << ss << std::endl;
            std::cout << "Done ... " << std::endl;
        }
        if ( command == "ping" )
        {
            boost::thread t ( ping_thread
                            , host.host_name
                            , host.port_no
                            );
        }
        if ( command == "push" )
        {
            std::string file_name;
            ss >> file_name;
            std::cout << "Pushing " << file_name << " ... " << std::endl;
            std::stringstream * ss = new std::stringstream();
            read_file ( file_name , ss );
            boost::thread t ( push_data_thread
                            , host.host_name
                            , host.port_no
                            , ss
                            );
            std::cout << "Done ... " << std::endl;
        }
        if ( command == "test" )
        {
            std::string file_name = "data/synthetic_test_data";
            std::cout << "Pushing " << file_name << " ... " << std::endl;
            std::stringstream * ss = new std::stringstream();
            read_file ( file_name , ss );
            boost::thread t ( push_data_thread
                            , host.host_name
                            , host.port_no
                            , ss
                            );
            std::cout << "Done ... " << std::endl;
        }

    }
    return 0;
}

