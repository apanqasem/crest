/*
    genILOC.h
    
    generate ILOC from AST
    
    first draft Summer 2012 cs1629@txstate.edu
*/


#ifndef GENILOC_H
#define GENILOC_H

#include <stdio.h>

#include <ast/ast.h>
#include "extraction/line.h"	/* ILOC list code */
#include<frontend/symTab.h>     /* for displaying names */

extern SymTab *symtab;
extern AstNode *ast;
extern const char *nodeNames[];


/* print the AST to stdout */
#define ROOT        0
#define SIBLING     1
#define CHILD       2
void printAST (AstNode *node, int depth, int relation);

/* do initial ILOC generation for a kernel */
struct ILLine *generateKernel (AstNode *kernelRoot);
struct ILLine *generateStmt (AstNode *stmtNode);
struct ILLine *generateExpr (AstNode *exprNode, struct ILOCSymbol &result);
struct ILLine *generateBiOpExpr (AstNode *exprNode, struct ILOCSymbol &result);
struct ILLine *generateUnaryExpr (AstNode *exprNode, struct ILOCSymbol &result);
struct ILLine *generateReference (AstNode *refNode, struct ILOCSymbol &location);

/* helper functions for building ILOC */
struct ILOCSymbol getScalarRef (AstNode *refNode);
void nextKernelTemp (struct ILOCSymbol &temp);
void setUnspecified (struct ILOCSymbol &var);
bool notSpecified (struct ILOCSymbol &target);


/* helper macros */
#define stripHead(head)         {if (head->next)\
                                    {head = head->next;\
                                    deleteILLine (head->prev);\
                                    head->prev = NULL;}\
                                 else\
                                    {deleteILLine (head);\
                                    head = NULL;}\
                                }


#endif  /* end of genILOC.h */
