#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <iostream>
#include <fstream>
#include <deque>
#include "multithreading/semaphore.h"
#include "asio/network.h"
#include "info.h"

void wait(int seconds)
{
    boost::this_thread::sleep_for(boost::chrono::seconds{seconds});
}

#define BUFFER_SIZE 10

semaphore * emptyCount = new semaphore(BUFFER_SIZE);

semaphore * fillCount = new semaphore(0);

boost::mutex _mutex;

struct Chunk
{
    std::string message;
    Chunk ( std::string _message )
    : message ( _message )
    {

    }
};

std::deque < Chunk * > data;

Chunk * produceItem()
{
    std::stringstream ss;
    ss << "message " << rand() << std::endl;
    return new Chunk( ss.str() );
}

void consumeItem ( Chunk * item )
{
    std::cout << item -> message << std::endl;
    delete item;
}

void putItemIntoBuffer ( Chunk * item )
{
    data . push_back ( item );
}

Chunk * removeItemFromBuffer ()
{
    Chunk * item = data . front ();
    data . pop_front ();
    return item;
}

void producer_thread()
{
    while(1)
    {
        Chunk * item = produceItem();
        emptyCount -> wait ();
        {
            boost::unique_lock<boost::mutex> lock(_mutex);
            putItemIntoBuffer(item);
        }
        fillCount -> signal ();
    }
}

void consumer_thread()
{
    while(1)
    {
        fillCount -> wait ();
        Chunk * item = NULL;
        {
            boost::unique_lock<boost::mutex> lock(_mutex);
            item = removeItemFromBuffer();
        }
        emptyCount -> signal ();
        consumeItem(item);
    }
}

void parse_config_file ( std::string file_name , host_info & host , std::vector < connection_info > & connections )
{
    std::string line;
    std::string token;
    std::ifstream myfile ( file_name.c_str() );
    bool get_host = false;
    std::string host_name;
    std::size_t port_no;
    if ( myfile . is_open () )
    {
        while ( getline ( myfile , line ) )
        {
            std::stringstream ss;
            ss << line;
            ss >> token;
            if ( token . find ( "url" ) != std::string::npos )
            {
                while ( ss >> token )
                {
                    if ( token . find ( "host" ) != std::string::npos )
                    {
                        std::size_t start_id = token . find ( "\'" );
                        std::size_t end_id = token . find ( "\'" , start_id+1 );
                        host_name = token.substr(start_id+1,end_id-start_id-1);
                    }
                    if ( token . find ( "port" ) != std::string::npos )
                    {
                        std::size_t start_id = token . find ( "\'" );
                        std::size_t end_id = token . find ( "\'" , start_id+1 );
                        port_no = (std::size_t)atoi(token.substr(start_id+1,end_id-start_id-1).c_str());
                    }
                }
                if ( !get_host )
                {
                    get_host = true;
                    host = host_info ( host_name , port_no );
                }
                else
                {
                    connections . push_back ( connection_info ( host_name , port_no ) );
                }
            }
        }
        myfile . close ();
    }
    else
    {
        std::cout << "Unable to open file : " << file_name << std::endl;
    }
}

void client_thread ( std::string host , std::size_t port ) 
{
    boost::asio::io_service svc;
    Client client(svc, host, std::to_string(port));

    client.send("hello world\n");
    client.send("bye world\n");
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
    config_file = std::string(argv[1]);
    // std::string line;
    // std::size_t n_threads = 8;
    // std::vector < boost::thread * > threads;
    // for ( std::size_t k(0)
    //     ; k < n_threads
    //     ; ++k
    //     )
    // {
    //     threads . push_back ( new boost::thread { producer_thread } );
    // }
    // std::vector < boost::thread * > consumer_threads;
    // for ( std::size_t k(0)
    //     ; k < n_threads
    //     ; ++k
    //     )
    // {
    //     threads . push_back ( new boost::thread { consumer_thread } );
    // }
    // for ( std::size_t k(0)
    //     ; k < threads.size()
    //     ; ++k
    //     )
    // {
    //     threads[k] -> join ();
    // }

    host_info host;
    std::vector < connection_info > connections;
    parse_config_file ( config_file 
                      , host 
                      , connections 
                      );

    std::cout << "port no : " << host . port_no << std::endl;
    Server * server = new Server ( host . port_no );

    boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

    std::vector < boost::thread * > threads;
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

    for ( std::size_t k(0)
        ; k < threads.size()
        ; ++k
        )
    {
        threads[k] -> join ();
    }

    server -> join ();

    return 0;
}

