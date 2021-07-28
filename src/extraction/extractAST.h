/*
    extractAST.h
    
    feature extraction at AST level
    
    Summer 2013
    cs1629@txstate.edu
*/


#ifndef EXTRACTAST_H
#define EXTRACTAST_H


#include <ast/ast.h>
#include <extraction/featureVector.h>


/* function prototype for AST-level feature extraction */
void extractAST (AstNode *root, FeatureVector &vector);


#endif  /* end of extractAST.h */
