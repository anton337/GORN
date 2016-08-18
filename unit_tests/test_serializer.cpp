#include "unit_test.h"

#include <iostream>
#include "serializers/store_message_serialize.h"
#include "serializers/find_message_serialize.h"
#include "serializers/find_reply_message_serialize.h"

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
    c_find_message . push_host ( host );
    std::string serialized_data = c_find_message . serialize ( 0 , vec . size () );
    return c_find_message . deserialize ( serialized_data ) == 0;
}

bool test_find_reply_message_serializer()
{
    std::vector < bool > vec;
    for ( std::size_t k(0)
        ; k < 10000
        ; ++k
        )
    {
        bool val = rand()%2==1;
        vec . push_back ( val );
    }
    std::cout << std::endl;
    FindReplyMessage c_find_reply_message;
    connection_info host ( "localhost" , 12345 );
    c_find_reply_message . set_data ( vec );
    c_find_reply_message . push_host ( host );
    std::string serialized_data = c_find_reply_message . serialize ( 0 , vec . size () );
    return c_find_reply_message . deserialize ( serialized_data ) == 0;
}

int main()
{
    srand ( time ( NULL ) );
    std::cout << "Welcome to Serializer Unit Test!" << std::endl;
    UnitTest("Test store serialization",test_store_message_serializer);
    UnitTest("Test find serialization" , test_find_message_serializer);
    UnitTest("Test find reply serialization" , test_find_reply_message_serializer);
    return 0;
}

