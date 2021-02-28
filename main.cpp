
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <bitset>

#include "binaryIO.h"
#include "huffman.h"
#include "LZW.h"
using namespace std;

/*
 * Author: Nils Johansson
 *  
 */

//Usage:    $ ./a.out filepath encoding
//Example:  $ ./a.out bible.txt huffman
int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        cout << "error: invalid number of arguments" << endl;
        cout << "Usage:" << endl;
        cout << "$ ./a.out filepath encoding" << endl;
        cout << "Example:" << endl;
        cout << "$ ./a.out bible.txt huffman" << endl;
        return 1;
    }
    string file = argv[1];
    string encoding = argv[2];
    if(encoding == "huffman")//TODO: pass streams instead of file name
    {
        string encoded = encode(file);
        int dotindex = file.find_last_of("."); 
        string fileExtension =  file.substr(dotindex);
        cout << "File extension: " << fileExtension << endl;
        decode(encoded, fileExtension);
    }
    else if(encoding == "lzw" || encoding == "LZW")
    {
        ifstream in(file);
        ofstream out("encoded.lzw");
        lz_encode(in, out);
        ifstream encoded_in_stream("encoded.lzw", ios::binary);
        int dotindex = file.find_last_of("."); 
        string fileExtension =  file.substr(dotindex);
        ofstream out_decoded("decoded"+fileExtension);
        lz_decode(encoded_in_stream, out_decoded);

    }
    else
    {
        cout << "error: invalid input" << endl;
        cout << "Usage: $ ./a.out filepath encoding" << endl;
        cout << "Example: $ ./a.out bible.txt huffman" << endl;
    }

    return 0;
}