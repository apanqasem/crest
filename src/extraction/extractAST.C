/*
    extractAST.c
    
    feature extraction at AST level
    
    Summer 2013
    cs1629@txstate.edu
*/


#include <extractAST.h>
#include <stdio.h>              /* for debugging purposes */

//#include <frontend/symTab.h>    /* displaying symbol names for debugging */
//extern SymTab *symtab;


int countASTLoops (AstNode *node);
int countASTConds (AstNode *node);
int countASTAssigns (AstNode *node);
int countASTFunCalls (AstNode *node);
int countASTFunCallsManyArgs (AstNode *node);
int countASTUnaryOps (AstNode *node);
int countASTBinaryOps (AstNode *node);
int countASTBinaryOpsIConst (AstNode *node);
int countASTAddressTaken (AstNode *node);
int countASTIntConstNum (AstNode *node, int num);
int countASTIntConstAssgn (AstNode *node);
int countASTLocalRefs (AstNode *node, map<int, int> &localVars);
int countReferencedLocals (map<int, int> &localVars);


/* extract certain features based on the AST representation of input code */
/* Features "in beta":
    ft19        (can be off if parser was confused  e.g. old-style type casts, macros, CUDA kernels, ...)
    ft20        (rough estimate [include else clauses? switches??])
    ft21
    ft22/ft23   ("ft22.5" -- combined because cannot reliably distinguish as yet)
    ft34
    ft37/ft38   ("ft37.5" -- combined because cannot reliably distinguish as yet)
    ft40
    ft41
    ft43
    ft46
    ft48
    ft50
    ft52        (depends on ft50) (missing some local decls, refs)
*/
void extractAST (AstNode *root, FeatureVector &vector)
{
    map<int, int> localVars;
    
    int numLoops = countASTLoops (root);
    int numConds = countASTConds (root);
    /* this is a very rough estimate */
    vector["ft20"] = numLoops + numConds;

    FeatureValue numAssigns = countASTAssigns (root);
    vector["ft21"] = numAssigns;

    FeatureValue numFunCalls = countASTFunCalls (root);
    vector["ft19"] = numFunCalls;
    vector["ft43"] = countASTFunCallsManyArgs (root);

    vector["ft34"] = countASTUnaryOps (root);
    vector["ft37.5"] = countASTAddressTaken (root);
    
    int numBinaryOps = countASTBinaryOps (root);
    vector["ft22.5"] = numBinaryOps;
    vector["ft41"] = countASTBinaryOpsIConst (root);
    
    vector["ft46"] = countASTIntConstNum (root, 0);
    vector["ft48"] = countASTIntConstNum (root, 1);
    
    vector["ft40"] = countASTIntConstAssgn (root);
    
    /* ft52 depends on ft50 */
    vector["ft50"] = countASTLocalRefs (root, localVars);
    vector["ft52"] = countReferencedLocals (localVars);
    
} /* end extractAST() */


/* recursively count the number of loop nodes in AST */
int countASTLoops (AstNode *node)
{
    list<AstNode*>::iterator iter;
    list<AstNode*> *children = node->getChildren();
    int count = 0;
    
    if (node->isLoopNode())
        ++count;
    for(iter = node->begin(); iter != node->end(); iter++)
        count += countASTLoops (*iter);
    for(iter = children->begin(); iter != children->end(); iter++)
        count += countASTLoops (*iter);
    return count;
}


/* recursively count the number of conditional nodes in AST */
int countASTConds (AstNode *node)
{
    list<AstNode*>::iterator iter;
    list<AstNode*> *children = node->getChildren();
    int count = 0;
    
    if (node->isCondNode())
        ++count;
    for(iter = node->begin(); iter != node->end(); iter++)
        count += countASTConds (*iter);
    for(iter = children->begin(); iter != children->end(); iter++)
        count += countASTConds (*iter);
    return count;
}


/* recursively count the number of assignment nodes in AST */
int countASTAssigns (AstNode *node)
{
    list<AstNode*>::iterator iter;
    list<AstNode*> *children = node->getChildren();
    int count = 0;
    
    if ( ((node->isAssignNode()) && (node->getNumSiblings() >= 2)) ||
         (node->getType() == VARDECLASSIGN) ||
         (node->getType() == ARRDECLASSIGN) )
        ++count;
    /* run through all siblings and children */
    for(iter = node->begin(); iter != node->end(); iter++)
        count += countASTAssigns (*iter);
    for(iter = children->begin(); iter != children->end(); iter++)
        count += countASTAssigns (*iter);
    return count;
}


/* recursively count the number of function call nodes in AST */
int countASTFunCalls (AstNode *node)
{
    list<AstNode*>::iterator iter;
    list<AstNode*> *children = node->getChildren();
    int count = 0;
    
    if (node->isFunCallNode())
        ++count;
    for(iter = node->begin(); iter != node->end(); iter++)
        count += countASTFunCalls (*iter);
    for(iter = children->begin(); iter != children->end(); iter++)
        count += countASTFunCalls (*iter);
    return count;
}


/* recursively count the number of function calls in AST with more than four arguments */
int countASTFunCallsManyArgs (AstNode *node)
{
    list<AstNode*>::iterator iter;
    list<AstNode*> *children = node->getChildren();
    int count = 0;
    
    if (node->isFunCallNode())
    {
        /* second sibling of a function call node is the argument list */
        iter = node->begin();
        iter++;
        if ((*iter)->getNumSiblings() > 4)
            ++count;
    }
    for(iter = node->begin(); iter != node->end(); iter++)
        count += countASTFunCallsManyArgs (*iter);
    for(iter = children->begin(); iter != children->end(); iter++)
        count += countASTFunCallsManyArgs (*iter);
    return count;
}


/* recursively count the number of unary operation nodes in AST */
int countASTUnaryOps (AstNode *node)
{
    list<AstNode*>::iterator iter;
    list<AstNode*> *children = node->getChildren();
    int count = 0;
    
    if (node->isUnaryOpNode())
        ++count;
    for(iter = node->begin(); iter != node->end(); iter++)
        count += countASTUnaryOps (*iter);
    for(iter = children->begin(); iter != children->end(); iter++)
        count += countASTUnaryOps (*iter);
    return count;
}


/* recursively count the number of binary operation nodes in AST */
int countASTBinaryOps (AstNode *node)
{
    list<AstNode*>::iterator iter;
    list<AstNode*> *children = node->getChildren();
    int count = 0;
    
    if (node->isBinaryOpNode())
        ++count;
    for(iter = node->begin(); iter != node->end(); iter++)
        count += countASTBinaryOps (*iter);
    for(iter = children->begin(); iter != children->end(); iter++)
        count += countASTBinaryOps (*iter);
    return count;
}


/* recursively count the number of binary operations with integer constant operand(s) in AST */
int countASTBinaryOpsIConst (AstNode *node)
{
    list<AstNode*>::iterator iter, left, right;
    list<AstNode*> *children = node->getChildren();
    int count = 0;
    
    left = node->begin();
    right = left++;
    if (node->isBinaryOpNode())
        if (((*left)->getType() == INT_REF) || ((*right)->getType() == INT_REF))
            ++count;
    for(iter = node->begin(); iter != node->end(); iter++)
        count += countASTBinaryOpsIConst (*iter);
    for(iter = children->begin(); iter != children->end(); iter++)
        count += countASTBinaryOpsIConst (*iter);
    return count;
}


/* recursively count the number of times an address is taken in an expression in AST */
int countASTAddressTaken (AstNode *node)
{
    list<AstNode*>::iterator iter;
    list<AstNode*> *children = node->getChildren();
    int count = 0;
    
    if (node->getType() == ADDRESSOFEXPR)
        ++count;
    for(iter = node->begin(); iter != node->end(); iter++)
        count += countASTAddressTaken (*iter);
    for(iter = children->begin(); iter != children->end(); iter++)
        count += countASTAddressTaken (*iter);
    return count;
}


/* recursively count the number of times a particular integer constant occurs in the AST */
int countASTIntConstNum (AstNode *node, int num)
{
    list<AstNode*>::iterator iter;
    list<AstNode*> *children = node->getChildren();
    int count = 0;
    
    if (node->getType() == INT_REF)
        if (node->getVal() == num)
            ++count;
    for(iter = node->begin(); iter != node->end(); iter++)
        count += countASTIntConstNum (*iter, num);
    for(iter = children->begin(); iter != children->end(); iter++)
        count += countASTIntConstNum (*iter, num);
    return count;
}


/* recursively count the number of times RHS of an assignment is an integer constant in the AST */
int countASTIntConstAssgn (AstNode *node)
{
    list<AstNode*>::iterator iter;
    list<AstNode*> *children = node->getChildren();
    int count = 0;
    
    if ((node->getType() == ASSIGN) && (node->getNumSiblings() >= 2))
    {
        iter = node->begin();
        if ((*iter)->isVarNode())
        {
            /* an assignment.  to a variable.  count it if RHS is an int const */
            iter++;
            if ((*iter)->getType() == INT_REF)
                ++count;
        }
    }
    
    if (node->getType() == VARDECLASSIGN)
    {
        iter = node->end();
        iter--;
        if ((*iter)->getType() == INT_REF)
            ++count;
    }
    
    for(iter = node->begin(); iter != node->end(); iter++)
        count += countASTIntConstAssgn (*iter);
    for(iter = children->begin(); iter != children->end(); iter++)
        count += countASTIntConstAssgn (*iter);
    return count;
} /* end countASTIntConstAssgn() */


/* returns raw count of all references to any local variable
    populates localVars pool as it goes */
int countASTLocalRefs (AstNode *node, map<int, int> &localVars)
{
    list<AstNode*>::iterator iter, iter2;
    list<AstNode*> *children = node->getChildren();
    int count = 0;
    int sym;
    
    switch (node->getType())
    {
    case EXTERNDECL:
        /* an external decl should not be included as a local */
        break;
    case VARDECL:
    case VARDECLASSIGN:
        /* local scalar, so add to list of known locals */
        for(iter = node->begin(); iter != node->end(); iter++)
        {
            if ((*iter)->getType () == ID_REF)
            {
                sym = (*iter)->getVal ();
                localVars[sym] = 0;
                //printf ("DEBUG: found decl of %d: '%s', count %lu\n", sym, symtab->lookup(sym), localVars.count (sym));
            }
            else if ((*iter)->getType () == SCALAR_REF)
            {
                iter2 = (*iter)->begin();
                sym = (*iter2)->getVal ();
                localVars[sym] = 0;
                //printf ("DEBUG: found decl of %d: '%s', count %lu\n", sym, symtab->lookup(sym), localVars.count (sym));
            }
            else if ((*iter)->getType () == VARDECLLIST)
            {
                for(iter2 = (*iter)->begin(); iter2 != (*iter)->end(); iter2++)
                {
                    if ((*iter2)->getType () == ID_REF)
                    {
                        sym = (*iter2)->getVal ();
                        localVars[sym] = 0;
                        //printf ("DEBUG: found decl of %d: '%s', count %lu\n", sym, symtab->lookup(sym), localVars.count (sym));
                    }
                } // end for varDeclList's siblings
            }
        } // end for node's siblings
        
        /* if an inital value expression is specified, it will be the last sibling node */
        if (node->getType() == VARDECLASSIGN)
        {
            iter = node->end ();
            iter--;
            count = countASTLocalRefs (*iter, localVars);
        }
        break;
    case ARRDECL:
    case ARRDECLASSIGN:
        /* local array, so add to list of known locals */
        for(iter = node->begin(); iter != node->end(); iter++)
        {
            if ((*iter)->getType () == ARRAY_REF)
            {
                iter2 = (*iter)->begin ();
                sym = (*iter2)->getVal ();
                localVars[sym] = 0;
                //printf ("DEBUG: found array decl of %d: '%s', count %lu\n", sym, symtab->lookup(sym), localVars.count (sym));

                /* check the indexing expression(s) too */
                for (iter2++; iter2 != (*iter)->end(); iter2++)
                    count += countASTLocalRefs (*iter2, localVars);
            }
        } // end for node's siblings
        
        /* if an inital value expression is specified, it will be the last sibling node */
        if (node->getType() == ARRDECLASSIGN)
        {
            iter = node->end ();
            iter--;
            count = countASTLocalRefs (*iter, localVars);
        }
        break;
    case SCALAR_REF:
    case ID_REF:
        /* if this reference is to a local, count it */
        sym = node->getVal ();
        //printf ("DEBUG: found ref to %d, count %lu\n", sym, localVars.count (sym));
        if (localVars.count (sym) > 0)
        {
            localVars[sym]++;
            ++count;
        }
        break;
    case ARRAY_REF:
        iter = node->begin ();
        sym = (*iter)->getVal ();
        //printf ("DEBUG: found ref to %d, count %lu\n", sym, localVars.count (sym));
        if (localVars.count (sym) > 0)
        {
            localVars[sym]++;
            ++count;
        }
        /* check the indexing expression(s) too */
        for (iter++; iter != node->end(); iter++)
            count += countASTLocalRefs (*iter, localVars);
        break;
    case STRUCT_REF:
        iter = node->begin ();
        sym = (*iter)->getVal ();
        //printf ("DEBUG: found ref to %d, count %lu\n", sym, localVars.count (sym));
        if (localVars.count (sym) > 0)
        {
            localVars[sym]++;
            ++count;
        }
        break;
    default:
        /* keep looking recursively */
        for(iter = node->begin(); iter != node->end(); iter++)
            count += countASTLocalRefs (*iter, localVars);
        for(iter = children->begin(); iter != children->end(); iter++)
            count += countASTLocalRefs (*iter, localVars);
    }
    
    return count;
} /* end countASTLocalRefs() */


/* scan the localVars table,
    counting the number of entries with at least one reference */
int countReferencedLocals (map<int, int> &localVars)
{
    map<int, int>::iterator iter;
    int count = 0;
    
    for(iter = localVars.begin(); iter != localVars.end(); iter++)
    {
        //printf ("DEBUG: <%d, %d>\n", iter->first, iter->second);
        if (iter->second > 0)
            ++count;
    }
    
    return count;
}


/* end of extractAST.c */
