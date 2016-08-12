#include <iostream>
#include <cstdlib>

int main(int argc,char * argv[])
{
    std::cerr << "Welcome to DataGenerator!" << std::endl;
    std::size_t num_per_iteration ( -1 );
    std::size_t num_iterations ( -1 );
    if ( argc != 3 )
    {
        std::cout << "try ./DataGenerator <num_per_iteration> <num_iterations>" << std::endl;
        return 1;
    }
    num_per_iteration = atoi(argv[1]);
    num_iterations = atoi(argv[2]);
    srand(time(0));
    for ( std::size_t iter(0)
        ; iter < num_iterations
        ; ++iter
        )
    {
        for ( std::size_t num(0)
            ; num < num_per_iteration
            ; ++num
            )
        {
            // std::cout << num << " ";
            std::cout << rand() << " ";
        }
        std::cout << std::endl;
    }
    return 0;
}

