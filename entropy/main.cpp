
#include <iostream>
#include <fstream>
#include "entropy.h"
#include <string>

/*
 * Author: Nils Johansson
 *  
 * Compute entropies for files
 * 
 * Usage:
 *  1. 
 *     Calculate entropy for one file:
 *     $ ./a.out [filepath] [order]
 *  2. 
 *      Calculate entropies for multiple files. Filepaths to all files should be contained in the given file where each line contains one filepath
 *      $ ./a.out compute [filepath] [order]
 */
int main(int argc, char* argv[])
{
    
    if(argc == 3)
    {
        std::string path = argv[1];
        int order = std::stoi(argv[2]);
        std::ifstream input(path);
        entropy(input, order);
    }
    else if(argc == 4)
    {
        if(std::string(argv[1]) == "compute")
        {
            std::ofstream output("entropies.txt");
            std::ifstream files(argv[2]);
            int order = std::stoi(argv[3]);
            std::string file_path;
            while(getline(files,file_path))
            {
                std::ifstream input(file_path);
                std::vector<double> rates = entropy(input, order);
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