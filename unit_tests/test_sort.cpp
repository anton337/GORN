#include "unit_test.h"
#include "data/data.h"

#include <stdlib.h>

bool test_one_file()
{
    std::vector < std::string > input;
    read_file  ( "unit_tests/test_data/data1.in"     , input );
    sort_data  ( input );
    write_file ( "unit_tests/test_output/simple.out" , input );
    return true;
}

bool test_directory()
{
    std::vector < std::string > files;
    std::string input_directory = "unit_tests/test_data/";
    std::string output_file = "unit_tests/test_output/comprehensive.out";
    get_files ( input_directory 
              , files
              );
    for ( std::size_t k(0)
        ; k < files.size()
        ; ++k
        )
    {
        std::cout << files[k] << std::endl;
    }
    sort_directory ( input_directory
                   , output_file
                   );
    return true;
}

bool test_scalable_directory()
{
    std::cout << "begin : test scalable directory" << std::endl;
    std::vector < std::string > files;
    std::string input_directory = "unit_tests/test_data/";
    std::string initial_file = "unit_tests/test_output/comprehensive.out";
    std::string comprehensive_file = "unit_tests/test_output/comprehensive.tmp";
    sort_files ( input_directory
               , initial_file
               , comprehensive_file
               );
    std::cout << "done : test scalable directory" << std::endl;
    return true;
}



bool test_create_file()
{
    std::string file_name = "unit_tests/test_output/create_test";
    std::vector < std::string > data;
    data . push_back ( "hello" );
    data . push_back ( "there" );
    write_file ( file_name , data );
    return true;
}

bool test_remove_file()
{
    std::string file_name = "unit_tests/test_output/remove_test";
    std::vector < std::string > data;
    data . push_back ( "hello" );
    data . push_back ( "there" );
    write_file ( file_name , data );
    remove_file ( file_name );
    return true;
}

int main()
{
    srand(time(NULL));
    std::cout << "Running Unit Tests ..." << std::endl;
    UnitTest("Test sort one file",test_one_file);
    UnitTest("Test sort directory",test_directory);
    UnitTest("Test sort scalable directory",test_scalable_directory);
    UnitTest("Test create file",test_create_file);
    UnitTest("Test remove file",test_remove_file);
    return 0;
}

