#ifndef PRODUCER_CONSUMER_QUEUE_H
#define PRODUCER_CONSUMER_QUEUE_H

#include "semaphore.h"

template < typename T >
class ProducerConsumerQueue
{

    public:

    void put ( T * item )
    {
        std::cout << "<<" << std::endl;
        emptyCount -> wait ();
        {
            boost::unique_lock<boost::mutex> lock(*_mutex);
            data . push_back ( item );
            std::cout << "elem=" << item->message << "   size=" << data . size () << std::endl;
        }
        fillCount -> signal ();
    }

    T * get ()
    {
        std::cout << ">>" << std::endl;
        T * item = NULL;
        fillCount -> wait ();
        {
            boost::unique_lock<boost::mutex> lock(*_mutex);
            std::cout << ">>size:" << data . size () << std::endl;
            if ( !data . empty () )
            {
                item = data . front ();
                if ( item )
                {
                    std::cout << ">>message:" << item -> message << std::endl;
                }
                else
                {
                    std::cout << ">>NULL" << std::endl;
                }
                data . pop_front ();
            }
        }
        emptyCount -> signal ();
        if ( item == NULL )
        {
            std::cout << "item is NULL" << std::endl;
        }
        std::cout << ">>Done!" << std::endl;
        return item;
    }

    ProducerConsumerQueue ( int BUFFER_SIZE )
    {
        emptyCount = new semaphore(BUFFER_SIZE);
        fillCount = new semaphore(0);
        _mutex = new boost::mutex();
    }

    private:

        semaphore * emptyCount;
        semaphore * fillCount;
        boost::mutex * _mutex;

        std::deque < T * > data;

};

#endif

