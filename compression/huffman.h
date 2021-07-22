#include <map>
#include <string>
#include <queue>
#include "huffnode.h"
#ifndef HUFFMAN_H
#define HUFFMAN_H

//Buils a frequency table from the input stream where the key is the character and the value is the frequency of the character in the stream
std::map<char, int> build_freqtable(std::istream& input);

/*
*  Builds a huffman tree based on a frequency table, returns root node to huffman tree
*/
HuffNode* build_huffmantree(const std::map<int, int> &freqTable);

/*
*  Recursive function that constructs the huffman tree
*/
HuffNode* huffmantree_builder(std::priority_queue<HuffNode> &nodeQueue);

/*
*Builds an encodingMap containing characters as the key and their codeword as value
*/
std::map<char, std::string> build_codewordmap(HuffNode* root);

/*
* Recursive function for building a codeword map
*/
void codewordmap_helper(HuffNode* tree, std::map<char,std::string>& encodingMap, std::string code="");

bool write_header(const std::vector<std::pair<char, std::string>> &new_codewords, std::ofstream & output);
bool write_codewords(const std::map<char, std::string> &codewords, std::ifstream &input, std::ofstream &output);
std::vector<std::pair<char, std::string>> encode_with_codeword_length(std::map<char,std::string>& codewords);
std::vector<std::string> codewords_from_codeword_length(std::vector<int> lengths);


//Encodes the given file with huffman
std::string huffman_encode(std::string inputFileName);
void huffman_decode(std::string filePath, std::string fileExtension);

#endif