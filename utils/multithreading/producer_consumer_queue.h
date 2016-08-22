#ifndef PRODUCER_CONSUMER_QUEUE_H
#define PRODUCER_CONSUMER_QUEUE_H

#include <deque>
#include "semaphore.h"

template < typename T >
class ProducerConsumerQueue
{

    public:

    std::size_t size ()
    {
        boost::unique_lock<boost::mutex> lock(*_mutex);
        return data . size ();
    }

    void put ( T * item )
    {
        emptyCount -> wait ();
        {
            boost::unique_lock<boost::mutex> lock(*_mutex);
            data . push_back ( item );
        }
        fillCount -> signal ();
    }

    T * get ()
    {
        T * item = NULL;
        fillCount -> wait ();
        {
            boost::unique_lock<boost::mutex> lock(*_mutex);
            if ( !data . empty () )
            {
                item = data . front ();
                data . pop_front ();
            }
        }
        emptyCount -> signal ();
        if ( item == NULL )
        {
            std::cout << "item is NULL" << std::endl;
        }
        return item;
    }

    ProducerConsumerQueue ( std::size_t BUFFER_SIZE )
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

