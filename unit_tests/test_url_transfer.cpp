#include <boost/thread.hpp>
#include "unit_test.h"
#include "data/data.h"
#include "multithreading/producer_consumer_queue.h"
#include "asio/client.h"
#include "asio/server.h"
#include <iostream>


struct Chunk
{
    std::string message;
    Chunk ( std::string _message )
    : message ( _message )
    {

    }
};

bool test_transfer_urls ()
{

    std::vector < std::string > vec;
    read_file ( "unit_tests/test_seed/seed.que" , vec );

    std::stringstream ss;
    for ( std::size_t k(1)
        ; k < vec.size()
        ; k += 2
        )
    {
        ss << vec[k] << " ";
    }

    int buffer_size = 1000;

    // initialize queue
    ProducerConsumerQueue < Chunk > * Queue = 
    new ProducerConsumerQueue < Chunk > (buffer_size);

    // initialize server
    Server < Chunk , ProducerConsumerQueue < Chunk > > * node_server = 
    new Server < Chunk , ProducerConsumerQueue < Chunk > > ( Queue , 33000 );

    // create client
    boost::asio::io_service svc;
    Client client(svc,"localhost","33000");

    // send message
    client . send ( ss . str () );

    sleep(2);

    // check result
    while ( Queue -> size () )
    {
        Chunk * result = Queue -> get();
        std::cout << "message : " << result -> message << std::endl;
    }

    return true;

}

int main()
{
    std::cout << "Welcome to URL Transfer Unit Test!" << std::endl;
    UnitTest("Test URL Transfer",test_transfer_urls);
    return 0;
}

