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
        char * data_ = new char[max_length];
        memset ( data_ , 0 , max_length );
        socket_.async_read_some ( boost::asio::buffer ( data_
                                                      , max_length
                                                      )
                                , boost::bind ( &session::handle_read
                                              , this
                                              , boost::asio::placeholders::error
                                              , data_
                                              , boost::asio::placeholders::bytes_transferred
                                              )
                                );
    }

private:
    void handle_read ( const boost::system::error_code& error
                     , char * p_data_
                     , size_t bytes_transferred
                     )
    {
        if (!error)
        {
            boost::asio::async_write ( socket_
                                     , boost::asio::buffer ( p_data_
                                                           , bytes_transferred
                                                           )
                                     , boost::bind ( &session::handle_write
                                                   , this
                                                   , boost::asio::placeholders::error
                                                   , p_data_
                                                   )
                                     );
        }
        else
        {
            delete this;
        }
    }

    void handle_write ( const boost::system::error_code& error 
                      , char * p_data_
                      )
    {
        if (!error)
        {
            std::stringstream ss;
            ss << p_data_;
            std::cout << p_data_ << std::endl;
            std::string str;
            while ( ss >> str )
            {
                queue -> put ( new Chunk(str) );
            }
            delete [] p_data_;
            char * data_ = new char[max_length];
            memset ( data_ , 0 , max_length );
            socket_.async_read_some ( boost::asio::buffer ( data_
                                                          , max_length
                                                          )
                                    , boost::bind ( &session::handle_read
                                                  , this
                                                  , boost::asio::placeholders::error
                                                  , data_
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
    QueueType * queue;
    tcp::acceptor acceptor_;
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





