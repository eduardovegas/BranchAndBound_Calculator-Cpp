#ifndef NODE_H
#define NODE_H

#include <vector>

class Node
{
    
    public:

        int val;
        int indexVal;
        bool min;
        bool done;
        bool type; // (false - >=) || (true - <=)
        double sol;

        std::vector<double> variables;
        
        Node();
        ~Node();
        Node(bool ty, int v, int iv);
        
};

#endif