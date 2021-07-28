/*
    genRefs.C
    
    ILOC generation of data references
    
    first draft Summer 2012 cs1629@txstate.edu
*/


#include "extraction/genILOC.h"


/* generate ILOC for a data reference */
struct ILLine *generateReference (AstNode *refNode, struct ILOCSymbol &location)
{
    int kind = refNode->getType();
    struct ILLine *head = NULL;
    struct ILLine *tail = NULL;
    union Operand destOperand, sourceOperand;
    
    switch (kind)
    {
    case SCALAR_REF:
    case ID_REF:
        //printf ("DEBUG: scalar reference node type #%d, %s.\n", kind, nodeNames[kind]);
        if (notSpecified (location))
        {
            // no code needed, just use the value
            location = getScalarRef (refNode);
            head = NULL;
        }
        else
        {
            /* load from a scalar value to desired location */
            destOperand.variable = location;
            sourceOperand.variable = getScalarRef (refNode);
            head = buildILLine2 (ILOC_I2I, destOperand, sourceOperand);
        }
        break;
    case ARRAY_REF:     /* TODO */
        /* compute offset and load value into a temp var for use */
    case STRUCT_REF:
        /* compute offset and load value into a temp var for use */

        //printf ("DEBUG: reference node type #%d, %s, not yet implemented.\n", kind, nodeNames[kind]);
        head = buildILNoOp();
        /* if caller doesn't have a specific place for result, create a temp */
        if (notSpecified (location))
            nextKernelTemp (location);
        /* TODO: ... */
        break;
    case INT_REF:        /* TODO: figure out how exactly we want to distinguish constants from named values */
        /* return constant value for use */
        /* if caller doesn't have a specific place for result, create a temp */
        if (notSpecified (location))
            nextKernelTemp (location);
        destOperand.variable = location;
        sourceOperand.longValue = refNode->getVal();
        head = buildILLine2 (ILOC_LOADI, destOperand, sourceOperand);
        break;
    case FLOAT_REF:
        /* return constant value for use */
        /* if caller doesn't have a specific place for result, create a temp */
        if (notSpecified (location))
            nextKernelTemp (location);
        destOperand.variable = location;
        sourceOperand.doubleValue = refNode->getFVal();
        head = buildILLine2 (ILOC_FLOADI, destOperand, sourceOperand);
        break;
    case CHAR_REF:
        /* return constant value for use */
        /* if caller doesn't have a specific place for result, create a temp */
        if (notSpecified (location))
            nextKernelTemp (location);
        destOperand.variable = location;
        sourceOperand.longValue = refNode->getVal();
        head = buildILLine2 (ILOC_CLOADI, destOperand, sourceOperand);
        break;
    case STR_REF:
        /* load effective address for use? */
    default:
        printf ("DEBUG: unexpected reference node, type %d: %s.\n", kind, nodeNames[kind]);
        head = buildILNoOp();
    /* TODO: placeholder needed? */
        location.identifier = -1;
        location.instance = -1;
    }
    
    return head;
} /* generateReference() */


/* returns a symbolic representation of a scalar reference */
struct ILOCSymbol getScalarRef (AstNode *refNode)
{
    int kind = refNode->getType();
    struct ILOCSymbol sym;
    
    if ((kind != SCALAR_REF) && (kind != ID_REF))
        printf ("DEBUG: %s encountered while expecting scalar reference.\n", nodeNames[kind]);
    
    /* set the identifier and instance of */
    sym.identifier = refNode->getVal ();
    sym.instance = 0;           /* TODO: this may need work */
    //printf ("DEBUG: getScalarRef(): id= %d\n", sym.identifier);
    
    return sym;
}


/* end of genRefs.C */
