#ifndef FIND_REPLY_MESSAGE_SERIALIZE_H
#define FIND_REPLY_MESSAGE_SERIALIZE_H

#include "serialize.h"
#include "info.h"

class FindReplyMessage : public Serialize
{
    std::vector < std::string > m_exists;
    std::vector < connection_info > m_connections;

public:

    FindReplyMessage () : Serialize ()
    {

    }

    std::vector < std::string > get_data ()
    {
        return m_exists;
    }

    void push_host ( connection_info info )
    {
        m_connections . push_back ( info );
    }

    void pop_host ( connection_info info )
    {
        m_connections . pop_back ();
    }

    void set_data ( std::vector < std::string > const & p_data )
    {
        m_exists . clear ();
        for ( std::size_t k(0)
            ; k < p_data . size ()
            ; ++k
            )
        {
            m_exists . push_back ( p_data [k] );
        }
    }

    std::string serialize( std::size_t offset , std::size_t num )
    {
        std::stringstream ss;
        ss << get_type () << " ";
        ss << m_connections . size () << " ";
        for ( std::size_t k ( 0 )
            ; k < m_connections . size ()
            ; ++k
            )
        {
            ss << m_connections[k].host_name << " " << m_connections[k].port_no << " ";
        }
        std::size_t size ( std::min ( m_exists . size () , offset + num ) );
        ss << size << " ";
        for ( std::size_t k ( offset )
            ; k < size
            ; ++k
            )
        {
            ss << m_exists [ k ] << " ";
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
        m_connections . clear ();
        std::size_t num_connections;
        ss >> num_connections;
        ss_check_sum << num_connections << " ";
        for ( std::size_t k ( 0 )
            ; k < num_connections
            ; ++k
            )
        {
            std::string connection_host;
            ss >> connection_host;
            ss_check_sum << connection_host << " ";
            std::size_t connection_port;
            ss >> connection_port;
            ss_check_sum << connection_port << " ";
            m_connections . push_back ( connection_info ( connection_host
                                                        , connection_port
                                                        )
                                      );
        }
        std::size_t num;
        ss >> num;
        ss_check_sum << num << " ";
        m_exists . clear ();
        for ( std::size_t k(0)
            ; k < num
            ; ++k
            )
        {
            std::string value;
            ss >> value;
            ss_check_sum << value << " ";
            m_exists . push_back ( value );
        }
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
        return FIND_REPLY_TYPE;
    }

};

#endif

