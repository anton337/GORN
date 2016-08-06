#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

void read_file ( std::string file_name , bool quiet = true )
{
    std::string line;
    std::string token;
    std::ifstream myfile ( file_name.c_str() );
    if ( myfile . is_open () )
    {
        std::cout << "File " << file_name << " is open." << std::endl;
        std::size_t num_print = 0;
        while ( getline ( myfile , line ) )
        {
            std::stringstream ss;
            ss << line;
            while ( ss >> token )
            {
                if ( !quiet && num_print < 1000 )
                {
                    num_print++;
                    std::cout << token << ",";
                }
            }
        }
        if ( !quiet )
        {
            std::cout << std::endl;
        }
        myfile . close ();
    }
    else
    {
        std::cout << "Unable to open file : " << file_name << std::endl;
    }
}

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
        std::stringstream ss;
        ss << line;
        std::string command;
        ss >> command;
        if ( command == "load-quiet" )
        {
            std::string file_name;
            ss >> file_name;
            std::cout << "Loading " << file_name << " ... " << std::endl;
            read_file ( file_name );
            std::cout << "Done ... " << std::endl;
        }
        if ( command == "load" )
        {
            std::string file_name;
            ss >> file_name;
            std::cout << "Loading " << file_name << " ... " << std::endl;
            read_file ( file_name , false );
            std::cout << "Done ... " << std::endl;
        }
    }
    return 0;
}

