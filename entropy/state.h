#include <vector>
#include <map>

using namespace std;

#ifndef STATE_H
#define STATE_H

/*
 * Struct for representing states and their transition probabilities to other states.
 * Takes a template parameter T that represents the symbol representation of a state
 */ 
template<typename T>
struct State
{
    T state;
    map<T, double> probabilities;//Probabilities for state transitions

    void initProbabilities(vector<T> symbols)
    {
        for (T symbol : symbols)
        {
            probabilities[symbol] = 0;
        }
    }
    // Returns a vector on probabilities where vector[i] contains the probability for
    // symbol i in this state, the vector is sorted lexicographically
    vector<double> getProbablities() const
    {
        vector<double> probs;
        for (auto prob : probabilities)
        {
           probs.push_back(prob.second);
        }
        return probs;
    }

    bool operator <(const State & other) const;
};


#endif