#include "entropy.h"
#include "state.h"
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <cmath>

//Log with base 2
double log2(double n)
{
    return log(n) / log(2);
}

/*
 * Returns a vector containing vectors of State objects. In returned_vector[i] the state objects for a markov source
 * of order i are contained. Each state object in the returned vector contains its state transition probabilities.
 */
std::vector<std::vector<State<std::string>>> get_states(std::vector<std::map<std::string, int>> &occurance_map, int order)
{
    //Map where the key is the symbol(s) representing the state and the value is a State object containing the transition probabilities
    std::vector<std::map<std::string, State<std::string>>> states(occurance_map.size()); 

    //Assemble all symbols and use them to init the probabilities for the states
    std::vector<std::vector<std::string>> state_symbols(occurance_map.size());
    for (int i = 0; i < occurance_map.size(); i++)
    {
        for (const auto & pair : occurance_map[i])
        {
           state_symbols[i].push_back(pair.first);
        }
    }

    for (int i = 0; i < state_symbols.size(); i++)
    {
        for (const auto &symbol : state_symbols[i])
        {
            states[i][symbol].state = symbol;
        }
    }

    typedef std::pair<std::string,std::string> string_pair;
    std::vector<std::map<string_pair,int>> transition_vectors(order);//Vector containing maps specifying transition occurances between pairs of states

    for (int i = 1; i < occurance_map.size(); i++)
    {
        for (auto state_occurance : occurance_map[i])
        {
            std::string state_string = state_occurance.first;
            //The transition is from the state in the second position in the pair to the first state in the pair
            string_pair state_transition{state_string.substr(0,state_string.size()-1), state_string.substr(1,state_string.size()-1)};
            transition_vectors[i-1][state_transition] = state_occurance.second;
        }
    }

    std::vector<std::vector<State<std::string>>> returned_states(states.size()); 
    for (int i = 0; i < transition_vectors.size(); i++)
    {
        for (auto &transition : transition_vectors[i])
        {
            string_pair t_pair = transition.first;
            int transition_occurances = transition.second;
            double probability = (double)transition_occurances/occurance_map[i].at(t_pair.second);
            if(states[i][t_pair.second].probabilities.find(t_pair.first) != states[i][t_pair.second].probabilities.end())
                states[i][t_pair.second].probabilities[t_pair.first] = 0;
            states[i][t_pair.second].probabilities[t_pair.first] = probability; 
        }
    }

    for (int i = 0; i < states.size(); i++)
    {
        for (const auto &map_pair : states[i])
        {
            returned_states[i].push_back(map_pair.second);
        }
    }
    return returned_states;
}

/*
 * Takes a input stream to a file and returns a vector containing
 * the entropy rates up to the given order 
 */ 
std::vector<double> entropy(std::ifstream &input, int order)
{

    std::vector<std::map<std::string, int>> occurance_map(order+1);
    std::vector<std::string> readers(order+1);
    for (int i = 0; i < order; i++)
    {
        readers[i] = std::string(i+1, 'x');
    }

    int n{0};
    
    char new_char;
    while(input.get(new_char))
    {
        n++;
        for (int i = 1; i < readers.size(); i++)
        {
            //Move the the content of the string one step to the right
            readers[i] = new_char + readers[i].substr(0,i);

        }
        readers[0][0] = new_char;
        for (int i = 0; i < occurance_map.size(); i++)
        {
            if(n > i)
                occurance_map[i][readers[i]]++; 
        }
        
    }

    std::vector<std::vector<double>> stationary_dist(order+1);

    int order_index = 0;
    for (const auto &occurances : occurance_map)
    {
        for (const auto &occ : occurances)
        {
            stationary_dist[order_index].push_back((double) occ.second/(n-order_index));
        }
        
        order_index++;
    }
    

    std::vector<std::vector<State<std::string>>> states = get_states(occurance_map, order); //Get state probabilities

    //Assemble a map containing the state transitions when a state consists of two characters
    std::vector<double> entropy_rates(order, 0);

    //Use probabilities to get single symbol entropy
    for (int i = 0; i < stationary_dist[0].size(); i++)
    {
        entropy_rates[0] -= stationary_dist[0][i]*log2(stationary_dist[0][i]);
    }

    for (int i = 0; i < order; i++)
    {
        for (int j = 0; j < states[i].size(); j++)
        {
            for (const auto &prob_pair : states[i][j].probabilities)
            {
                double prob = prob_pair.second;
                if(prob <= 0)
                    continue;
                double calc = stationary_dist[i][j]*(-prob * log2(prob));
                entropy_rates[i+1] += calc;
            }
        }
    }
    
    //Calculate entropy rate for order 1 and 2 using chain rule
    for (int i = 0; i < entropy_rates.size(); i++)
    {
        std::cout << "Entropy rate order " << i << " " << entropy_rates[i] << std::endl;
    }
    
    return entropy_rates;
}