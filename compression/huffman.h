#include <map>
#include <string>
#include <queue>
#include "huffnode.h"
#ifndef HUFFMAN_H
#define HUFFMAN_H

//Buils a frequency table from the input stream where the key is the character and the value is the frequency of the character in the stream
map<char, int> buildFreqTable(istream& input);

/*
*  Builds a huffman tree based on a frequency table, returns root node to huffman tree
*/
HuffNode* buildHuffmantree(const map<int, int> &freqTable);

/*
*  Recursive function that constructs the huffman tree
*/
HuffNode* huffmanTreeBuilder(priority_queue<HuffNode> &nodeQueue);

/*
*Builds an encodingMap containing characters as the key and their codeword as value
*/
map<char, string> buildCodewordMap(HuffNode* root);

/*
* Recursive function for building a codeword map
*/
void codewordMapHelper(HuffNode* tree, map<char,string>& encodingMap, string code="");
bool writeHeader(const vector<pair<char, string>> &new_codewords, ofstream & output);
bool write_codewords(const map<char, string> &codewords, ifstream &input, ofstream &output);

//Encodes the given file with huffman
string encode(string inputFileName);
void decode(string filePath, string fileExtension);

#endif