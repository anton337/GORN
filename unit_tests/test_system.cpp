#include <boost/thread.hpp>
#include "unit_test.h"
#include "multithreading/producer_consumer_queue.h"
#include "asio/client.h"
#include "asio/server.h"

#include <stdlib.h>

struct Chunk
{
    std::string message;
    Chunk ( std::string _message )
    : message ( _message )
    {

    }
};

bool test_send_message ()
{
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
    client . send ( "Sending Test Message ... \n" );

    sleep(2);

    // check result
    while ( Queue -> size () )
    {
        Chunk * result = Queue -> get();
        std::cout << "message : " << result -> message << std::endl;
    }

    return true;

}

bool test_send_huge_message ()
{
    int buffer_size = 1000;

    // initialize queue
    ProducerConsumerQueue < Chunk > * Queue = 
    new ProducerConsumerQueue < Chunk > (buffer_size);

    // initialize server
    Server < Chunk , ProducerConsumerQueue < Chunk > > * node_server = 
    new Server < Chunk , ProducerConsumerQueue < Chunk > > ( Queue , 33001 );

    // create client
    boost::asio::io_service svc;
    Client client(svc,"localhost","33001");

    // construct huge message
    std::stringstream ss;
    for ( std::size_t k(0)
        ; k < 1000
        ; ++k
        )
    {
        ss << k << " ";
    }

    // send message
    client . send ( ss . str () );

    sleep(2);

    // check result
    std::size_t count = 0;
    while ( Queue -> size () )
    {
        Chunk * result = Queue -> get();
        std::cout << "message : " << count++ << "   " << result -> message << std::endl;
    }

    return true;

}

bool test_send_two_huge_messages_to_same_server ()
{
    int buffer_size = 1000;

    // initialize queue
    ProducerConsumerQueue < Chunk > * Queue = 
    new ProducerConsumerQueue < Chunk > (buffer_size);

    // initialize server
    Server < Chunk , ProducerConsumerQueue < Chunk > > * node_server = 
    new Server < Chunk , ProducerConsumerQueue < Chunk > > ( Queue , 33002 );

    // create client
    boost::asio::io_service svc1;
    Client client1(svc1,"localhost","33002");

    // construct huge message
    std::stringstream ss1;
    for ( std::size_t k(0)
        ; k < 10000
        ; ++k
        )
    {
        ss1 << k << " ";
    }

    boost::asio::io_service svc2;
    Client client2(svc2,"localhost","33002");

    // construct huge message
    std::stringstream ss2;
    for ( std::size_t k(0)
        ; k < 10000
        ; ++k
        )
    {
        ss2 << k << " ";
    }

    // send message
    client1 . send ( ss1 . str () );
    client2 . send ( ss2 . str () );

    sleep(2);

    // check result
    std::size_t count = 0;
    while ( Queue -> size () )
    {
        Chunk * result = Queue -> get();
        std::cout << "message : " << count++ << "   " << result -> message << std::endl;
    }

    return true;

}

int main()
{
    srand(time(NULL));
    std::cout << "Running Unit Tests ..." << std::endl;
    UnitTest("Test send message",test_send_message);
    UnitTest("Test send huge message",test_send_huge_message);
    UnitTest("Test send two huge messages to same server",test_send_two_huge_messages_to_same_server);
    return 0;
}

