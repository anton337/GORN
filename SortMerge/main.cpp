#include <iostream>
#include <vector>
#include <stdlib.h>
#include <algorithm>

void generate_sorted_data ( std::vector < long > & data , int size )
{
    data.resize(size);
    for ( int k(0)
        ; k < data.size()
        ; ++k
        )
    {
        data[k] = (long)rand()+2147483647*(long)rand();
    }
    std::sort ( data.begin () , data.end () );
}

std::vector < long > operator + ( std::vector < long > const & A
                                , std::vector < long > const & B
                                )
{
    std::vector < long > C ( A . size () + B . size () );
    std::vector < long > :: const_iterator it_A = A . begin ();
    std::vector < long > :: const_iterator it_B = B . begin ();
    std::vector < long > ::       iterator it_C = C . begin ();
    while ( it_C != C . end () )
    {
        if ( it_A != A . end () && it_B != B . end () )
        {
            if ( *it_A > *it_B )
            {
                *it_C = *it_B;
                ++it_B;
            }
            else
            {
                *it_C = *it_A;
                ++it_A;
            }
        }
        else if ( it_A == A . end () )
        {
            *it_C = *it_B;
            ++it_B;
        }
        else
        {
            *it_C = *it_A;
            ++it_A;
        }
        ++it_C;
    }
    return C;
}

int main()
{
    srand(time(NULL));
    std::vector < long > array1;
    std::vector < long > array2;
    generate_sorted_data ( array1 , 1000 );
    generate_sorted_data ( array2 , 1000 );
    std::vector < long > output;
    output = array1 + array2;
    for ( int k(0)
        ; k < output.size()
        ; ++k
        )
    {
        std::cout << output[k] << std::endl;
    }
    return 0;
}
