#include "unit_test.h"
#include "data/data.h"
#include "hash/hash.h"
#include "serializers/system_status_serialize.h"
#include <sstream>

bool test_system_status()
{
    return true;
}

int main()
{
    std::cout << "Running Unit Tests ..." << std::endl;
    UnitTest("System Status Unit test",test_system_status);
    return 0;
}

