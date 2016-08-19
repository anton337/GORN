#include "unit_test.h"

#include <iostream>

#include "data/queue_file.h"
#include <boost/thread.hpp>

bool test_file_queue ()
{
    QueueFile queue_file ( "unit_tests/queue_data/" );
    std::vector < QueueEntry > vec;
    {
        vec . push_back ( QueueEntry ( 1 , "hello"  ) );
        vec . push_back ( QueueEntry ( 2 , "how"    ) );
        vec . push_back ( QueueEntry ( 3 , "are"    ) );
        vec . push_back ( QueueEntry ( 4 , "you"    ) );
        vec . push_back ( QueueEntry ( 5 , "?"      ) );
    }
    queue_file . Push ( "file1" , vec );
    std::vector < QueueEntry > out_vec;
    queue_file . Pop ( out_vec );
    for ( std::size_t k(0)
        ; k < out_vec . size ()
        ; ++k
        )
    {
        std::cout << out_vec[k].key << " " << out_vec[k].value << std::endl;
    }
    return true;
}

void pop ( std::vector < QueueEntry > * vec
         , QueueFile                    queue_file
         )
{
    queue_file . Pop ( *vec );
}

void push ( std::vector < QueueEntry > vec
          , std::string                file_name
          , QueueFile                  queue_file
          )
{
    queue_file . Push ( file_name , vec );
}

bool test_file_queue_parallel_test ()
{
    std::size_t n_threads = 20;
    QueueFile queue_file ( "unit_tests/queue_data/" );
    std::vector < boost::thread * > threads;
    std::vector < QueueEntry > vec;
    std::size_t check_sum_1 = 0;
    for ( std::size_t k(0)
        ; k < 10000
        ; ++k
        )
    {
        std::stringstream ss;
        std::size_t num = rand();
        ss << num;
        vec . push_back ( QueueEntry ( k , ss.str() ) );
        check_sum_1 += num;
    }
    std::cout << "push" << std::endl;
    for ( std::size_t file(0)
        ; file < n_threads
        ; ++file
        )
    {
        std::stringstream ss;
        ss << "file-" << file << ".que";
        threads . push_back ( new boost::thread ( push 
                                                , vec 
                                                , ss.str() 
                                                , queue_file 
                                                ) 
                            );
    }
    std::vector < QueueEntry > out_vec;
    std::cout << "pop" << std::endl;
    for ( std::size_t file(0)
        ; file < n_threads
        ; ++file
        )
    {
        threads . push_back ( new boost::thread ( pop 
                                                , &out_vec 
                                                , queue_file 
                                                ) 
                            );
    }
    for ( std::size_t k(0)
        ; k < threads . size ()
        ; ++k
        )
    {
        threads[k] -> join ();
    }
    std::cout << "done" << std::endl;
    std::size_t check_sum_2 = 0;
    for ( std::size_t k(0)
        ; k < out_vec . size ()
        ; ++k
        )
    {
        std::size_t num = atoi(out_vec[k].value.c_str());
        check_sum_2 += num;
    }
    check_sum_1 *= n_threads; // since we duplicated the data n_threads times
    std::cout << "check sum : " << check_sum_1 << " " << check_sum_2 << std::endl;
    return check_sum_1 == check_sum_2;
}

int main()
{
    srand ( time ( NULL ) );
    std::cout << "Welcome to File Queue Unit Test!" << std::endl;
    UnitTest("Test file queue",test_file_queue);
    UnitTest("Test file queue parallel test",test_file_queue_parallel_test);
    return 0;
}

