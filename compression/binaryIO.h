#include <iostream>
#include <exception>
#include <vector>
#include <bitset>
#include <fstream>

#ifndef BINARYIO_H
#define BINARYIO_H

/*
 * Class used for writing individual bits to an ostream
 */
class BitWriter
{
public:
    BitWriter() = default;

    //Write on bit to the given ostream
    void writeBit(std::ostream &out,int bit);

    //Flushes the buffer to the ostream. Meant to be used when ending the binary output
    //Also adds a byte which specifies how many bits are real in the second to last byre and not padded zeros
    void flush(std::ostream &out, bool lastByte=true);

private:
    unsigned char buffer{0};
    int bitCounter{0};
};

//Reads one byte from the istream to the given char
int readByte(char &out, std::istream &in);

//Returns the binary that the file with the given filename contains
std::string getBinaryString(std::string file);
std::string getBinaryStringFromStream(std::ifstream &in);

#endif