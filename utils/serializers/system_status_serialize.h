#ifndef SYSTEM_STATUS_SERIALIZE_H
#define SYSTEM_STATUS_SERIALIZE_H

#include "serialize.h"

#include <vector>
#include <sstream>

template < typename T >
void serialize_t ( std::vector < T >     const & in 
                 , std::vector < float >       & vec 
                 )
{
    vec . push_back ( in . size () );
    for ( std::size_t k(0)
        ; k < in . size ()
        ; ++k
        )
    {
        vec . push_back ( in[k] );
    }
}

template < typename T >
void deserialize_t ( std::stringstream & ss 
                   , std::stringstream & ss_check_sum
                   , std::vector < T > & vec
                   )
{
    std::size_t num;
    ss >> num;
    vec . clear ();
    for ( std::size_t k(0)
        ; k < num
        ; ++k
        )
    {
        T val;
        ss >> val;
        vec . push_back ( val );
    }
}

struct SystemInfo
{
    std::vector < float >   p_cpu_percent    ;
    float                   p_memory_percent ;
    void serialize ( std::vector < float > & vec )
    {
        serialize_t ( p_cpu_percent , vec );
        vec . push_back ( p_memory_percent );
    }
    void deserialize ( std::stringstream & ss 
                     , std::stringstream & ss_check_sum
                     )
    {
        deserialize_t ( ss , ss_check_sum , p_cpu_percent );
        ss >> p_memory_percent;
    }
};

struct HashTableInfo
{
    std::size_t queue_size;
    std::vector < std::size_t > sorting_queue_size;
    std::vector < std::size_t > find_sorting_queue_size;
    std::vector < std::size_t > output_queue_size;
    std::vector < std::size_t > find_output_queue_size;
    void serialize ( std::vector < float > & vec )
    {
        vec . push_back ( queue_size );
        serialize_t ( sorting_queue_size , vec );
        serialize_t ( find_sorting_queue_size , vec );
        serialize_t ( output_queue_size , vec );
        serialize_t ( find_output_queue_size , vec );
    }
    void deserialize ( std::stringstream & ss 
                     , std::stringstream & ss_check_sum
                     )
    {
        ss >> queue_size;
        deserialize_t ( ss , ss_check_sum ,      sorting_queue_size );
        deserialize_t ( ss , ss_check_sum , find_sorting_queue_size );
        deserialize_t ( ss , ss_check_sum ,       output_queue_size );
        deserialize_t ( ss , ss_check_sum ,  find_output_queue_size );
    }
};

struct CrawlerInfo
{
    std::size_t Q_size;
    std::size_t Z_size;
    std::size_t map_queue_size;
    std::size_t M_size;
    void serialize ( std::vector < float > & vec )
    {
        vec . push_back ( Q_size );
        vec . push_back ( Z_size );
        vec . push_back ( map_queue_size );
        vec . push_back ( M_size );
    }
    void deserialize ( std::stringstream & ss 
                     , std::stringstream & ss_check_sum 
                     )
    {
        ss >> Q_size;
        ss >> Z_size;
        ss >> map_queue_size;
        ss >> M_size;
    }
};

struct InfoPackage
{

    SystemInfo                          m_system_info;

    std::vector < HashTableInfo >   m_hash_table_info;

    std::vector < CrawlerInfo >        m_crawler_info;

    std::vector < float > serialize ( )
    {
        std::vector < float > vec;
        m_system_info . serialize ( vec );
        vec . push_back ( m_hash_table_info . size () );
        for ( std::size_t k(0)
            ; k < m_hash_table_info . size ()
            ; ++k
            )
        {
            m_hash_table_info[k] . serialize ( vec );
        }
        vec . push_back ( m_crawler_info . size () );
        for ( std::size_t k(0)
            ; k < m_crawler_info . size ()
            ; ++k
            )
        {
            m_crawler_info[k] . serialize ( vec );
        }
        return vec;
    }

    void deserialize ( std::stringstream & ss 
                     , std::stringstream & ss_check_sum 
                     )
    {
        m_system_info . deserialize ( ss , ss_check_sum );
        std::size_t num_hash_table;
        ss >> num_hash_table;
        m_hash_table_info . clear ();
        for ( std::size_t k(0)
            ; k < num_hash_table
            ; ++k
            )
        {
            m_hash_table_info . push_back ( HashTableInfo () );
            m_hash_table_info[k] . deserialize ( ss , ss_check_sum );
        }
        std::size_t num_crawler;
        ss >> num_crawler;
        m_crawler_info . clear ();
        for ( std::size_t k(0)
            ; k < num_crawler
            ; ++k
            )
        {
            m_crawler_info . push_back ( CrawlerInfo () );
            m_crawler_info[k] . deserialize ( ss , ss_check_sum );
        }
    }

};

template < typename InfoType >
class SystemStatusMessage : public Serialize
{
    InfoType m_info;
public:

    SystemStatusMessage () : Serialize ()
    {

    }

    InfoType get_data ()
    {
        return m_info;
    }

    void set_data ( InfoType p_info )
    {
        m_info = p_info;
    }

    std::string serialize( std::size_t offset , std::size_t num )
    {
        std::stringstream ss;
        ss << get_type () << " ";
        std::vector < float > vec = m_info . serialize (  );
        for ( std::size_t k(0)
            ; k < vec . size ()
            ; ++k
            )
        {
            ss << vec[k] << " ";
        }
        std::size_t check_sum = 0;
        for ( std::size_t k(0)
            ; k < ss.str().length()
            ; ++k
            )
        {
            check_sum += ( (int)ss.str()[k] + 1 ) ;
        }
        ss << check_sum << " ";
        return ss.str();
    }

    std::size_t deserialize( std::string str )
    {
        std::stringstream ss;
        std::stringstream ss_check_sum;
        ss << str;
        std::size_t type;
        ss >> type;
        ss_check_sum << type << " ";
        if ( type != get_type () )
        {
            return 1;
        }
        m_info . deserialize ( ss , ss_check_sum );
        std::size_t check_sum = 0;
        for ( std::size_t k(0)
            ; k < ss_check_sum.str().length()
            ; ++k
            )
        {
            check_sum += ( (int)ss_check_sum.str()[k] + 1 ) ;
        }
        std::size_t check_sum_2;
        ss >> check_sum_2;
        if ( check_sum != check_sum_2 )
        {
            std::cout << "check sums do not match!" << std::endl;
        }
        std::string residual;
        ss >> residual;
        if ( !residual.empty() )
        {
            std::cout << "Data still left in serialized input, an error has occurred" << std::endl;
            std::cout << " residual data \'" << residual << "\'" << std::endl;
            return 1;
        }
        return 0;
    }

    std::size_t get_type ()
    {
        return STORE_TYPE;
    }

};

#endif

