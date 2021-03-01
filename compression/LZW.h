#include <fstream>

#ifndef LZW_H
#define LZW_H
void lz_encode(std::ifstream &in, std::ofstream &out);
void lz_decode(std::ifstream &in, std::ofstream &out);

#endif