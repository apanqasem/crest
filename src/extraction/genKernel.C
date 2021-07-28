/*
    genKernel.C
    
    do initial ILOC generation at the kernel level
    
    first draft Summer 2012 cs1629@txstate.edu
*/


#include "extraction/genILOC.h"
#include "driver/errors.h"


unsigned int kernelIdentifier;


/* print the AST, showing sibling and child relations */
void printAST (AstNode *node, int depth, int relation)
{
    list<AstNode*>::iterator iter;
    list<AstNode*> *children = node->getChildren();
    int kind = node->getType();
    int j;
    
    /* indent */
    for (j = 1; j < depth; ++j)
        printf ("  ");
    
    if (relation == SIBLING) printf ("..");
    if (relation == CHILD) printf (": ");
    printf("%s\n", nodeNames[kind]);
    
    /* run through siblings */
    for(iter = node->begin(); iter != node->end(); iter++)
    {
        printAST (*iter, depth + 1, SIBLING);
    }
    
    /* run through children */
    for (iter = children->begin(); iter != children->end(); iter++)
    {
        printAST (*iter, depth + 1, CHILD);
    }
}


/* generate ILOC code for a function body */
struct ILLine *generateFunBody (AstNode *node)
{
    struct ILLine *head;
    
    /* TODO: prologue? */
    //puts ("DEBUG: prologue?");
    
    /* generate ILOC for the statement list */
    head = generateStmt (node);
    
    /* TODO: epiloge? */
    //puts ("DEBUG: epilogue?");
    
    return head;
}


/* do initial ILOC generation for a kernel */
struct ILLine *generateKernel (AstNode *kernelRoot)
{
    list<AstNode*>::iterator iter, iter2;
    int k;
    int kind = kernelRoot->getType();
    struct ILLine *head = NULL;
    
    /* quick sanity check... */
    if (kind != KERNEL)
        error ("Expected Kernel AST node.", PROGRAMMER);

    /* iterate through the kernel's siblings */
    for (iter = kernelRoot->begin(); iter != kernelRoot->end(); iter++)
    {
        kind = (*iter)->getType ();
        if (kind == FUNHEAD)
        {
            /* pull kernel's identifier for use in generating temp names */
            AstNode *funStartNode = *((*iter)->begin());
            if (funStartNode->getType() != FUNNAMETYPE)
                error ("Expected Kernel Name & Type node.", PROGRAMMER);
            for (iter2 = funStartNode->begin(); iter2 != funStartNode->end(); iter2++)
            {
                if ((*iter2)->getType () == ID_REF)
                {
                    kernelIdentifier = (*iter2)->getVal ();
                    //printf ("DEBUG: found kernel's ID_REF node... value = %d\n", kernelIdentifier);
                }
            }
        }
        
        if (kind == FUNBODY)
        {
            if (head == NULL)
                head = generateFunBody (*iter);
            else
                error ("Kernel with multiple bodies.", PROGRAMMER);
        }
    }
    
    //printf ("DEBUG: generateKernel() returning %p\n", head);
    
    return head;
} /* generateKernel() */


/* return a usable temp symbol */
void nextKernelTemp (struct ILOCSymbol &temp)
{
    //struct ILOCSymbol temp;
    static int oneup = 0;
    
    temp.identifier = kernelIdentifier;
    temp.instance = ++oneup;
    //printf ("DEBUG: next temp value id: %d, instance: %d\n", temp.identifier, temp.instance);
}


/* set the variable to 'unspecified' */
void setUnspecified (struct ILOCSymbol &var)
{
    var.identifier = -1;
    var.instance = -1;
}


/* returns true if target is not a valid target */
bool notSpecified (struct ILOCSymbol &target)
{
    return (target.identifier < 0) ? true : false;
}


/* end of genKernel.C */
