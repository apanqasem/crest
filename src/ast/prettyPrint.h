#ifndef PRETTYTPRINT_H
#define PRETTYTPRINT_H


#include<ast/ast.h>

#define INDENT 2

void prettyPrintAst(AstNode *node, int nestLevel);
void prettyPrintSubtree(AstNode *node, int nestLevel);


#endif 
