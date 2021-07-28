/*
    cfg.C
*/


#include "extraction/cfg.h"
#include "driver/errors.h"


// default constructor
BasicBlock::BasicBlock ()
{
    numEdgesIn = 0;
    capEdgesIn = DEFAULT_EDGEIN_CAPACITY;
    edgeIn = new BasicBlock *[capEdgesIn];

    numEdgesOut = 0;
    edgeOut[0] = NULL;
    edgeOut[1] = NULL;

    code = NULL;
}


// constructor to build from ILOC code
BasicBlock::BasicBlock (struct ILLine *iloc)
{
    numEdgesIn = 0;
    capEdgesIn = DEFAULT_EDGEIN_CAPACITY;
    edgeIn = new BasicBlock *[capEdgesIn];

    numEdgesOut = 0;
    edgeOut[0] = NULL;
    edgeOut[1] = NULL;

    code = iloc;
}


// if index == 0, attach as unconditional / true branch (default)
// if index == 1, attach as false branch
void BasicBlock::attachParent (BasicBlock *parent, int index)
{
    if ((index < 0) || (index > 1))
        error ("invalid edge out index", PROGRAMMER);

    if (parent->edgeOut[index] == NULL)
    {
        parent->numEdgesOut++;
        parent->edgeOut[index] = this;
        addEdgeIn (parent);
    }
    else
    {
        error ("redefinition of an edge out", PROGRAMMER);
    }
}


void BasicBlock::addEdgeIn (BasicBlock *parent)
{
    // expand vector if needed
    if (numEdgesIn >= capEdgesIn)
    {
        int newCapEdgesIn = capEdgesIn * 2;
        BasicBlock **newEdgeIn = new BasicBlock *[newCapEdgesIn];
        
        for (int i = 0; i < capEdgesIn; ++i)
            newEdgeIn[i] = edgeIn[i];
        
        delete [] edgeIn;
        
        capEdgesIn = newCapEdgesIn;
        edgeIn = newEdgeIn;
    }
    
    edgeIn[numEdgesIn] = parent;
    ++numEdgesIn;
}


void BasicBlock::addEdgeOut (BasicBlock *child)
{
    if (numEdgesOut < 2)
    {
        edgeOut[numEdgesOut] = child;
        ++numEdgesOut;
    }
    else
    {
        error ("too many edges out", PROGRAMMER);
    }
}


/* end of cfg.C */
