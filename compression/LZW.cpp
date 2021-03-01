#include <vector>
#include <map>
#include <iostream>
#include "binaryIO.h"
#include <map>
#include <cmath>
#include <string>
#include "LZW.h"

using namespace std;

//Computes log2(n) rounded up
int ceil_log2(int n)
{
    return (int)ceil(log(n)/log(2));
}

//Converts a binary string to an integer
int binary_to_int(string binary)
{
    return stoi(binary, nullptr,2);
}

void lz_encode(ifstream &input, ofstream &out)
{
    cout << "--- Encoding with LZW ---" << endl;
    //Initialize codeword dictionary with all possible single symbols
    map<string, unsigned int> code_dict;
    unsigned char i{0};
    while(true)
    {
        code_dict[string(1,i)] = i;
        if(++i == 0) //If we overflow we have added all chars
            break;
    }
    unsigned int dict_size = 256;
    
    //Read symbols
    string read_code = "";
    char read_symbol;
    string encoded_string = "";
    while(input.get(read_symbol))
    {
        read_code += read_symbol;
        if(code_dict.count(read_code) == 0)
        {
            string dict_word = read_code.substr(0, read_code.size()-1); //The symbol sequence we want to output now
            int word_length = ceil_log2(dict_size);
            //Get the binary codeword with the correct size
            string binary = bitset<128>(code_dict.at(dict_word)).to_string().substr(128-word_length,128);
            encoded_string += binary;
            code_dict[read_code] = dict_size++;  //Add new codeword
            read_code = read_symbol;
        }
    }
    //Output the last symbol
    int word_length = ceil_log2(dict_size);
    string binary = bitset<128>(code_dict.at(read_code)).to_string().substr(128-word_length,128);
    encoded_string += binary;

    //Write the bitstream to a file
    BitWriter bit_writer{};
    for (int i = 0; i < encoded_string.size(); i++)
    {
        string bit = string(1, encoded_string[i]);
        bit_writer.writeBit(out, stoi(bit));
    }
    bit_writer.flush(out, false);
    out.close();

}

void lz_decode(ifstream &in, ofstream &out)
{
    cout << "--- Decoding with LZW ---" << endl;
    //Initialize codeword dictionary with all possible single symbols
    map<unsigned int, string> code_dict;
    unsigned char i{0};
    while(true)
    {
        code_dict[i] = string(1,i);
        if(++i == 0) //If we overflow we have added all chars
            break;
    }
    unsigned int dict_size = 256;

    string codeword_binary = ""; //Read each binary codeword into a string
    int codeword_length = ceil_log2(dict_size); //Length of the next codeword to read
    string bitstream_string = getBinaryStringFromStream(in);//Whole encoded file in binary
    int pos = 0; //Position of the next codeword to read
    pair<string,string> symbols; //keep track of the two most recent symbol sequences read
    codeword_binary = bitstream_string.substr(pos, codeword_length);
    symbols.second = code_dict[binary_to_int(codeword_binary)]; //Get the symbol that corespond to the index in the codebook
    pos += codeword_length;
    dict_size++;
    codeword_length = ceil_log2(dict_size);
    out << symbols.second;

    //read indexes while the position for the next index to read starts before the last byte
    while(pos < bitstream_string.size()-8)
    {
        symbols.first = symbols.second;
        codeword_binary = bitstream_string.substr(pos, codeword_length); //Get the next index
        unsigned int codeword_index = binary_to_int(codeword_binary);
        if(codeword_index == dict_size-1)
        {
            //The symbol we want to get from the codebook is the one we are about to insert
            symbols.second = symbols.first + symbols.first[0];
        }
        else
        {
            symbols.second = code_dict[codeword_index];
        }
        out << symbols.second;
        string new_codeword = symbols.first + symbols.second[0]; //new symbol sequence to add to the codebook
        code_dict[dict_size-1] = new_codeword;
        dict_size++;
        pos += codeword_length;
        codeword_length = ceil_log2(dict_size);
    }
    
}