#ifndef SERVER_H
#define SERVER_H


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
#include <boost/thread.hpp>
#include <string>
#include <vector>
#include "multithreading/producer_consumer_queue.h"

using boost::asio::ip::tcp;

struct Packet
{
    std::string data;
    Packet ( std::string _data )
    : data ( _data )
    {

    }
};

template < typename Chunk, typename QueueType >
void collect_packets ( ProducerConsumerQueue < Packet > * packet_queue 
                     , QueueType                        * _queue
                     , std::size_t                        max_length
                     )
{
    std::stringstream ss;
    bool done = false;
    while ( !done )
    {
        Packet * item = packet_queue -> get ();
        ss << item -> data;
        if ( item -> data . size () < max_length )
        {
            Chunk * o_item = new Chunk ( ss . str () );
            _queue -> put ( o_item );
            done = true;
        }
        delete item;
    }
}

template < typename Chunk, typename QueueType >
class session
{
public:
    session ( boost::asio::io_service& io_service
            , QueueType * _queue
            )
    : socket_(io_service)
    , queue  (_queue)
    , packet_queue ( new ProducerConsumerQueue < Packet > (10000) )
    {
        boost::thread t ( collect_packets < Chunk, QueueType > 
                        , packet_queue
                        , _queue
                        , max_length
                        );
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        char * data_ = new char[max_length+1];
        memset ( data_ , 0 , max_length+1 );
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
            packet_queue -> put ( new Packet ( std::string ( p_data_ ) ) );
            delete [] p_data_;
            char * data_ = new char[max_length+1];
            memset ( data_ , 0 , max_length+1 );
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
    enum { max_length = 4*1024 };
    QueueType * queue;
    ProducerConsumerQueue < Packet > * packet_queue;
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


#endif





