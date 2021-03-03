#include <map>
#include <string>
#include <queue>
#include "huffnode.h"
#ifndef HUFFMAN_H
#define HUFFMAN_H

//Buils a frequency table from the input stream where the key is the character and the value is the frequency of the character in the stream
map<char, int> build_freqtable(istream& input);

/*
*  Builds a huffman tree based on a frequency table, returns root node to huffman tree
*/
HuffNode* build_huffmantree(const map<int, int> &freqTable);

/*
*  Recursive function that constructs the huffman tree
*/
HuffNode* huffmantree_builder(priority_queue<HuffNode> &nodeQueue);

/*
*Builds an encodingMap containing characters as the key and their codeword as value
*/
map<char, string> build_codewordmap(HuffNode* root);

/*
* Recursive function for building a codeword map
*/
void codewordmap_helper(HuffNode* tree, map<char,string>& encodingMap, string code="");

bool write_header(const vector<pair<char, string>> &new_codewords, ofstream & output);
bool write_codewords(const map<char, string> &codewords, ifstream &input, ofstream &output);
vector<pair<char, string>> encode_with_codeword_length(map<char,string>& codewords);
vector<string> codewords_from_codeword_length(vector<int> lengths);


//Encodes the given file with huffman
string huffman_encode(string inputFileName);
void huffman_decode(string filePath, string fileExtension);

#endif