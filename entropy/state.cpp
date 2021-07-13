#include "state.h"

template<typename T>
bool State<T>::operator<(const State &other) const
{
    return state < other.state;
}

//Compare pairs of chars lexicographically
template<>
bool State<std::pair<char,char>>::operator<(const State &other) const
{
    std::string s0 = std::string()+state.first + state.second;
    std::string s1 = std::string()+other.state.first + other.state.second;
    return s0 < s1;
}