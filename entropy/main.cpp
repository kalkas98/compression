
#include <iostream>
#include <fstream>
#include "entropy.h"

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
 *      Calculate entropies for multiple files. Filepaths to all files should be contained in the given file where each line contains one filepath
 *      $ ./a.out compute [filepath]
 */
int main(int argc, char* argv[])
{
    
    if(argc == 2)
    {
        std::string path = argv[1];
        std::ifstream input(path);
        entropy(input);
    }
    else if(argc == 3)
    {
        if(std::string(argv[1]) == "compute")
        {
            std::ofstream output("entropies.txt");
            std::ifstream files(argv[2]);
            std::string file_path;
            while(getline(files,file_path))
            {
                std::ifstream input(file_path);
                std::vector<double> rates = entropy(input);
                output << file_path << ": " << std::endl;
                output.precision(3);
                for(double rate: rates)
                {
                    output << rate << "\n";
                }
            }
            output.close();
        }
    }
    else
    {
        std::cout << "Bad args" << std::endl;
    } 
    
    return 0;
}