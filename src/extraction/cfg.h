/*
    cfg.h
*/


#ifndef CFG_H
#define CFG_H


#include "extraction/line.h"

#define DEFAULT_EDGEIN_CAPACITY     4


class BasicBlock
{
public:
    BasicBlock ();                      // default constructor
    BasicBlock (struct ILLine *code);   // constructor to build from ILOC code

    void attachParent (BasicBlock *parent, int index = 0);

private:
    // N edges in stored as a vector
    int numEdgesIn;             // current number of edges leading into this block
    int capEdgesIn;             // current vector capacity
    BasicBlock **edgeIn;

    // edges out ( 0-2 )
    int numEdgesOut;
    BasicBlock *edgeOut[2];     // index 0: path out if unconditional, true path if conditional
                                // index 1: false path if conditional

    // block of code
    struct ILLine *code;

    void addEdgeIn (BasicBlock *parent);
    void addEdgeOut (BasicBlock *child);
};


#endif  /* end of cfg.h */
