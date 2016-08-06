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

template < typename Chunk, typename QueueType >
void server_thread ( QueueType * queue , const int PORT = 52275 ) 
{
    while ( 1 )
    {
        try
        {
            boost::asio::io_service io_service;
            boost::asio::ip::tcp::acceptor acceptor(io_service);
            boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), PORT);
            acceptor.open(ep.protocol());
            boost::asio::socket_base::reuse_address option(true);
            acceptor.set_option(option);
            acceptor.bind(ep);
            acceptor.listen();
            try
            {
                boost::asio::ip::tcp::socket socket(io_service);
                acceptor.accept(socket);
                boost::asio::streambuf sb;
                boost::system::error_code ec;
                while (boost::asio::read(socket, sb, ec)) {
                    // std::cout << "received: '" << &sb << "'\n";
                    std::stringstream ss;
                    ss << std::string ( std::istreambuf_iterator<char>( &sb ) , std::istreambuf_iterator<char>() ) << std::endl;
                    std::string str;
                    while ( ss >> str )
                    {
                        Chunk * item = new Chunk ( str );
                        (queue) -> put ( item );
                    }
                    if (ec) {
                        std::cout << "status: " << ec.message() << "\n";
                        break;
                    }
                }
            }
            catch (std::exception& e)
            {
                std::cerr << "Exception: " << e.what() << " : " << PORT << std::endl;
                sleep(1);
                //exit(1);
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << " : " << PORT << std::endl;
            sleep(1);
            //exit(1);
        }
    }
}

template < typename Chunk, typename QueueType >
struct Server
{

    boost::thread * tg;

    Server(QueueType * queue, const int port)
    {
        tg = new boost::thread(server_thread<Chunk,QueueType>,queue,port);
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

