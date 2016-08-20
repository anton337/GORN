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
    std::size_t index;
    Packet ( std::string _data 
           , std::size_t _index
           )
    : data  ( _data  )
    , index ( _index )
    {

    }
};

bool packet_comparator ( Packet a , Packet b )
{
    return a . index < b . index ;
}

template < typename Chunk, typename QueueType >
void collect_packets ( ProducerConsumerQueue < Packet > * packet_queue 
                     , QueueType                        * _queue
                     , std::size_t                        max_length
                     , bool                             * destroyed
                     )
{
    while ( !(*destroyed) )
    {
        std::stringstream ss;
        std::vector < Packet > vec;
        bool done = false;
        while ( !done )
        {
            Packet * item = packet_queue -> get ();
            vec . push_back ( *item );
            if ( item -> data . size () < max_length )
            {
                std::sort ( vec . begin () , vec . end () , packet_comparator );
                for ( std::size_t k(0)
                    ; k < vec . size ()
                    ; ++k
                    )
                {
                    ss << vec[k] . data;
                }
                Chunk * o_item = new Chunk ( ss . str () );
                _queue -> put ( o_item );
                done = true;
            }
            delete item;
            item = NULL;
        }
    }
    delete packet_queue;
}

template < typename Chunk, typename QueueType >
class session
{
    bool * destroyed;
    boost::thread * t;
public:
    session ( boost::asio::io_service& io_service
            , QueueType * _queue
            )
    : socket_(io_service)
    , queue  (_queue)
    , packet_queue ( new ProducerConsumerQueue < Packet > (10000000) )
    {
        destroyed = new bool(false);
        t = new boost::thread ( collect_packets < Chunk, QueueType > 
                              , packet_queue
                              , _queue
                              , max_length
                              , destroyed
                              );
    }

    ~session ()
    {

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
                                              , 1
                                              )
                                );
    }

private:
    void handle_read ( const boost::system::error_code& error
                     , char * p_data_
                     , std::size_t bytes_transferred
                     , std::size_t index
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
                                                   , index+1
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
                      , std::size_t index
                      )
    {
        if (!error)
        {
            packet_queue -> put ( new Packet ( std::string ( p_data_ ) , index ) );
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
                                                  , index+1
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





