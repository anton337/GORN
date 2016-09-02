#include <boost/thread.hpp>
#include <iostream>
#include <deque>
#include <set>
#include <exception>
#include "asio/client.h"
#include "multithreading/producer_consumer_queue.h"
#include "data/queue_file.h"
#include "hash/hash.h"
#include "info.h"
#include "serializers/store_message_serialize.h"
#include "serializers/find_message_serialize.h"

host_info server_host;

struct node
{
    std::string host;
    std::string dir;
    node ( std::string _host
         , std::string _dir
         )
    : host ( _host )
    , dir  ( _dir  )
    {

    }
};

struct NodeComparator 
{
    bool operator () ( node * a , node * b )
    {
        return a -> host < b -> host || a -> dir < b -> dir ;
    }
};

boost::mutex * _mutex = new boost::mutex ();
std::set < node * , NodeComparator > M;
ProducerConsumerQueue < node > * Q = new ProducerConsumerQueue < node > ( -1 );
ProducerConsumerQueue < node > * Z = new ProducerConsumerQueue < node > ( -1 );

int  get_connections ( std::string host
                     , std::string dir
                     , node * parent
                     , ProducerConsumerQueue < node > * Q
                     , ProducerConsumerQueue < node > * Z
                     )
{
    HashGenerator hash;
    boost::asio::io_service svc;
    int port = 80;
    Client client(svc, host , std::to_string ( port ) );
    std::string request ( "GET "+dir+" HTTP/1.1\r\nHost: "+host+"\r\nConnection: close\r\n\r\n" );
    std::string output = client . send_complete ( request );
    std::size_t n_links = 0;
    {
        std::size_t pos = 0;
        std::size_t href_pos = 0;
        while ( (pos = output . find ( "<a " , pos+1 )) != std::string::npos )
        {
            n_links++;
            href_pos = output . find ( "href=" , pos );
            std::string str = output . substr ( href_pos + 6 , output . find ( "\"" , href_pos + 8 ) - 6 - href_pos );
            std::size_t host_start = str . find ( "/" , 0          ) + 2;
            std::size_t host_end   = str . find ( "/" , host_start );
            if (  host_start != std::string::npos
               )
            {
                if ( host_end != std::string::npos
                   )
                {
                    std::string host = str . substr ( host_start , host_end - host_start );
                    std::string dir  = str . substr ( host_end+1 );
                    if ( hash . validate ( host + "/"+dir ) )
                    {
                        if ( Q -> size () < 1000 )
                        {
                            Q -> put ( new node ( host , "/"+dir ) );
                        }
                        else
                        {
                            Z -> put ( new node ( host , "/"+dir ) );
                        }
                    }
                }
                else
                {
                    std::string host = str . substr ( host_start , host_end - host_start );
                    if ( hash . validate ( host ) )
                    {
                        if ( Q -> size () < 1000 )
                        {
                            Q -> put ( new node ( host , "/" ) );
                        }
                        else
                        {
                            Z -> put ( new node ( host , "/" ) );
                        }
                    }
                }
            }
            else
            {
                //std::cout << "not found : " << str << std::endl;
            }
        }
    }
    return n_links;
}

int  get_connections_fake ( std::string host
                          , std::string dir
                          , node * parent
                          , ProducerConsumerQueue < node > * Q
                          , ProducerConsumerQueue < node > * Z
                          )
{
    std::size_t n_links ( (rand()%100==1)?10000:rand()%3 );
    for ( std::size_t k(0)
        ; k < n_links
        ; ++k
        )
    {
        std::stringstream host_ss;
        host_ss << host << rand();
        std::string host ( host_ss . str () );
        std::stringstream dir_ss;
        dir_ss << dir << rand();
        std::string dir ( dir_ss . str () );
        if ( Q -> size () < 1000 )
        {
            Q -> put ( new node ( host , dir ) );
        }
        else
        {
            Z -> put ( new node ( host , dir ) );
        }
    }
    return n_links;
}

ProducerConsumerQueue < node > * map_queue = new ProducerConsumerQueue < node > ( -1 );

void push_map ( std::vector < std::string > const & vec )
{
    boost::asio::io_service svc;
    Client client(svc, server_host . host_name , std::to_string( server_host . port_no ));
    int batch_count = 100;
    std::size_t k(0);
    while ( true )
    {
        int count = 0;
        bool done = true;
        std::vector < std::string > cpy;
        for (
            ; k < vec . size ()
            ; ++k
            )
        {
            cpy . push_back ( vec[k] );
            count++;
            if ( batch_count == count )
            {
                StoreMessage message;
                message . set_data ( cpy );
                client . send ( message . serialize ( 0 , cpy . size () ) );
                done = false;
                usleep(10000);
                break;
            }
        }
        if ( !done ) continue;
        StoreMessage message;
        message . set_data ( cpy );
        client . send ( message . serialize ( 0 , cpy . size () ) );
        usleep(10000);
        break;
    }
}

void save_map ()
{
    std::size_t batch_size = 100;
    std::vector < std::string > vec;
    while ( true )
    {
        node * item = map_queue -> get ();
        vec . push_back ( item -> host + item -> dir );
        // delete item;
        if ( vec . size () > batch_size )
        {
            push_map ( vec );
            vec . clear ();
        }
    }
}

void push_list ( std::vector < std::string > const & vec )
{
    boost::asio::io_service svc;
    Client client(svc, server_host . host_name , std::to_string ( server_host . port_no ) );
    int batch_count = 100;
    std::size_t k(0);
    while ( true )
    {
        int count = 0;
        bool done = true;
        std::vector < std::string > cpy;
        for (
            ; k < vec . size ()
            ; ++k
            )
        {
            cpy . push_back ( vec[k] );
            count++;
            if ( batch_count == count )
            {
                FindMessage message;
                message . set_data ( cpy );
                client . send ( message . serialize ( 0 , cpy . size () ) );
                done = false;
                usleep(10000);
                break;
            }
        }
        if ( !done ) continue;
        FindMessage message;
        message . set_data ( cpy );
        client . send ( message . serialize ( 0 , cpy . size () ) );
        usleep(10000);
        break;
    }
}

void save_list ()
{
    std::size_t batch_size = 1000;
    std::vector < std::string > vec;
    while ( true )
    {
        node * item = Z -> get ();
        vec . push_back ( item -> host + item -> dir );
        // delete item;
        if ( vec . size () > batch_size )
        {
            push_list ( vec );
            vec . clear ();
        }
    }
}

void connections_thread ()
{
    HashGenerator hash;
    while ( true )
    {
        try 
        {
            node * n = Q -> get ();
            if ( hash . validate ( n -> host + n -> dir ) )
            {
                bool proceed = false;
                {
                    boost::unique_lock < boost::mutex > lock ( *_mutex );
                    if ( M . find ( n ) == M . end () )
                    {
                        std::size_t size_prev = M . size ();
                        M . insert ( n );
                        std::size_t size_next = M . size ();
                        if ( size_next > size_prev )
                        {
                            proceed = true;
                            std::cout << n -> host << n->dir << " : " << Q -> size () << " : " << M . size () << std::endl;
                        }
                    }
                }
                if ( proceed )
                {
                    int n_connections = get_connections ( n -> host 
                                                        , n -> dir
                                                        , n
                                                        , Q
                                                        , Z
                                                        );
                    std::cout << "n_connections : " << n_connections << std::endl;
                    if ( n_connections > 0 )
                    {
                        map_queue -> put ( n );
                    }
                }
            }
            // delete n;
        }
        catch ( std::exception & e )
        {
            std::cout << "Exception : " << e . what () << std::endl;
        }
    }
}

QueueFile < QueueEntryValue > queue_file ( "queue_data/" );

void fetch_data_from_queue_thread ()
{
    while ( true )
    {
        try 
        {
            if ( Q -> size () < 200 )
            {
                std::vector < QueueEntryValue > vec;
                queue_file . Pop ( vec );
                for ( std::size_t k(0)
                    ; k < vec . size ()
                    ; ++k
                    )
                {
                    std::string str = vec[k] . value;
                    std::cout << "fetching item : " << vec[k] . value << std::endl;
                    std::size_t host_start = 0;
                    std::size_t host_end   = str . find ( "/" , host_start );
                    if ( host_start != std::string::npos
                       )
                    {
                        if ( host_end != std::string::npos
                           )
                        {
                            std::string host = str . substr ( host_start , host_end - host_start );
                            std::string dir  = str . substr ( host_end+1 );
                            std::cout << "host : " << host << " dir : " << dir << std::endl;
                            {
                                Q -> put ( new node ( host , "/"+dir ) );
                            }
                        }
                        else
                        {
                            std::string host = str . substr ( host_start , host_end - host_start );
                            std::cout << "host : " << std::endl;
                            {
                                Q -> put ( new node ( host , "/" ) );
                            }
                        }
                    }
                }
            }
        }
        catch ( std::exception & e )
        {
            std::cout << "Exception : " << e . what () << std::endl;
        }
    }
}

void set_status_thread ()
{
    while ( true )
    {
        std::stringstream filename;
        filename << "system_status/crawler/" << "crawler_info____" << server_host . host_name << "_" << server_host . port_no;
        std::ofstream output_file;
        output_file . open ( filename . str () . c_str () );
        if ( output_file . is_open () )
        {
            output_file << "Q_size: " << Q -> size () << std::endl;
            output_file << "Z_size: " << Z -> size () << std::endl;
            output_file << "map_queue_size: " << map_queue -> size () << std::endl;
            output_file << "M_size: " << M . size () << std::endl;
            output_file . close ();
        }
        else
        {
            std::cout << "Unable to open file : " << filename . str () << std::endl;
        }
        usleep(1000000);
    }
}

int main(int argc,char **argv)
{
    srand(time(NULL));
    std::cout << "Welcome to Crawler!" << std::endl;
    std::string config_file;
    std::string seed_name;
    if ( argc != 3 )
    {
        std::cout << "try ./ClientInterface <config_file> <seed_name>" << std::endl;
        return 1;
    }
    config_file = std::string(argv[1]);
    seed_name = std::string(argv[2]);

    std::vector < connection_info > connections;
    parse_config_file ( config_file 
                      , server_host 
                      , connections 
                      );

    std::cout << "host name : " << server_host . host_name << std::endl;
    std::cout << "port no : " << server_host . port_no << std::endl;

    Q -> put ( new node ( seed_name , "/" ) );

    std::vector < boost::thread * > threads;
    for ( std::size_t k(0)
        ; k < 40
        ; ++k
        )
    {
        threads . push_back ( new boost::thread ( connections_thread ) );
    }
    threads . push_back ( new boost::thread ( save_list                    ) );
    threads . push_back ( new boost::thread ( fetch_data_from_queue_thread ) );
    threads . push_back ( new boost::thread ( save_map                     ) );
    threads . push_back ( new boost::thread ( set_status_thread            ) );
    for ( std::size_t k(0)
        ; k < threads . size ()
        ; ++k
        )
    {
        threads [k] -> join ();
    }
    return 0;
}

