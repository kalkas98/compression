#include <vector>
#include <map>


#ifndef STATE_H
#define STATE_H

/*
 * Struct for representing states and their transition probabilities to other states.
 * Takes a template parameter T that represents the symbol representation of a state
 * T must have and implementation of the < operator
 */ 
template<typename T>
struct State
{
    //Representation of the state eg. a char, a pair of chars or something else
    T state; 


    /* 
     * map that contains probabilities for state transitions from the current state 
     * The key represents another state
     * The value is the probability for the state transition
     */
    std::map<T, double> probabilities;

    /*
     *  Takes a vector<T> containing symbols that represent the possible states this state can transition to
     *  and initilizes all of the transition probabilities to 0
     */
    void initProbabilities(const std::vector<T> &symbols)
    {
        for (T const &symbol : symbols)
        {
            probabilities[symbol] = 0;
        }
    }

    // Returns a vector containing only the state transition probabilities 
    // The vector is sorted in the same way as the keys of the probabilities map
    std::vector<double> getProbablities() const
    {
        std::vector<double> probs(probabilities.size());
        int i = 0;
        for (const auto prob : probabilities)
        {
            probs[i] = prob.second;
            i++;
        }
        return probs;
    }

    bool operator <(const State & other) const;
};


#endif