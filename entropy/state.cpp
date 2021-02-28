#include "state.h"

template<typename T>
bool State<T>::operator<(const State &other) const
{
    return state < other.state;
}

template<>
bool State<pair<char,char>>::operator<(const State &other) const
{
    string s0 = string()+state.first + state.second;
    string s1 = string()+other.state.first + other.state.second;
    return s0 < s1;
}