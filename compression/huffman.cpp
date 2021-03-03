#include <iostream>
#include <fstream>
#include <exception>
#include <bitset>
#include <cmath>
#include <algorithm>

#include "huffman.h"
#include "binaryIO.h"


//Buils a frequency table from the input stream where the key is the character and the value is the frequency of the character in the stream
map<char, int> build_freqtable(istream& input)
{
    map<char, int> freqTable;
    char character;
    char eof = char_traits<char>::eof();
    while(input.get(character))
    {
        if(freqTable.find(character) == freqTable.end())
        {                
            freqTable.insert(pair<int,int>(character,1));
        }
        else
        {
            freqTable.at(character) +=1;
        }
    }
    return freqTable;
}

/*
*  Builds a huffman tree based on a frequency table, returns root node to huffman tree
*/
HuffNode* build_huffmantree(const map<char, int> &freqTable)
{
    priority_queue<HuffNode> nodeQueue;
    for (auto const& freqPair: freqTable)
    {
        nodeQueue.push(HuffNode(freqPair.first, freqPair.second)); // Push all leaf nodes
    }
    return huffmantree_builder(nodeQueue);
}

/*
*  Recursive function that constructs the huffman tree
*/
HuffNode* huffmantree_builder(priority_queue<HuffNode> &nodeQueue)
{
    if(nodeQueue.empty())
    {
        return nullptr;
    }
    if(nodeQueue.size() == 1)
    {
        return new HuffNode(nodeQueue.top());
    }
    else
    {
        //Take the two nodes with the smallest frequencies and create a parent to them
        HuffNode* node0 = new HuffNode(nodeQueue.top());
        nodeQueue.pop();
        HuffNode* node1 = new HuffNode(nodeQueue.top());
        nodeQueue.pop();
        nodeQueue.push(HuffNode(node0->frequency + node1->frequency, node0, node1));

        return huffmantree_builder(nodeQueue);
    }
}

/*
*Builds an encodingMap containing characters as the key and their codeword as value
*/
map<char, string> build_codewordmap(HuffNode* root)
{
    map<char, string> encodingMap;
    if(root != nullptr)
    {
        codewordmap_helper(root, encodingMap);
    }
    return encodingMap;
}

/*
* Recursive function for building a codeword map
*/
void codewordmap_helper(HuffNode* tree, map<char,string>& encodingMap, string code)
{
    if(tree->isLeaf())
    {
        encodingMap.insert(pair<int,string>(tree->character, code));
    }
    else
    {
        codewordmap_helper(tree->zeroChild, encodingMap, code + "0");
        codewordmap_helper(tree->oneChild, encodingMap, code + "1");
    }
}

//Takes a vector containing the length of codewords and returns codewords for
//the huffman tree
vector<string> codewords_from_codeword_length(vector<int> lengths)
{
    int c = 0;
    vector<string> codes(lengths.size());
    string binary = std::bitset<512>(c).to_string();
    codes[0] = binary.substr(binary.size()-lengths[0], lengths[0]);
    for (int i = 1; i < lengths.size(); i++)
    {
        c++;
        c = c*pow(2, lengths[i]-lengths[i-1]);
        string binary = bitset<512>(c).to_string();
        codes[i] = binary.substr(binary.size()-lengths[i], lengths[i]);
    }
    return codes;
}

//Set the the codewords to be based on the length of the original codewords
//Returns a vector containing pairs containing symbols and their new codewords
//The returned vector is sorted on codeword length.
//The codeword map that this function takes as an argument is alaso altered so that it contains the new codewords
vector<pair<char, string>> encode_with_codeword_length(map<char,string>& codewords)
{
    vector<int> codeword_lengths;
    vector<pair<char, string>> sorted_codewords;
    for (auto itr = codewords.begin(); itr != codewords.end(); ++itr)
    {
        codeword_lengths.push_back(itr->second.size());
        sorted_codewords.push_back({itr->first, itr->second});
    }
    sort(codeword_lengths.begin(),codeword_lengths.end());
    sort(sorted_codewords.begin(), sorted_codewords.end(), [=](pair<char,string>& a, pair<char,string> & b)
    {
        return a.second.size() < b.second.size();
    }
    );

    vector<string> new_codewords = codewords_from_codeword_length(codeword_lengths);

    for (int i = 0; i < sorted_codewords.size(); i++)
    {
        codewords[sorted_codewords[i].first] = new_codewords[i];
        sorted_codewords[i].second = new_codewords[i];
    }
    return sorted_codewords;
}


bool write_header(const vector<pair<char, string>> &new_codewords, ofstream & output)
{
    //Write header on following format
    //N Number of symbols, S symbol, H*L H codewords with length L
    //NSSSHLHL
    if(new_codewords.size() < 1)
    {
        output.close();
        return false;
    }
    output << (char)(new_codewords.size()-1);
    for (int i = 0; i < new_codewords.size(); i++)
    {
        output << new_codewords[i].first;
    }
    char length_counter {1};
    char previous_length{new_codewords[0].second.size()};
    for (int i = 1; i < new_codewords.size(); i++)
    {
        char previous_length = new_codewords[i-1].second.size();
        char length = new_codewords[i].second.size();
        if(previous_length != length)
        {
            output << length_counter;
            output << previous_length;
            length_counter = 0;
        }
        length_counter++;
        if(i == new_codewords.size()-1)
        {
            output << length_counter;
            output << length;
        }
    }
    return true;
}

bool write_codewords(const map<char, string> &codewords, ifstream &input, ofstream &output)
{
    BitWriter bitWriter{};
    char byte;
    while(readByte(byte, input))
    {
        //Read byte and output the codeword to the ofstream
        string codeword = codewords.at(byte);

        if(codeword == "")
            return false;
        for (const char &bit : codeword)
        {
            int int_bit = bit -'0';
            bitWriter.writeBit(output, int_bit); 
        }
    }
    bitWriter.flush(output);
    return true;
}

/*
 * Encodes the file with the given path with the huffman coding algorithm and returns the path to the encoded file
 */
string huffman_encode(string inputFileName)
{
    cout << "--- Encoding with huffman ---" << endl;
           
    ifstream input(inputFileName);
    map<char, int> freqTable  = build_freqtable(input);

    HuffNode* root = build_huffmantree(freqTable);
    map<char, string> codewords = build_codewordmap(root); //map that contains the symbol as key and the binary codword as value


    //Calculate the rate
    //We will write different codewords(based on the length of the codewords) to the encoded file later.
    //But since all codewords will still be of the same length the rate wont change
    int sum{0};
    int total_freq{0};
    for (auto frq : freqTable)
    {
        total_freq += frq.second;
    }
    
    for (auto codeword : codewords)
    {
        sum += freqTable[codeword.first] * codeword.second.size();
    }
    double rate = (double)sum/total_freq;
    cout.precision(3);

    //Alter the code map so that the codewords are based on the length of the original code words
    vector<pair<char, string>> new_codewords = encode_with_codeword_length(codewords);
    
    input.clear();
    input.seekg(0, ios::beg);

    int dotindex = inputFileName.find_last_of("."); 
    string encodedFile = inputFileName.substr(0, dotindex) + ".huf";
    cout << "Writing encoded data to " << encodedFile << endl;  
    ofstream output(encodedFile);

    if(!write_header(new_codewords, output))
        throw runtime_error("Error when writing header to encoded file");
    if(!write_codewords(codewords,input,output))
        throw runtime_error("Error when writing codewords to encoded file");
    output.close();
    root->freeTree();

    
    cout << "Rate: " << rate << endl; 
    cout << "-----------------------------" << endl;    
    return encodedFile;
}

//Reads the header and returns the length of the header in bytes
//Also takes a codeword map by reference and modifies it 
//so that it ocntains the codewords and the symbols decribed by the header
int read_header(ifstream &input, map<string, char> & codewordmap)
{
    char N;
    readByte(N,input);
    unsigned int iN = (unsigned char)N;
    iN++;
    vector<char> symbols;
    for (unsigned int i = 0; i < iN; i++)
    {
        char symbol;
        readByte(symbol,input);
        symbols.push_back(symbol);
    }
    vector<int> lengths;
    int i{};
    int headerLength{1+iN};//Number of bytes in the header
    while(i < iN)
    {
        char H,L;//H is the number of codewords with length L
        readByte(H,input);
        readByte(L,input);
        unsigned int iL = (unsigned char)L;
        unsigned int iH = (unsigned char)H;
        i += iH;
        headerLength += 2;
        for (int j = 0; j < iH; j++)
        {
            lengths.push_back(iL);
        }
    }
    vector<string> codewords = codewords_from_codeword_length(lengths);
    for (int i = 0; i < codewords.size(); i++)
    {
        codewordmap[codewords[i]] = symbols[i];
    }
    return headerLength;
}

void huffman_decode(string filePath, string fileExtension)
{
    cout << "--- Decoding huffman code ---" << endl;
    cout << "Decoding encoded file: " + filePath << endl;
    ifstream input(filePath);
    map<string, char> codewordmap;
    int headerLength = read_header(input, codewordmap);

    input.close();

    //Get the whole encoded file(except for the header) as a binary string
    //Probably causes problems when decoding very large files and we cant store the string in memory
    string binary = getBinaryString(filePath).substr(headerLength*8);

    //The last byte in the encoded file specifies how many of the last bits in the second to last byte should be read
    int lastByteLength = stoi(binary.substr(binary.size()-8), nullptr, 2); 
    if(lastByteLength == 0)
        lastByteLength = 8;

    string current_binary{""};
    int dotindex = filePath.find_last_of("."); 
    
    string decodedFile =  "decoded"+fileExtension;
    
    cout << "Decoding to file: " << decodedFile << endl;
    ofstream output(decodedFile);
    //Read each bit of the binary string and output the symbols to the codewords that are found
    for (auto it = binary.begin(); it != binary.end()-8*2+lastByteLength; it++)
    {
        current_binary += *it;
        if(codewordmap.find(current_binary) != codewordmap.end())
        {
            output << codewordmap[current_binary];
            current_binary = "";
        }
    }
    cout << "-----------------------------" << endl;
    output.close();
}