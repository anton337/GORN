#ifndef NETWORK_H
#define NETWORK_H

#include <boost/asio.hpp>

struct Client
{
    boost::asio::io_service& io_service;
    boost::asio::ip::tcp::socket socket;

    Client(boost::asio::io_service& svc, std::string const& host, std::string const& port) 
        : io_service(svc), socket(io_service) 
    {
        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::iterator endpoint = resolver.resolve(boost::asio::ip::tcp::resolver::query(host, port));
        boost::asio::connect(this->socket, endpoint);
    };

    void send(std::string const& message) {
        socket.send(boost::asio::buffer(message));
    }
};

#include <boost/thread.hpp>

#include <iostream>

void server_thread ( const int PORT = 52275 ) 
{
    while ( 1 )
    {
        try
        {
            boost::asio::io_service io_service;
            boost::asio::ip::tcp::acceptor acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), PORT));

            {
                boost::asio::ip::tcp::socket socket(io_service);
                acceptor.accept(socket);

                boost::asio::streambuf sb;
                boost::system::error_code ec;
                while (boost::asio::read(socket, sb, ec)) {
                    std::cout << "received: '" << &sb << "'\n";

                    if (ec) {
                        std::cout << "status: " << ec.message() << "\n";
                        break;
                    }
                }
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << " : " << PORT << std::endl;
        }
    }
}

struct Server
{

    boost::thread * tg;

    Server(const int port)
    {
        tg = new boost::thread(server_thread,port);
    }

    ~Server()
    {
        tg->join();
        delete tg;
    }

    void join ()
    {
        tg->join();
        delete tg;
    }

};

#endif

