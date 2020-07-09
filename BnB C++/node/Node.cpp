#include "Node.h"

Node::Node()
{   
    val = 0;
    indexVal = 0;
    min = false;
    done = false;
    type = true;
    sol = -9999999999999;
}

Node::Node(bool ty, int v, int iv)
{   
    val = v;
    indexVal = iv;
    min = false;
    done = false;
    type = ty;
    sol = -9999999999999;
}

Node::~Node()
{

    //printf("deletou\n");

}