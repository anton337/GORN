#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <iostream>
#include <fstream>
#include <deque>
#include "multithreading/producer_consumer_queue.h"
#include "asio/client.h"
#include "asio/server.h"
#include "data/data.h"
#include "info.h"

void wait(int seconds)
{
    sleep(seconds);//boost::this_thread::sleep_for(boost::chrono::seconds{seconds});
}

#define BUFFER_SIZE 10000

host_info host;

std::vector < connection_info > connections;

enum MessageType { UNDEFINED = -1, STORE = 0 , FIND = 1 };

struct Chunk
{
    MessageType type;
    std::string message;
    Chunk ( std::string _message 
          , MessageType _type   = UNDEFINED
          )
    : message ( _message )
    ,   type  (    _type )
    {

    }
};

Server < Chunk , ProducerConsumerQueue < Chunk > > * node_server = NULL;

ProducerConsumerQueue < Chunk > * Queue = new ProducerConsumerQueue < Chunk > (BUFFER_SIZE);

ProducerConsumerQueue < Chunk > * * sorting_queue;

ProducerConsumerQueue < Chunk > * output_queue = new ProducerConsumerQueue < Chunk > (BUFFER_SIZE);

std::size_t num_sorting_queue = -1;

std::size_t node_index = -1;

std::size_t num_received = 0;

std::size_t find_chord_connection ( std::size_t index )
{
    std::size_t ret (-1);
    for ( std::size_t k(0)
        ; k < connections.size()
        ; ++k
        )
    {
        std::size_t width = (connections[k].port_no - host.port_no + 32) % 4;
        std::size_t dist  = (index - host.port_no + 32) % 4;
        if ( 2*width > dist )
        {
            ret = k;
            break;
        }
    }
    return ret;
}

void consumeItem ( Chunk * item )
{
    if ( item != NULL )
    {
        std::size_t k = atoi ( item -> message . c_str () );
        std::size_t index = k % 4; // num_sorting_queue;
        if ( index != node_index )
        {
            std::size_t queue_index = find_chord_connection ( index );
            (sorting_queue [queue_index]) -> put ( item );
        }
        else
        {
            // std::cout << host.port_no << " - " << ++num_received <<  " : " << item -> message << std::endl;
            output_queue -> put ( item );
        }
    }
}

void write_output_thread()
{
    int batch_num = 0;
    std::vector < std::string > Q;
    while ( 1 )
    {
        Chunk * item = output_queue -> get ();
        if ( item != NULL )
        {
            Q . push_back ( item -> message );
        }
        if ( Q . size () > 100000 )
        {
            std::cout << "writing to file : " << host . port_no << std::endl;
            sort_data ( Q );
            std::stringstream ss;
            ss << "batch_output/" << "batch_" << host . port_no << "_" << batch_num << ".bat";
            write_file ( ss.str() , Q );
            Q . clear ();
            batch_num++;
        }
    }
}

void consumer_thread()
{
    while(1)
    {
        Chunk * item = (Queue) -> get();
        if ( item == NULL )
        {
            std::cout << "item is NULL ... " << std::endl;
            continue;
        }
        consumeItem(item);
    }
}

std::size_t redirection_ind = 0;
void redirection_thread ( std::string host , std::size_t port , std::vector < std::string > Q ) 
{
    std::cout << "redirection ... " << redirection_ind++ << std::endl;
    boost::asio::io_service svc;
    Client client(svc, host, std::to_string(port));
    std::size_t k(0);
    int batch_count = 100;
    while ( true )
    {
        int count = 0;
        bool done = true;
        std::stringstream ss;
        for ( 
            ; k < Q.size()
            ; ++k
            )
        {
            ss << Q[k] << " ";
            count++;
            if ( batch_count == count )
            {
                client . send ( ss.str () );
                done = false;
                break;
            }
        }
        if ( !done ) continue;
        client . send ( ss.str () );
        break;
    }
}

void consumer_redirection_thread(int queue_index)
{
    std::vector < std::string > Q;
    while(1)
    {
        Chunk * item = ((sorting_queue [queue_index])) -> get ();
        Q . push_back ( item -> message );
        if ( Q . size () > 10000 )
        {
            boost::thread redirection ( redirection_thread
                                      , connections[queue_index].host_name
                                      , connections[queue_index].port_no
                                      , Q
                                      );
            Q . clear ();
        }
    }
}

void client_thread ( std::string host , std::size_t port ) 
{
    boost::asio::io_service svc;
    Client client(svc, host, std::to_string(port));
    client.send("Clinet Testing Connection ... \n");
    client.send("Done Testing ... \n");
}

void sigint(int a)
{
    std::cout << "^C caught!" << std::endl;
    exit(1);
}

int main(int argc,char * argv[])
{
    srand(time(NULL));
    std::cout << "Welcome to HashTableDaemon!" << std::endl;
    std::string config_file;
    if ( argc != 2 )
    {
        std::cout << "try ./HashTableDaemon <config_file>" << std::endl;
        return 1;
    }
    signal(SIGINT,sigint);
    config_file = std::string(argv[1]);
    std::vector < boost::thread * > threads;

    threads . push_back ( new boost::thread { consumer_thread } );

    threads . push_back ( new boost::thread { write_output_thread } );

    parse_config_file ( config_file 
                      , host 
                      , connections 
                      );


    sorting_queue = new ProducerConsumerQueue < Chunk > * [ connections . size () ];
    num_sorting_queue = connections . size ();
    for ( std::size_t k(0)
        ; k < connections.size()
        ; ++k
        )
    {
        sorting_queue[k] =  ( new ProducerConsumerQueue < Chunk > ( BUFFER_SIZE ) );
        threads . push_back ( new boost::thread ( consumer_redirection_thread
                                                , k
                                                )
                            );
    }

    node_index = host . port_no % 10;

    std::cout << "port no : " << host . port_no << std::endl;
    node_server = new Server < Chunk , ProducerConsumerQueue < Chunk > > ( Queue 
                                                                         , host . port_no 
                                                                         );

    sleep(1);//boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

    for ( std::size_t k(0)
        ; k < connections.size()
        ; ++k
        )
    {
        threads . push_back ( new boost::thread ( client_thread
                                                , connections[k].host_name
                                                , connections[k].port_no
                                                )
                            );
    }

    std::cout << "join" << std::endl;
    for ( std::size_t k(0)
        ; k < threads.size()
        ; ++k
        )
    {
        threads[k] -> join ();
    }

    return 0;
}

