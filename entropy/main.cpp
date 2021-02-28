
#include <iostream>
#include <fstream>
#include "entropy.h"

using namespace std;
/*
 * Author: Nils Johansson
 *  
 * Compute entropies for files
 * 
 * Usage:
 *  1. 
 *     Calculate entropy for one file:
 *     $ ./a.out [filepath]
 *  2. 
 *      Calculate entropies for multiple files. Filepaths to all files should be contained in the given file where each line contains one relative filepath to the curent dir
 *      $ ./a.out compute [filepath]
 *  3.
 *      $ ./a.out sparse [filepath] compute the entropy with a sparse matrix solver algorithm
 * 
 * 
 */
int main(int argc, char* argv[])
{
    
    if(argc == 2)
    {
        string path = argv[1];
        ifstream input(path);
        entropy(input);
    }
    else if(argc == 3)
    {
        if(string(argv[1]) == "compute")
        {
            ofstream output("entropies.txt");
            ifstream files(argv[2]);
            string file_path;
            while(getline(files,file_path))
            {
                ifstream input(file_path);
                vector<double> rates = entropy(input);
                output << file_path << ": " << endl;
                for(double rate: rates)
                {
                    output << rate << "\n";
                }
            }
            output.close();
        }
        else if(string(argv[2]) == "sparse")
        {
            string path = argv[1];
            ifstream input(path);
            entropy(input, true);
        }
    }
    else
    {
        cout << "Bad args" << endl;
    } 
    
    return 0;
}