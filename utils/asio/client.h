#ifndef CLIENT_H
#define CLIENT_H


//////////////////////////////////////
//                                  //
//                                  //
//             CLIENT               //
//                                  //
//                                  //
//////////////////////////////////////

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

enum { max_length = 1024 };

struct Client
{

    boost::asio::io_service& io_service;
    boost::asio::ip::tcp::socket socket;

    Client(boost::asio::io_service& svc, std::string const& host, std::string const& port) 
        : io_service(svc), socket(io_service) 
    {
        try
        {
            tcp::resolver resolver(io_service);
            tcp::resolver::query query(tcp::v4(), host, port);
            tcp::resolver::iterator iterator = resolver.resolve(query);

            boost::asio::connect(socket, iterator);

        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << "\n";
        }
    }

    void send(std::string const& message) {
        try
        {
            using namespace std; // For strlen.
            const char * request = message . c_str ();
            size_t request_length = strlen(request);
            boost::asio::write(socket, boost::asio::buffer(request, request_length));

            char reply[max_length];
            size_t reply_length = boost::asio::read(socket,boost::asio::buffer(reply, request_length));
            if ( reply_length == 0 )
            {
                std::cout << "done reading . " << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }

};






#endif





