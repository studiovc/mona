#ifndef __NODE_H__
#define __NODE_H__

#include <vector>
#include <string>

namespace monash {

class Node
{
public:
    Node(int type) : type(type) {}
    ~Node() {}

    typedef std::vector<Node*> Nodes;
    Nodes nodes;
    int type;
    std::string text;
    int value;
    enum
    {
        NODES,
        NUMBER,
        SYMBOL,
        STRING,
        QUOTE,
    };
    void print(int depth = 0);
    std::string typeToString();
};

}; // namespace monash

#endif // __NODE_H__