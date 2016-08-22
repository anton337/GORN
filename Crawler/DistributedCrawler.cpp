#include <boost/thread.hpp>
#include <iostream>
#include <deque>
#include <set>
#include <exception>
#include "asio/client.h"
#include "multithreading/producer_consumer_queue.h"
#include "data/queue_file.h"
#include "hash/hash.h"

host_info host;

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
node * min_pt = NULL;

int width  = 1000;
int height = 1000;

int mouse_x = 0;
int mouse_y = 0;

double m_mouse_x = 0;
double m_mouse_y = 0;

float camera_x = 0;
float camera_y = 0;
float camera_z = 0;

bool pick = false;

int  get_connections ( std::string host
                     , std::string dir
                     , node * parent
                     , ProducerConsumerQueue < node > * Q
                     , ProducerConsumerQueue < node > * Z
                     )
{
    boost::asio::io_service svc;
    int port = 80;
    Client client(svc, host, std::to_string(port));
    std::string request ( "GET "+dir+" HTTP/1.1\r\nHost: "+host+"\r\nConnection: close\r\n\r\n" );
    //std::cout << request << std::endl;
    std::string output = client . send_complete ( request );
    //std::cout << output << std::endl;
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
                    //std::cout << "found : " << str << std::endl;
                    //std::cout << host << " " << dir << std::endl;
                    if ( Q -> size () < 1000 )
                    {
                        Q -> put ( new node ( host , "/"+dir , parent ) );
                    }
                    else
                    {
                        Z -> put ( new node ( host , "/"+dir , parent ) );
                    }
                }
                else
                {
                    std::string host = str . substr ( host_start , host_end - host_start );
                    //std::cout << "found : " << str << std::endl;
                    //std::cout << host << std::endl;
                    if ( Q -> size () < 1000 )
                    {
                        Q -> put ( new node ( host , "/" , parent ) );
                    }
                    else
                    {
                        Z -> put ( new node ( host , "/"+dir , parent ) );
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

// QueueFile queue_file ( "queue_data/" );

struct node
{
    std::vector < std::string > vec;
    node ( std::vector < std::string > _vec
          )
    : vec ( _vec )
    {

    }
};

ProducerConsumerQueue < node > * map_queue = new ProducerConsumerQueue < node > ( -1 );

void push_map ( std::vector < std::string > const & vec )
{
    boost::asio::io_service svc;
    Client client(svc, host . host, std::to_string(host . port));
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
    delete ss;
}

void save_map ()
{
    std::size_t batch_size = 100;
    std::vector < std::string > vec;
    while ( true )
    {
        node * item = map_queue -> get ();
        vec . push_back ( item -> host + item -> dir );
        delete item;
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
    Client client(svc, host . host, std::to_string(host . port));
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
    delete ss;
}

void save_list ()
{
    std::size_t batch_size = 1000;
    std::vector < std::string > vec;
    while ( true )
    {
        node * item = Z -> get ();
        vec . push_back ( item -> host + item -> dir );
        delete item;
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
                            map_size = M . size ();
                            map_queue -> put ( n );
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
                }
            }
            delete n;
        }
        catch ( std::exception & e )
        {
            std::cout << "Exception : " << e . what () << std::endl;
        }
    }
}

int main(int argc,char **argv)
{
    srand(time(NULL));
    std::cout << "Welcome to Crawler!" << std::endl;
    std::string config_file;
    if ( argc != 2 )
    {
        std::cout << "try ./ClientInterface <config_file>" << std::endl;
        return 1;
    }
    config_file = std::string(argv[1]);

    std::vector < connection_info > connections;
    parse_config_file ( config_file 
                      , host 
                      , connections 
                      );

    std::cout << "port no : " << host . port_no << std::endl;

    Q -> put ( new node ( "www.ask.com" , "/" , NULL ) );
    Q -> put ( new node ( "www.google.com" , "/" , NULL ) );
    Q -> put ( new node ( "www.facebook.com" , "/" , NULL ) );
    Q -> put ( new node ( "www.youtube.com" , "/" , NULL ) );
    Q -> put ( new node ( "www.bing.com" , "/" , NULL ) );
    std::vector < boost::thread * > threads;
    for ( std::size_t k(0)
        ; k < 4
        ; ++k
        )
    {
        threads . push_back ( new boost::thread ( connections_thread ) );
    }
    for ( std::size_t k(0)
        ; k < threads . size ()
        ; ++k
        )
    {
        threads [k] -> join ();
    }
    return 0;
}

