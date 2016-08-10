#include <boost/thread.hpp>
#include "unit_test.h"
#include "multithreading/producer_consumer_queue.h"
#include "asio/network.h"

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
    while(1)
    {
        Chunk * result = Queue -> get();
        std::cout << "message : " << result -> message << std::endl;
    }

    return true;

}

int main()
{
    srand(time(NULL));
    std::cout << "Running Unit Tests ..." << std::endl;
    UnitTest("Test send message",test_send_message);
    return 0;
}

