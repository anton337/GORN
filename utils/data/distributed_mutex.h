#ifndef DISTRIBUTED_MUTEX_H
#define DISTRIBUTED_MUTEX_H

class DistributedMutex
{
private:
    std::string m_unlock_name;
    std::string   m_lock_name;
public:
    DistributedMutex ( std::string p_unlock_name
                     , std::string   p_lock_name
                     )
    : m_unlock_name  ( p_unlock_name )
    ,   m_lock_name  (   p_lock_name )
    {

    }
private:
    int lock ()
    {
        if ( boost::filesystem::exists ( m_lock_name ) )
        {
            // directory locked, move along
            return 1;
        }
        else
        {
            // lock directory
            // boost::filesystem::rename(src,target)
            // should be atomic
            // so this should be equivalent to locking a mutex
            try
            {
                boost::filesystem::rename ( m_unlock_name , m_lock_name );
            }
            catch ( ... )
            {
                return 1;
            }
            return 0;
        }
    }

    int unlock ()
    {
        if ( boost::filesystem::exists ( m_unlock_name ) )
        {
            // can't unlock a mutex that is not locked, 
            // this should be an error,
            // it seems the developer messed up
            std::cout << "can't unlock a mutex that is not locked" << std::endl;
            return 1;
        }
        else
        {
            // unlock directory
            // boost::filesystem::rename(src,target)
            // should be atomic
            // so this should be equivalent to unlocking a mutex
            try
            {
                boost::filesystem::rename ( m_lock_name , m_unlock_name );
            }
            catch ( ... )
            {
                return 1;
            }
            return 0;
        }
    }
public:

    int Lock()
    {
        return lock();
    }

    void Unlock()
    {
        while ( unlock () == 1 ) { usleep(500000); }
    }
};

#endif

