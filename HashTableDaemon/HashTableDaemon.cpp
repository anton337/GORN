#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <iostream>
#include <fstream>
#include <deque>
#include "multithreading/producer_consumer_queue.h"
#include "asio/network.h"
#include "info.h"

void wait(int seconds)
{
    sleep(seconds);//boost::this_thread::sleep_for(boost::chrono::seconds{seconds});
}

#define BUFFER_SIZE 10000

std::vector < connection_info > connections;

struct Chunk
{
    std::string message;
    Chunk ( std::string _message )
    : message ( _message )
    {

    }
};

ProducerConsumerQueue < Chunk > * Queue = new ProducerConsumerQueue < Chunk > (BUFFER_SIZE);

ProducerConsumerQueue < Chunk > * * sorting_queue;

std::size_t num_sorting_queue = -1;

std::size_t node_index = -1;

void consumeItem ( Chunk * item )
{
    std::cout << "consumeItem" << std::endl;
    if ( item != NULL )
    {
        std::size_t k = atoi ( item -> message . c_str () );
        std::size_t index = k % num_sorting_queue;
        std::cout << "p1" << std::endl;
        if ( index != node_index )
        {
        std::cout << "p2" << std::endl;
            (sorting_queue [index]) -> put ( item );
        std::cout << "p3" << std::endl;
        }
        else
        {
        std::cout << "p4" << std::endl;
            std::cout << item -> message << std::endl;
            delete item;
        std::cout << "p5" << std::endl;
        }
    }
}

void consumer_thread()
{
    while(1)
    {
        std::cout << "consumer thread" << std::endl;
        Chunk * item = (Queue) -> get();
        if ( item == NULL )
        {
            std::cout << "item is NULL ... " << std::endl;
            continue;
        }
        std::cout << "SUCCESS : " << item -> message << std::endl;
        consumeItem(item);
    }
}

void redirection_thread ( std::string host , std::size_t port ) 
{
    std::cout << "redirection ... " << std::endl;
    boost::asio::io_service svc;
    Client client(svc, host, std::to_string(port));
    client.send("hello world\n");
    // client.send("bye world\n");
}

void consumer_redirection_thread(int queue_index)
{
    std::vector < std::string > Q;
    while(1)
    {
        Chunk * item = ((sorting_queue [queue_index])) -> get ();
        Q . push_back ( item -> message );
        if ( Q . size () > 100 )
        {
            boost::thread redirection ( redirection_thread
                                      , connections[queue_index].host_name
                                      , connections[queue_index].port_no
                                      );
            Q . clear ();
        }
    }
}

void client_thread ( std::string host , std::size_t port ) 
{
    boost::asio::io_service svc;
    Client client(svc, host, std::to_string(port));

    client.send("hello world\n");
    client.send("bye world\n");
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
    std::size_t n_threads = 1;
    std::vector < boost::thread * > threads;

    for ( std::size_t k(0)
        ; k < n_threads
        ; ++k
        )
    {
        threads . push_back ( new boost::thread { consumer_thread } );
    }

    host_info host;
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
    Server < Chunk , ProducerConsumerQueue < Chunk > > * server 
        = new Server < Chunk , ProducerConsumerQueue < Chunk > > ( Queue 
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

