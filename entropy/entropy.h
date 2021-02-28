#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "state.h"


#ifndef ENTROPY_H
#define ENTROPY_H
vector<double> entropy(ifstream & file, bool sparse=false);
vector<State<char>> getStates(const map<pair<char,char>, int> &conditional_occurances, const map<char, int> &followed_occurances);
void get_probabilities();
vector<double> getStationaryDistribution(const vector<State<char>> &states, bool sparse=false);

#endif