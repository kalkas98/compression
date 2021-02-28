#include "entropy.h"
#include "state.h"
#include "Eigen/Dense"
#include "Eigen/SparseLU"
#include <vector>
#include <map>
#include <fstream>
#include <iostream>

using namespace std;
// I use the Eigen libary for solving matrix equations
// See: https://eigen.tuxfamily.org/index.php?title=Main_Page
using namespace Eigen;

/*
 * Takes one template parameter:
 * T - State type
 * 
 * Two function paramenters:
 * 
 * transition_occurances - Map that contains a transition as the key and the value as the number of occurances of that transition
 * state_occurances - Map that contains a state as the key and the total number of times a transition starts from the state as the value
 * 
 * Returns a vector of state objects, where each State object contains the probabilities for transitions to other order_1_states
 * 
 */
template<typename T>
vector<State<T>> getStates(const map<pair<T,T>, int> &transition_occurances, const map<T, int> &state_occurances)
{
    map<T, State<T>> order_1_states;

    vector<T> state_v;//Assemble all symbols and use them to init the probabilities for the order_1_states
    for (auto state : state_occurances)
    {
        state_v.push_back(state.first);
    }
    for (auto state: state_v)
    {
        order_1_states[state].init_probabilities(state_v);
        order_1_states[state].state = state;
    }
    for (auto occurance : transition_occurances)
    {
        double probability = (double)occurance.second/state_occurances.at(occurance.first.second);
        order_1_states[occurance.first.second].probabilities[occurance.first.first] = probability; 
        order_1_states[occurance.first.second].state = occurance.first.second;
    }
    vector<State<T>> stateVector;
    for (auto state : order_1_states)
    {
        stateVector.push_back(state.second);
    }   
    return stateVector;
}

/*
 * Takes a template parameter T that represents the type of a state
 * 
 * Also takes two function parameters:
 *  order_1_states - a vector containing the order_1_states and the transition probabilities
 *  sparse - an optional arguement. If true we use a sparse matrix solver instead of a dense solver.
 */ 
template<typename T>
vector<double> getStationaryDistribution(const vector<State<T>> &order_1_states, bool sparse)
{
    int N = order_1_states.size();
    //We want to solve the equation (A - I) * w = 0
    //Where A is our transition matrix, I is the identity matrix and w is the stationary distribution
    VectorXd sol;
    if(sparse)
    {
        //Assemble matrix equation and solve it
        SparseMatrix<double> A(N,N);
        SparseMatrix<double> identity(N,N);
        identity.setIdentity();

        for (int i{0};  i< N; i++)
        {
            vector<double> probs = order_1_states[i].getProbablities();
            for (int j{0};  j < N ; j++)
            {
                A.coeffRef(j,i) = probs[j];
            }
            
        }
        A -= identity;
        for (int i = 0; i < N; i++) //Replace last equation with equation sum(w_i) = 1
        {
            A.coeffRef(N-1, i) = 1;
        }
        VectorXd b(N);
        //For some reason elements are sometimes initialized to a value very close to zero instead of zero. So we set the vector to zero.
        b.setZero(); 
        b(N-1) = 1;
        A.makeCompressed();
        SparseLU<SparseMatrix<double>> solver;
        solver.analyzePattern(A);
        solver.factorize(A);
        sol = solver.solve(b);
    }
    else
    {
        MatrixXd A(N, N);
        for (int i{0};  i< N; i++)
        {
            vector<double> probs = order_1_states[i].getProbablities();
            for (int j{0};  j < N ; j++)
            {
                A(j,i) = probs[j];  //set(Row, column) to the probability
            }
            
        }
        A -= MatrixXd::Identity(N, N);
        for (int i = 0; i < N; i++) //Replace last equation with equation sum(w_i) = 1
        {
            A(N-1, i) = 1;
        }
        VectorXd b(N);
        //For some reason elements are sometimes initialized to a value very close to zero instead of zero. So we set the vector to zero.
        b.setZero(); 
        b(N-1) = 1;
        sol = A.colPivHouseholderQr().solve(b); //Solve for the stationary distrubution
    }

    //Convert sol to a regular stl vector
    vector<double> statioanry_dist_1;
    statioanry_dist_1.resize(sol.size());
    VectorXd::Map(&statioanry_dist_1[0], sol.size()) = sol;
    return statioanry_dist_1;
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
 *  Also takes a second optional parameter sparse. When sparse is true we will use a sparse matrix equation
 *  solver algorithm from the Eigen libary
 * 
 */ 
vector<double> entropy(ifstream &input, bool sparse)
{
    map<char, int> occurances; //Occurances of single symbols
    map<pair<char,char>, int> pair_occurances; //Occurances of symbol pairs (a, b)
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
    char last_char = chars.second;
    occurances[last_char]--; //Remove the occurance counted for the last char since no character follows it
    vector<State<char>> order_1_states = getStates<char>(pair_occurances, occurances); //Get state probabilities

    pair_occurances[trip.state]--; //Remove the last occurance counter for the last pair
    //Assemble a map containing the state transitions when a state consists of two characters
    typedef pair<pair<char,char>, pair<char,char>> state_2_pair;
    map<state_2_pair, int> state_2_transitions;
    for (auto transition : triple_occurances)
    {
        state_2_pair tmp = {{transition.first.new_char, transition.first.state.first}, transition.first.state};
        state_2_transitions[tmp] = transition.second;
    }
    vector<State<pair<char,char>>> order_2_states = getStates<pair<char,char>>(state_2_transitions, pair_occurances); //Get state probabilities

    vector<double> statioanry_dist_1 = getStationaryDistribution<char>(order_1_states, sparse);
    vector<double> stationary_dist_2 = getStationaryDistribution<pair<char,char>>(order_2_states, sparse);

    //***Use probabilities to get single symbol entropy
    double single_entropy{0};
    for (int i = 0; i < statioanry_dist_1.size(); i++)
    {
        double log_2 = log(statioanry_dist_1[i]) / log(2);
        single_entropy -= statioanry_dist_1[i]*log_2;
    }
    cout << "Single entropy: " << single_entropy << endl;

    //*** Calculate joint entropy from probability distrubution for pairs of symbols
    double joint_entropy{0};
    for (int i = 0; i < order_1_states.size(); i++)
    {
        for (auto prob : order_1_states[i].getProbablities())
        {
            double calc = statioanry_dist_1[i]*prob;
            if(calc <= 0)
                continue;
            double log_2 = log(calc)/log(2);
            joint_entropy += -calc*log_2;
        }
    }
    //Compute joint entropy with three variables with probabilities from triplets
    double triple_joint_entropy{0};
    for (int i = 0; i < order_2_states.size(); i++)
    {
        for (auto prob : order_2_states[i].getProbablities())
        {
            double calc = stationary_dist_2[i]*prob;
            if(calc <= 0)
                continue;
            double log_2 = log(calc)/log(2);
            triple_joint_entropy += -calc*log_2;
        }
    }

    cout << "Triple joint entropy: " << triple_joint_entropy << endl;
    cout << "Joint entropy: " << joint_entropy << endl;

    //*** Calculate entropy rate for order 1 and 2 using chain rule
    double entropy_rate = joint_entropy - single_entropy;
    double order_2_entropy_rate = triple_joint_entropy-single_entropy-entropy_rate;
    cout << "Entropy rate order 1: " << entropy_rate << endl;
    cout << "Entropy rate order 2 " << order_2_entropy_rate << endl;

    //Return entropy rates
    vector<double> rates;
    rates.push_back(single_entropy);
    rates.push_back(entropy_rate);
    rates.push_back(order_2_entropy_rate);
    
    return rates;
}