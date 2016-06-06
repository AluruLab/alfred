#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <algorithm>
#include <functional>
#include <vector>
#include <sstream>
#include "defs.hpp"

// UTILITY FUNCTIONS -------------------------------------------------
// trim taken from stack overflow
// http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))).base(),
            s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}


static inline std::vector<std::string> &split(const std::string &s, char delim,
                                              std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

static inline bool ends_with(std::string const &value,
                             std::string const &ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

#ifdef WIN32
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '\\' || ch == '/';
    }
};
#else
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '/';
    }
};
#endif

static inline std::string basename( std::string const& pathname )
{
    return std::string(
        std::find_if( pathname.rbegin(), pathname.rend(),
                      MatchPathSeparator() ).base(),
        pathname.end() );
}

static inline std::string removeExtension( std::string const& filename )
{
    std::string::const_reverse_iterator
                        pivot
            = std::find( filename.rbegin(), filename.rend(), '.' );
    return pivot == filename.rend()
        ? filename
        : std::string( filename.begin(), pivot.base() - 1 );
}
static inline char bits_to_char(unsigned value){
    switch(value) {
    case 0:
        return 'A';
    case 1:
        return 'C';
    case 2:
        return 'G';
    case 3:
        return 'T';
    default:
        return 'N';
    }
}

static inline int char_to_bits(char c) {
    int cvalue = -1;
    switch (c) {
        case 'A':
        case 'a':
            cvalue = 0;
            break;
        case 'C':
        case 'c':
            cvalue = 1;
            break;
        case 'G':
        case 'g':
            cvalue = 2;
            break;
        case 'T':
        case 't':
            cvalue = 3;
            break;
    }
    return cvalue;
}

static inline bool is_valid_dna(char c){
    for(auto i = 0u; i < DNA_ALPHABET_SIZE; i++)
        if(c == DNA_ALPHABET[i])
            return true;
    return false;
}

static inline bool is_valid_prot(char c){
    for(auto i = 0u; i < PROT_ALPHABET_SIZE; i++)
        if(c == PROT_ALPHABET[i])
            return true;
    return false;
}

static inline bool is_valid_apha(char c){
    return is_valid_dna(c) || is_valid_prot(c);
}

static inline std::string str_reverse(const std::string& in){
    std::string rev_out = in;
    std::reverse(rev_out.begin(), rev_out.end());
    return rev_out;
}
#endif /* UTIL_H */
