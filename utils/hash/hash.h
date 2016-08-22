#ifndef HASH_H
#define HASH_H

#include <map>

struct HashGenerator
{

    std::map < char , std::size_t > hash_map;

    HashGenerator ()
    {
        init();
    }

    void init()
    {
        // allowed characters : !*'();:@&=+$,/?#[]   %
        //                      A-Z
        //                      a-z
        //                      0-9
        //                      .~-
        std::size_t index = 1;
        hash_map['!'] = index++;
        hash_map['*'] = index++;
        hash_map['\''] = index++;
        hash_map['('] = index++;
        hash_map[')'] = index++;
        hash_map[';'] = index++;
        hash_map[':'] = index++;
        hash_map['@'] = index++;
        hash_map['&'] = index++;
        hash_map['='] = index++;
        hash_map['+'] = index++;
        hash_map['$'] = index++;
        hash_map[','] = index++;
        hash_map['/'] = index++;
        hash_map['?'] = index++;
        hash_map['#'] = index++;
        hash_map['['] = index++;
        hash_map[']'] = index++;
        hash_map['%'] = index++;
        for ( char c='A'
            ; c <= 'Z'
            ; ++c
            )
        {
            hash_map[c] = index++;
        }
        for ( char c='a'
            ; c <= 'z'
            ; ++c
            )
        {
            hash_map[c] = index++;
        }
        for ( char c='0'
            ; c <= '9'
            ; ++c
            )
        {
            hash_map[c] = index++;
        }
        hash_map['.'] = index++;
        hash_map['~'] = index++;
        hash_map['-'] = index++;
    }

    bool validate ( std::string const & str )
    {
        for ( std::size_t k(0)
            ; k < str . size ()
            ; ++k
            )
        {
            if ( hash_map . find ( str[k] ) == hash_map . end () )
            {
                return false;
            }
        }
        return true;
    }

    std::size_t generate_hash ( std::string const & str )
    {
        if ( ! validate ( str ) )
        {
            std::cout << "hash generator : " 
                      << " input string \"" 
                      << str 
                      <<  "\" contains illegal characters " 
                      << std::endl;
        }
        std::size_t ret = 0;
        for ( std::size_t k(0)
            ; k < str . size ()
            ; ++k
            )
        {
            ret *= 2;
            ret += hash_map[str[k]];
        }
        return ret;
    }

};

#endif

