#include "unit_test.h"

#include <iostream>

bool test_transfer_urls ()
{

    return true;
}

int main()
{
    std::cout << "Welcome to URL Transfer Unit Test!" << std::endl;
    UnitTest("Test URL Transfer",test_transfer_urls);
    return 0;
}

