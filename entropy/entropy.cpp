#include "entropy.h"
#include "state.h"
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <cmath>

using namespace std;

/*
 * Takes one template parameter:
 * T - State type
 * 
 * Two function paramenters:
 * 
 * transition_occurances - Map that contains a transition as the key and the value as the number of occurances of that transition
 * state_occurances - Map that contains a state as the key and the total number of times a transition starts from the state as the value
 * 
 * Returns a vector of state objects, where each State object contains the probabilities for transitions to other states
 * 
 */
template<typename T>
vector<State<T>> get_states(const map<pair<T,T>, int> &transition_occurances, const map<T, int> &state_occurances)
{
    //Map where the key is the symbol(s) representing the state and the value is a State object containing the transition probabilities
    map<T, State<T>> states; 

    vector<T> state_symbols;//Assemble all symbols and use them to init the probabilities for the states
    for (auto state : state_occurances)
    {
        state_symbols.push_back(state.first);
    }
    for (auto state: state_symbols)
    {
        states[state].initProbabilities(state_symbols);
        states[state].state = state;
    }
    for (auto occurance : transition_occurances)
    {
        double probability = (double)occurance.second/state_occurances.at(occurance.first.second);
        states[occurance.first.second].probabilities[occurance.first.first] = probability; 
        states[occurance.first.second].state = occurance.first.second;
    }
    vector<State<T>> stateVector;
    for (auto state : states)
    {
        stateVector.push_back(state.second);
    }   
    return stateVector;
}

//Struct that helps for reading three symbols at a time and recording the state transitions
struct triple
{
    pair<char,char> state;
    char new_char{};

    bool operator <(const triple &other) const
    {
        string s1 = string()+new_char+state.first+state.second;
        string s2 = string()+other.new_char+other.state.first+other.state.second;
        return s1 < s2; //Compare lexicograpically
    }

    bool operator ==(const triple &other) const
    {
        return new_char == other.new_char && state == other.state;
    }
};

/*
 * Takes a input stream to a file and returns a vector containing
 *  The signle symbol entropy
 *  The entropy rate when viewing the file as a markov source of order 1
 *  The entropy rate when viewing the file as a markov source of order 2
 * 
 */ 
vector<double> entropy(ifstream &input)
{
    map<char, int> occurances; //Occurances of single symbols
    map<pair<char,char>, int> pair_occurances; //Occurances of symbol pairs (a, b), ie (b|a)
    map<triple, int> triple_occurances; //Occurances of symbol triples (a,b,c), ie (c|ba)

    pair<char,char> chars; //Read pairs of chars
    triple trip; //Also read triples of chars
    int n{0};
    if(input.get(chars.second))
    {
        occurances[chars.second]++;
        n++;
        while(input.get(chars.first))
        {
            n++;
            pair_occurances[chars]++; //Count occurances for every symbol pair (a,b)
            occurances[chars.first]++; //Count total number of occurances of every symbol
            trip.new_char = chars.first;
            if(n > 2)
            {
                triple_occurances[trip]++;
            }
            trip.state = chars;
            chars.second = chars.first;
        }
    }
    //Calculate single symbol and pair probabilities
    char last_char = chars.second;
    vector<double> stationary_dist_1;
    vector<double> stationary_dist_2;
    for (auto occurance : occurances)
    {
        stationary_dist_1.push_back((double)occurance.second/n);
    }
    for (auto pair : pair_occurances)
    {
        stationary_dist_2.push_back((double)pair.second/(n-1));
    }
    
    //Calcualkte state transition probabilities
    occurances[last_char]--; //Remove the occurance counted for the last char since no character follows it
    vector<State<char>> states = get_states<char>(pair_occurances, occurances); //Get state probabilities

    pair_occurances[trip.state]--; //Remove the last occurance counter for the last pair
    //Assemble a map containing the state transitions when a state consists of two characters
    typedef pair<pair<char,char>, pair<char,char>> state_2_pair;
    map<state_2_pair, int> state_2_transitions;
    for (auto transition : triple_occurances)
    {
        state_2_pair tmp = {{transition.first.new_char, transition.first.state.first}, transition.first.state};
        state_2_transitions[tmp] = transition.second;
    }
    vector<State<pair<char,char>>> order_2_states = get_states<pair<char,char>>(state_2_transitions, pair_occurances); //Get state probabilities

    //Use probabilities to get single symbol entropy
    double single_entropy{0};
    for (int i = 0; i < stationary_dist_1.size(); i++)
    {
        double log_2 = log(stationary_dist_1[i]) / log(2);
        single_entropy -= stationary_dist_1[i]*log_2;
    }
    cout << "Single entropy: " << single_entropy << endl;

    double order_1_entropy_rate;
    for (int i = 0; i < states.size(); i++)
    {
        for (auto prob : states[i].getProbablities())
        {
            if(prob <= 0)
                continue;
            double calc = stationary_dist_1[i]*(-prob*(log(prob)/log(2)));
            order_1_entropy_rate += calc;
        }
    }

    double order_2_entropy_rate{0};
    for (int i = 0; i < order_2_states.size(); i++)
    {
        for (auto prob : order_2_states[i].getProbablities())
        {
            if(prob <= 0)
                continue;
            double calc = stationary_dist_2[i]*(-prob*(log(prob)/log(2)));
            order_2_entropy_rate += calc;
        }
    }

    //Calculate entropy rate for order 1 and 2 using chain rule
    cout << "Entropy rate order 1: " << order_1_entropy_rate << endl;
    cout << "Entropy rate order 2 " << order_2_entropy_rate << endl;


    //Return entropy rates
    vector<double> rates;
    rates.push_back(single_entropy);
    rates.push_back(order_1_entropy_rate);
    rates.push_back(order_2_entropy_rate);
    
    return rates;
}