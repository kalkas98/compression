#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "state.h"


#ifndef ENTROPY_H
#define ENTROPY_H
std::vector<double> entropy(std::ifstream & file, int order);
std::vector<State<char>> get_states(const std::map<std::pair<char,char>, int> &conditional_occurances, const std::map<char, int> &followed_occurances);
void get_probabilities();
#endif