#include <iostream>
#include <cstdlib>

int main(int argc,char * argv[])
{
    std::cout << "Welcome to ClientInterface!" << std::endl;
    std::string config_file;
    if ( argc != 2 )
    {
        std::cout << "try ./ClientInterface <config_file>" << std::endl;
        return 1;
    }
    config_file = std::string(argv[1]);
    std::string line;
    while(1)
    {
        std::cout << ">>> ";
        getline(std::cin,line);
        std::cout << line << std::endl;
    }
    return 0;
}

