#ifndef NETWORK_H
#define NETWORK_H


//////////////////////////////////////
//                                  //
//                                  //
//             SERVER               //
//                                  //
//                                  //
//////////////////////////////////////

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

template < typename Chunk, typename QueueType >
class session
{
public:
    session ( boost::asio::io_service& io_service
            , QueueType * _queue
            )
    : socket_(io_service)
    , queue  (_queue)
    {

    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        socket_.async_read_some ( boost::asio::buffer ( data_
                                                      , max_length
                                                      )
                                , boost::bind ( &session::handle_read
                                              , this
                                              , boost::asio::placeholders::error
                                              , boost::asio::placeholders::bytes_transferred
                                              )
                                );
    }

private:
    void handle_read(const boost::system::error_code& error,
        size_t bytes_transferred)
    {
        if (!error)
        {
            std::string str = data_;
            boost::asio::async_write ( socket_
                                     , boost::asio::buffer ( data_
                                                           , bytes_transferred
                                                           )
                                     , boost::bind ( &session::handle_write
                                                   , this
                                                   , boost::asio::placeholders::error
                                                   )
                                     );
        }
        else
        {
            delete this;
        }
    }

    void handle_write(const boost::system::error_code& error)
    {
        if (!error)
        {
            std::stringstream ss;
            ss << data_;
            std::string str;
            while ( ss >> str )
            {
                queue -> put ( new Chunk(str) );
            }
            socket_.async_read_some ( boost::asio::buffer ( data_
                                                          , max_length
                                                          )
                                    , boost::bind ( &session::handle_read
                                                  , this
                                                  , boost::asio::placeholders::error
                                                  , boost::asio::placeholders::bytes_transferred
                                                  )
                                    );
        }
        else
        {
          delete this;
        }
    }

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    QueueType * queue;
};

template < typename Chunk, typename QueueType >
class server
{
public:
    server ( boost::asio::io_service& io_service
           , QueueType * _queue
           , short port
           )
    : io_service_(io_service)
    , queue ( _queue )
    , acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
    {
        start_accept();
    }

private:
    void start_accept()
    {
        session < Chunk , QueueType > * new_session = new session < Chunk , QueueType > ( io_service_
                                                                                        , queue
                                                                                        );
        acceptor_.async_accept ( new_session->socket()
                               , boost::bind ( &server::handle_accept
                                             , this
                                             , new_session
                                             , boost::asio::placeholders::error
                                             )
                               );
    }

    void handle_accept ( session < Chunk , QueueType > * new_session
                       , const boost::system::error_code& error
                       )
    {
        if (!error)
        {
            new_session->start();
        }
        else
        {
            delete new_session;
        }

        start_accept();
    }

    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
    QueueType * queue;
};

template < typename Chunk, typename QueueType >
void server_thread ( QueueType * queue , const int PORT = 52275 ) 
{
    try
    {
        boost::asio::io_service io_service;

        using namespace std; // For atoi.
        server < Chunk , QueueType >  s(io_service, queue, PORT);

        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
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
            std::cout << "Sending : " << message << std::endl;
            const char * request = message . c_str ();
            size_t request_length = strlen(request);
            boost::asio::write(socket, boost::asio::buffer(request, request_length));

            char reply[max_length];
            size_t reply_length = boost::asio::read(socket,boost::asio::buffer(reply, request_length));
            std::cout << "Reply is: ";
            std::cout.write(reply, reply_length);
            std::cout << "\n";
        
            // socket.send(boost::asio::buffer(message));
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << "\n";
        }
    }

};






#endif





