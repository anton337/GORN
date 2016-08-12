#ifndef SORTING_H
#define SORTING_H

#include <vector>

template < typename T >
std::vector < T > operator + ( std::vector < T > const & A
                             , std::vector < T > const & B
                             )
{
    std::vector < T > C ( A . size () + B . size () );
    typename std::vector < T > :: const_iterator it_A = A . begin ();
    typename std::vector < T > :: const_iterator it_B = B . begin ();
    typename std::vector < T > ::       iterator it_C = C . begin ();
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

#endif

