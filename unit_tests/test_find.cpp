#include "unit_test.h"
#include "data/data.h"
#include <sstream>

bool test_find()
{
    std::vector < std::string > vec;
    for ( std::size_t k(0)
        ; k < 10
        ; ++k
        )
    {
        std::stringstream ss;
        ss << k;
        vec . push_back ( ss . str () );
    }
    std::sort ( vec.begin() , vec.end() );
    search_new ( "unit_tests/test_find/map_data"
               , vec
               , "unit_tests/test_find/search_results"
               );
    return true;
}

int main()
{
    std::cout << "Running Unit Tests ..." << std::endl;
    UnitTest("Test find unit test",test_find);
    return 0;
}

