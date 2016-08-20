#include "unit_test.h"

#include <iostream>
#include "serializers/store_message_serialize.h"
#include "serializers/find_message_serialize.h"

#include "multithreading/producer_consumer_queue.h"
#include "asio/client.h"
#include "asio/server.h"

bool test_store_message_serializer()
{
    std::vector < std::string > vec;
    for ( std::size_t k(0)
        ; k < 10000
        ; ++k
        )
    {
        std::stringstream ss;
        ss << rand();
        vec . push_back ( ss.str() );
    }
    std::cout << std::endl;
    StoreMessage c_store_message;
    c_store_message . set_data ( vec );
    std::string serialized_data = c_store_message . serialize ( 0 , vec . size () );
    return c_store_message . deserialize ( serialized_data ) == 0;
}

bool test_find_message_serializer()
{
    std::vector < std::string > vec;
    for ( std::size_t k(0)
        ; k < 10000
        ; ++k
        )
    {
        std::stringstream ss;
        ss << rand();
        vec . push_back ( ss.str() );
    }
    std::cout << std::endl;
    FindMessage c_find_message;
    connection_info host ( "localhost" , 12345 );
    c_find_message . set_data ( vec );
    std::string serialized_data = c_find_message . serialize ( 0 , vec . size () );
    return c_find_message . deserialize ( serialized_data ) == 0;
}

struct Chunk
{
    std::string message;
    Chunk ( std::string _message )
    : message ( _message )
    {

    }
};

bool test_serializer_over_network()
{
    std::vector < std::string > vec;
    for ( std::size_t k(0)
        ; k < 10
        ; ++k
        )
    {
        std::stringstream ss;
        ss << rand();
        vec . push_back ( ss.str() );
    }
    std::cout << std::endl;
    StoreMessage c_store_message;
    c_store_message . set_data ( vec );
    std::string serialized_data = c_store_message . serialize ( 0 , vec . size () );

    // initialize queue
    std::size_t buffer_size = 10000;
    ProducerConsumerQueue < Chunk > * Queue = 
    new ProducerConsumerQueue < Chunk > (buffer_size);

    // initialize server
    Server < Chunk , ProducerConsumerQueue < Chunk > > * node_server = 
    new Server < Chunk , ProducerConsumerQueue < Chunk > > ( Queue , 33000 );

    // create client
    boost::asio::io_service svc;
    Client client(svc,"localhost","33000");

    // send message
    client . send ( serialized_data );

    sleep(2);

    // check result
    while ( Queue -> size () )
    {
        Chunk * result = Queue -> get();
        std::cout << "message : " << result -> message << std::endl;
        c_store_message . deserialize ( result -> message );
    }

    return true;

}

bool test_huge_file_serializer_over_network()
{
    std::vector < std::string > vec;
    for ( std::size_t k(0)
        ; k < 10000
        ; ++k
        )
    {
        std::stringstream ss;
        ss << rand();
        vec . push_back ( ss.str() );
    }
    std::cout << std::endl;
    StoreMessage c_store_message;
    c_store_message . set_data ( vec );
    std::string serialized_data = c_store_message . serialize ( 0 , vec . size () );

    // initialize queue
    std::size_t buffer_size = 10000;
    ProducerConsumerQueue < Chunk > * Queue = 
    new ProducerConsumerQueue < Chunk > (buffer_size);

    // initialize server
    Server < Chunk , ProducerConsumerQueue < Chunk > > * node_server = 
    new Server < Chunk , ProducerConsumerQueue < Chunk > > ( Queue , 33001 );

    // create client
    boost::asio::io_service svc;
    Client client(svc,"localhost","33001");

    // send message
    client . send ( serialized_data );

    sleep(2);

    // check result
    while ( Queue -> size () )
    {
        Chunk * result = Queue -> get();
        std::cout << "message : " << result -> message << std::endl;
        c_store_message . deserialize ( result -> message );
    }

    return true;

}

int main()
{
    srand ( time ( NULL ) );
    std::cout << "Welcome to Serializer Unit Test!" << std::endl;
    UnitTest("Test store serialization",test_store_message_serializer);
    UnitTest("Test find serialization" , test_find_message_serializer);
    UnitTest("Test serializer over network" , test_serializer_over_network);
    UnitTest("Test huge file serializer over network" , test_huge_file_serializer_over_network);
    return 0;
}

