#include "unit_test.h"
#include "data/data.h"
#include "hash/hash.h"
#include "serializers/system_status_serialize.h"
#include <sstream>

bool test_system_status()
{
    InfoPackage info;

    info . m_system_info . p_cpu_percent . push_back ( 20 );
    info . m_system_info . p_cpu_percent . push_back ( 30 );
    info . m_system_info . p_cpu_percent . push_back ( 40 );
    info . m_system_info . p_cpu_percent . push_back ( 50 );
    info . m_system_info . p_memory_percent = 53;

    HashTableInfo hash_table_info;

    hash_table_info . queue_size = 2300000;
    hash_table_info . sorting_queue_size . push_back ( 12345 );
    hash_table_info . sorting_queue_size . push_back ( 123456 );
    hash_table_info . sorting_queue_size . push_back ( 1234567 );
    hash_table_info . sorting_queue_size . push_back ( 12345678 );
    hash_table_info . find_sorting_queue_size . push_back ( 12345 );
    hash_table_info . find_sorting_queue_size . push_back ( 123456 );
    hash_table_info . find_sorting_queue_size . push_back ( 1234567 );
    hash_table_info . find_sorting_queue_size . push_back ( 12345678 );
    hash_table_info . output_queue_size . push_back ( 12345 );
    hash_table_info . output_queue_size . push_back ( 123456 );
    hash_table_info . output_queue_size . push_back ( 1234567 );
    hash_table_info . output_queue_size . push_back ( 12345678 );
    hash_table_info . find_output_queue_size . push_back ( 12345 );
    hash_table_info . find_output_queue_size . push_back ( 123456 );
    hash_table_info . find_output_queue_size . push_back ( 1234567 );
    hash_table_info . find_output_queue_size . push_back ( 12345678 );

    info . m_hash_table_info . push_back ( hash_table_info );
    info . m_hash_table_info . push_back ( hash_table_info );

    CrawlerInfo crawler_info;

    crawler_info . Q_size = 3456;
    crawler_info . Z_size = 34567;
    crawler_info . map_queue_size = 345678;
    crawler_info . M_size = 3456789;

    info . m_crawler_info . push_back ( crawler_info );
    info . m_crawler_info . push_back ( crawler_info );

    SystemStatusMessage < InfoPackage > info_message;

    info_message . set_data ( info );

    std::string message ( info_message . serialize ( 0 , 0 ) );
    std::cout << message << std::endl;

    return info_message . deserialize ( message ) == 0 ;
}

int main()
{
    std::cout << "Running Unit Tests ..." << std::endl;
    UnitTest("System Status Unit test",test_system_status);
    return 0;
}

