#include "binaryIO.h"
#include <iostream>
#include <exception>
#include <vector>
#include <bitset>
#include <fstream>


void BitWriter::writeBit(std::ostream &out, int bit)
{
    if(bit != 0 && bit != 1)
    {
        std::cout << "Got: " << bit << std::endl;
        throw std::invalid_argument("WriteBit only takes a bit(0 or 1)");
    }
    if(bit == 1)
    {
        buffer |= 1 << (7-bitCounter);
    }
    bitCounter++;
    if(bitCounter == 8)
    {
        out << buffer;
        bitCounter = 0;
        buffer = 0;
    }
}

//Flushes the buffer to the ostream. Meant to be used when ending the binary output
//Also adds a byte which specifies how many bits are real and not padded zeros if the second arg is set to true
void BitWriter::flush(std::ostream &out, bool lastByte)
{
    unsigned char validBits = bitCounter;
    while(bitCounter != 0)//Padd with zeros and flush
        writeBit(out, 0);
    //At the end of the file specify how many bits in the second to last byte are valid(0 => all are valid)
    //For example 2 => the two most significant bits are valid, the rest is padded zeros
    if(lastByte)
        out << validBits; 
    bitCounter = 0;
    buffer = 0;
}

/*
    Useful functions for reading binary from files and streams
*/

//Reads one byte from the istream to the given char
int readByte(char &out, std::istream &in)
{
    if(!in.get(out))
    {
        return 0;
    }
    return 1;
}

//Returns the binary that the file with the given filename contains
std::string getBinaryString(std::string file)
{
    std::ifstream is(file, std::ios::binary);
    std::vector<char> bin(std::istreambuf_iterator<char>(is),{});
    std::string binaryString{};
    for(auto it = bin.begin(); it != bin.end(); it++)
    {
        binaryString += std::bitset<8>(*it).to_string();  
    }
    is.close();
    return binaryString;
}

//Returns the binary that the file with the given filename contains
std::string getBinaryStringFromStream(std::ifstream &in)
{
    std::vector<char> bin(std::istreambuf_iterator<char>(in),{});
    std::string binaryString{};
    for(auto it = bin.begin(); it != bin.end(); it++)
    {
        binaryString += std::bitset<8>(*it).to_string();  
    }
    in.close();
    return binaryString;
}