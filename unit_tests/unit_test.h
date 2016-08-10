#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#include <iostream>

class UnitTest
{
public:
    UnitTest( std::string test_name, bool (*functor)() )
    {
        ( (*functor)() ) ? std::cout << test_name << " : PASSED!" << std::endl 
                         : std::cout << test_name << " : Failed..." << std::endl
                         ;
    }
};

#endif

