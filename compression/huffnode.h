#include <string>
#include <map>
#include <queue>


#ifndef HUFFNODE_H
#define HUFFNODE_H
struct HuffNode
{
    HuffNode(char character, unsigned int frequency) : character{character}, frequency{frequency}, zeroChild{nullptr}, oneChild{nullptr}
    {}

    HuffNode(unsigned int frequency, HuffNode* zeroChild, HuffNode* oneChild) : frequency{frequency}, zeroChild{zeroChild}, oneChild{oneChild}
    {}

    char character{};
    unsigned int frequency{};

    HuffNode* zeroChild{};//Pointers to children of the node
    HuffNode* oneChild{}; //Both children == nullptr => node is a leaf

    bool isLeaf() const
    {
        return zeroChild == nullptr && oneChild == nullptr;    
    }

    bool operator<(const HuffNode &other) const
    {
        return this->frequency > other.frequency;
    }

    /*
    * Frees all memory allocated by the tree which the node is the root of.
    */
    void freeTree()
    {
        if(this->isLeaf())
        {
            delete this;
            return;
        }
        if(zeroChild != nullptr)
        {
            zeroChild->freeTree();
        }
        if(oneChild != nullptr)
        {
            oneChild->freeTree();
        }
        delete this;
    }

};



#endif