#include "unit_test.h"
#include "asio/client.h"
#include "data/data.h"

#include <stdlib.h>

bool test_sending_request_to_server()
{
    boost::asio::io_service svc;
    int port = 80;
    std::string host = "akodochygov.net84.net";
    std::string dir  = "/mathstuff.html";
    Client client(svc, host, std::to_string(port));
    std::string request ( "GET "+dir+" HTTP/1.1\r\nHost: "+host+"\r\nConnection: close\r\n\r\n" );
    std::cout << request << std::endl;
    client . send_complete ( request );
    return true;
}

int main()
{
    srand(time(NULL));
    std::cout << "Running Unit Tests ..." << std::endl;
    UnitTest("Test sending request to server",test_sending_request_to_server);
    return 0;
}

