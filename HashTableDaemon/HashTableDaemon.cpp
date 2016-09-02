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
#include "serializers/store_message_serialize.h"
#include "serializers/find_message_serialize.h"
#include "serializers/system_status_serialize.h"
#include "data/distributed_mutex.h"
#include "system/get_tot_cpu_info.h"
#include "system/memory.h"

void wait(int seconds)
{
    sleep(seconds);//boost::this_thread::sleep_for(boost::chrono::seconds{seconds});
}

#define BUFFER_SIZE -1

host_info host;

std::vector < connection_info > connections;

struct Chunk
{
    std::string message;
    Chunk ( std::string _message 
          )
    : message ( _message )
    {

    }
};

Server < Chunk , ProducerConsumerQueue < Chunk > > * node_server = NULL;

ProducerConsumerQueue < Chunk > * Queue = new ProducerConsumerQueue < Chunk > (BUFFER_SIZE);

ProducerConsumerQueue < Chunk > * * sorting_queue;

ProducerConsumerQueue < Chunk > * * find_sorting_queue;

ProducerConsumerQueue < Chunk > * output_queue = new ProducerConsumerQueue < Chunk > (BUFFER_SIZE);

ProducerConsumerQueue < Chunk > * find_output_queue = new ProducerConsumerQueue < Chunk > (BUFFER_SIZE);

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

InfoPackage construct_status_message ();

void consumeItem ( Chunk * item )
{
    if ( item != NULL )
    {
        switch ( Serialize :: extract_type ( item -> message ) )
        {
            case STORE_TYPE :
            {
                StoreMessage message;
                if ( message . deserialize ( item -> message ) != 0 )
                {
                    std::cout << "failed to deserialize store message " << std::endl;
                    break;
                }
                std::vector < std::string > data = message . get_data ();
                for ( std::size_t i(0)
                    ; i < data.size()
                    ; ++i
                    )
                {
                    std::size_t k = atoi ( data[i] . c_str () );
                    std::size_t index = k % 4; // num_sorting_queue;
                    if ( index != node_index )
                    {
                        std::cout << "index : " << index << std::endl;
                        std::size_t queue_index = find_chord_connection ( index );
                        (sorting_queue [queue_index]) -> put ( new Chunk ( data[i] ) );
                    }
                    else
                    {
                        std::cout << "store : " << host.port_no << " - " << index << " : " << ++num_received <<  " : " << data[i] << std::endl;
                        output_queue -> put ( new Chunk ( data[i] ) );
                    }
                }
                break;
            }
            case FIND_TYPE :
            {
                FindMessage message;
                if ( message . deserialize ( item -> message ) != 0 )
                {
                    std::cout << "failed to deserialize find message " << std::endl;
                    break;
                }
                std::vector < std::string > data = message . get_data ();
                for ( std::size_t i(0)
                    ; i < data.size()
                    ; ++i
                    )
                {
                    std::size_t k = atoi ( data[i] . c_str () );
                    std::size_t index = k % 4; // num_sorting_queue;
                    if ( index != node_index )
                    {
                        std::size_t queue_index = find_chord_connection ( index );
                        (find_sorting_queue [queue_index]) -> put ( new Chunk ( data[i] ) );
                    }
                    else
                    {
                        std::cout << "find : " << host.port_no << " - " << ++num_received <<  " : " << data[i] << std::endl;
                        find_output_queue -> put ( new Chunk ( data[i] ) );
                    }
                }
                break;
            }
            case SYSTEM_STATUS_TYPE :
            {
                std::cout << "Receiving system status report ... " << std::endl;
                SystemStatusMessage < InfoPackage > report;
                if ( report . deserialize ( item -> message ) != 0 )
                {
                    std::cout << "failed to deserialize status message " << std::endl;
                    break;
                }
                std::cout << "Status Report : " << item -> message << std::endl;
                break;
            }
            case GET_SYSTEM_STATUS_TYPE :
            {
                std::cout << "Get system status request ... " << std::endl;
                InfoPackage status ( construct_status_message () );
                std::stringstream ss;
                ss << item -> message;
                std::string type;
                ss >> type;
                std::string host;
                ss >> host;
                std::string port_s;
                ss >> port_s;
                boost::asio::io_service svc;
                Client client ( svc, host, port_s );
                SystemStatusMessage < InfoPackage > message;
                message . set_data ( status );
                client . send ( message . serialize ( 0 , 0 ) );
                std::cout << "sending status ... " << std::endl;
                break;
            }
            default :
            {
                // message type not recognized 
                std::cout << item -> message << std::endl;
                break;
            }
        }
    }
}

DistributedMutex c_mutex ( "sorted_output/unlocked"
                         , "sorted_output/locked"
                         );

void find_write_output_thread()
{
    int batch_num = 0;
    std::vector < std::string > Q;
    while ( 1 )
    {
        Chunk * item = find_output_queue -> get ();
        if ( item != NULL )
        {
            Q . push_back ( item -> message );
        }
        delete item;
        item = NULL;
        if ( Q . size () > 100 /*100000*/ )
        {
            if ( c_mutex . Lock () == 1 ) continue;
            std::cout << "writing to file : " << host . port_no << std::endl;
            sort_data ( Q );
            std::stringstream map_ss;
            map_ss << "sorted_output/" << "comprehensive.out";
            std::stringstream output_ss;
            output_ss << "queue_data/" << "queue_" << host . port_no << "_" << batch_num << ".que";
            search_new ( map_ss . str () 
                       , Q 
                       , output_ss . str () 
                       );
            Q . clear ();
            batch_num++;
            c_mutex . Unlock ();
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
        if ( Q . size () > 100 /*100000*/ )
        {
            std::cout << "writing to file : " << host . port_no << std::endl;
            sort_data ( Q );
            std::stringstream ss;
            ss << "batch_output/" << "batch_" << host . port_no << "_" << batch_num << ".bat";
            write_file ( ss.str() , Q );
            Q . clear ();
            batch_num++;
        }
        delete item;
        item = NULL;
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
        delete item;
        item = NULL;
    }
}

std::size_t redirection_ind = 0;
void redirection_thread ( std::string host 
                        , std::size_t port 
                        , std::vector < std::string > Q 
                        ) 
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
        std::vector < std::string > cpy;
        for ( 
            ; k < Q.size()
            ; ++k
            )
        {
            cpy . push_back ( Q[k] );
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

std::size_t find_redirection_ind = 0;
void find_redirection_thread ( std::string host 
                             , std::size_t port 
                             , std::vector < std::string > Q 
                             ) 
{
    std::cout << "find redirection ... " << find_redirection_ind++ << std::endl;
    boost::asio::io_service svc;
    Client client(svc, host, std::to_string(port));
    std::size_t k(0);
    int batch_count = 100;
    while ( true )
    {
        int count = 0;
        bool done = true;
        std::vector < std::string > cpy;
        for ( 
            ; k < Q.size()
            ; ++k
            )
        {
            cpy . push_back ( Q[k] );
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

void find_consumer_redirection_thread(int queue_index)
{
    std::vector < std::string > Q;
    while(1)
    {
        Chunk * item = ((find_sorting_queue [queue_index])) -> get ();
        Q . push_back ( item -> message );
        if ( Q . size () > 10000 )
        {
            boost::thread redirection ( find_redirection_thread
                                      , connections[queue_index].host_name
                                      , connections[queue_index].port_no
                                      , Q
                                      );
            Q . clear ();
        }
        delete item;
        item = NULL;
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
        delete item;
        item = NULL;
    }
}

void client_testing_connection_thread ( std::string host , std::size_t port ) 
{
    boost::asio::io_service svc;
    Client client(svc, host, std::to_string(port));
    client.send("Client Testing Connection ... \n");
    client.send("Done Testing ... \n");
}

void sigint(int a)
{
    std::cout << "^C caught!" << std::endl;
    exit(1);
}

void set_status_thread ()
{
    while ( true )
    {
        std::stringstream filename;
        filename << "system_status/hash_table/" << "hash_table_info____" << host . host_name << "_" << host . port_no;
        std::ofstream output_file;
        output_file . open ( filename . str () . c_str () );
        if ( output_file . is_open () )
        {
            output_file << "queue_size: " << Queue -> size () << std::endl;
            for ( std::size_t k(0)
                ; k < 4
                ; ++k
                )
            {
                output_file << "sorting_queue_size" << k << ": " << sorting_queue[k] -> size () << std::endl;
            }
            for ( std::size_t k(0)
                ; k < 4
                ; ++k
                )
            {
                output_file << "find_sorting_queue_size" << k << ": " << find_sorting_queue[k] -> size () << std::endl;
            }
            {
                output_file << "output_queue_size" << ": " << output_queue -> size () << std::endl;
            }
            {
                output_file << "find_output_queue_size" << ": " << find_output_queue -> size () << std::endl;
            }
            output_file . close ();
        }
        else
        {
            std::cout << "Unable to open file : " << filename . str () << std::endl;
        }
        sleep(1);
    }
}

InfoPackage construct_status_message ()
{
    InfoPackage info;

    // get crawler info
    {
        std::vector < std::string > crawler_files;
        get_files ( "system_status/crawler/" , crawler_files );
        for ( std::size_t k (0)
            ; k < crawler_files . size ()
            ; ++k
            )
        {
            CrawlerInfo crawler_info;
            std::string line;
            std::ifstream file ( crawler_files[k] );
            if ( file . is_open () )
            {
                {
                    getline ( file , line );
                    float queue_size;
                    std::string label;
                    std::stringstream ss;
                    ss << line;
                    ss >> label;
                    ss >> queue_size;
                    crawler_info . Q_size = queue_size;
                }
                {
                    getline ( file , line );
                    float queue_size;
                    std::string label;
                    std::stringstream ss;
                    ss << line;
                    ss >> label;
                    ss >> queue_size;
                    crawler_info . Z_size = queue_size;
                }
                {
                    getline ( file , line );
                    float queue_size;
                    std::string label;
                    std::stringstream ss;
                    ss << line;
                    ss >> label;
                    ss >> queue_size;
                    crawler_info . map_queue_size = queue_size;
                }
                {
                    getline ( file , line );
                    float queue_size;
                    std::string label;
                    std::stringstream ss;
                    ss << line;
                    ss >> label;
                    ss >> queue_size;
                    crawler_info . M_size = queue_size;
                }
                info . m_crawler_info . push_back ( crawler_info );
            }
            else
            {
                std::cout << "Unable to open file : " << crawler_files[k] << std::endl;
            }
        }
    }

    // get hash table info
    {
        std::vector < std::string > hash_table_files;
        get_files ( "system_status/hash_table/" , hash_table_files );
        for ( std::size_t k (0)
            ; k < hash_table_files . size ()
            ; ++k
            )
        {
            HashTableInfo hash_table_info;
            std::string line;
            std::ifstream file ( hash_table_files[k] );
            if ( file . is_open () )
            {
                {
                    getline ( file , line );
                    float queue_size;
                    std::string label;
                    std::stringstream ss;
                    ss << line;
                    ss >> label;
                    ss >> queue_size;
                    hash_table_info . queue_size = queue_size;
                }
                for ( std::size_t i(0)
                    ; i < 4
                    ; ++i
                    )
                {
                    getline ( file , line );
                    float queue_size;
                    std::string label;
                    std::stringstream ss;
                    ss << line;
                    ss >> label;
                    ss >> queue_size;
                    hash_table_info . sorting_queue_size . push_back ( queue_size );
                }
                for ( std::size_t i(0)
                    ; i < 4
                    ; ++i
                    )
                {
                    getline ( file , line );
                    float queue_size;
                    std::string label;
                    std::stringstream ss;
                    ss << line;
                    ss >> label;
                    ss >> queue_size;
                    hash_table_info . find_sorting_queue_size . push_back ( queue_size );
                }
                {
                    getline ( file , line );
                    float queue_size;
                    std::string label;
                    std::stringstream ss;
                    ss << line;
                    ss >> label;
                    ss >> queue_size;
                    hash_table_info . output_queue_size . push_back ( queue_size );
                }
                {
                    getline ( file , line );
                    float queue_size;
                    std::string label;
                    std::stringstream ss;
                    ss << line;
                    ss >> label;
                    ss >> queue_size;
                    hash_table_info . find_output_queue_size . push_back ( queue_size );
                }
                info . m_hash_table_info . push_back ( hash_table_info );
            }
            else
            {
                std::cout << "Unable to open file : " << hash_table_files[k] << std::endl;
            }
        }
    }

    // get system info
    {
        double percent[5];
        memoryInfo c_memoryInfo;
        getCurrentValue(percent);
        std::size_t mem_total;
        std::size_t mem_used;
        std::size_t mem_cache;
        std::size_t mem_buffers;
        c_memoryInfo . get_memory_info ( mem_total 
                                       , mem_used
                                       , mem_cache
                                       , mem_buffers
                                       );
        for ( std::size_t k(1)
            ; k < 5
            ; ++k
            )
        {
            info . m_system_info . p_cpu_percent . push_back ( percent[k] );
        }
        info . m_system_info . p_memory_percent = 100 * (float)mem_used / (float)mem_total;
    }

    return info;
}

int main(int argc,char * argv[])
{

    srand(time(NULL));
    std::cout << "Welcome to HashTableDaemon!" << std::endl;

    #ifdef DEBUG
    std::cout << "[DEBUG MODE]" << "   " << "PID:" << getpid() << std::endl;;
    #endif

    std::string config_file;
    if ( argc != 2 )
    {
        std::cout << "try ./HashTableDaemon <config_file>" << std::endl;
        return 1;
    }

    init();

    config_file = std::string(argv[1]);
    std::vector < boost::thread * > threads;

    threads . push_back ( new boost::thread { consumer_thread } );

    threads . push_back ( new boost::thread { write_output_thread } );

    parse_config_file ( config_file 
                      , host 
                      , connections 
                      );


    sorting_queue      = new ProducerConsumerQueue < Chunk > * [ connections . size () ];
    find_sorting_queue = new ProducerConsumerQueue < Chunk > * [ connections . size () ];
    num_sorting_queue = connections . size ();
    for ( std::size_t k(0)
        ; k < connections.size()
        ; ++k
        )
    {
        sorting_queue      [k] =  ( new ProducerConsumerQueue < Chunk > ( BUFFER_SIZE ) );
        find_sorting_queue [k] =  ( new ProducerConsumerQueue < Chunk > ( BUFFER_SIZE ) );
        threads . push_back ( new boost::thread ( consumer_redirection_thread
                                                , k
                                                )
                            );
        threads . push_back ( new boost::thread ( find_consumer_redirection_thread
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
        threads . push_back ( new boost::thread ( client_testing_connection_thread
                                                , connections[k].host_name
                                                , connections[k].port_no
                                                )
                            );
    }

    threads . push_back ( new boost::thread ( find_write_output_thread ) );

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

