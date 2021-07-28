/*
    genStmts.C
    
    do initial ILOC generation at the statement level
    
    first draft Summer 2012 cs1629@txstate.edu
*/


#include "extraction/genILOC.h"
#include<frontend/symTab.h>
extern SymTab *symtab;


/* generate ILOC for a statement */
struct ILLine *generateStmt (AstNode *stmtNode)
{
    list<AstNode*>::iterator iter;
    list<AstNode*> *children = stmtNode->getChildren();
    AstNode *targetNode;
    AstNode *exprNode;
    int kind = stmtNode->getType();
    struct ILLine *head = NULL;
    struct ILLine *tail = NULL;
    struct ILLine *code = NULL;
    union Operand target, value;
    
    /* for developement purposes: sanity check we are looking at some sort of statement */
    if (!(stmtNode->isStmtNode()) && (kind != FUNBODY))
        printf ("DEBUG: %s node is not known to be a statement node.\n", nodeNames[kind]);
    
    /* deal with the current node depending on what kind of node it is */
    //printf ("DEBUG: generateStmt(): %s node.\n", nodeNames[kind]);
    switch (kind)
    {
    case FUNBODY:
    case LOOPBODY:      // does this belong here?
    case CONDBODY:
    case COMPOUND:
    case STMTLIST:
        /* start building block */
        //printf ("DEBUG: %s group of statements\n", nodeNames[kind]);
        tail = head = buildILNoOp();
        
        /* run through siblings then children */
        //printf ("DEBUG: %s siblings...\n", nodeNames[kind]);
        for(iter = stmtNode->begin(); iter != stmtNode->end(); iter++)
            tail = addILList (tail, generateStmt (*iter));
        //printf ("DEBUG: %s children...\n", nodeNames[kind]);
        for (iter = children->begin(); iter != children->end(); iter++)
            tail = addILList (tail, generateStmt (*iter));
        
        /* strip placeholder no-op */
        //printf ("DEBUG: about to strip head at %p\n", head);
        stripHead (head);
        break;
    case VARDECL:
    case ARRDECL:
        //printf ("DEBUG: declaration statement (%s)\n", nodeNames[kind]);
        head = NULL;        /* no code needed */
        break;
    case VARDECLASSIGN:
        //printf ("DEBUG: declaration statement (%s)\n", nodeNames[kind]);
        tail = head = buildILNoOp();
        for (iter = stmtNode->begin(); iter != stmtNode->end(); iter++)
        {
            kind = (*iter)->getType ();
            if ((kind == QUAL) || (kind == TYPE) || (kind == IDTYPE))
            { /* do nothing */ }
            else if ((kind == SCALAR_REF) || (kind == ID_REF))
            {
                /* set the target variable */
                targetNode = *iter;
                target.variable = getScalarRef (*iter);
            }
            else
            {
                //printf ("DEBUG: varDeclAssign: evaluating %s\n", nodeNames[kind]);
                code = generateExpr (*iter, target.variable);
                tail = addILList (tail, code);
            }
        }
        /* strip placeholder no-op */
        stripHead (head);
        break;
    case ARRDECLASSIGN:
/* TODO: complete this */
        //printf ("TODO: declaration statement (%s)\n", nodeNames[kind]);
        head = buildILNoOp();
        break;
    case ASSIGNADD:
    case ASSIGNSUB:
    case ASSIGNRS:
        /* TODO */
        //printf ("TODO: assignment statement type %s is incomplete\n", nodeNames[kind]);
    case ASSIGN:    /* TODO: belongs at front of this sub-list */
        //printf ("DEBUG: assignment statement (%s)\n", nodeNames[kind]);
        
        /* handle stand-alone expressions */
        if (stmtNode->getNumSiblings () == 1)
        {
            setUnspecified (target.variable);
            head = generateExpr (stmtNode, target.variable);
            return head;
        }
/* TODO: complete this for shorthand assigns! */
        iter = stmtNode->begin ();
        targetNode = *iter;
        iter++;
        exprNode = *iter;
        nextKernelTemp (value.variable);
        //printf ("DEBUG: temp value for assignment: %d.%d\n", value.variable.identifier, value.variable.instance);
        switch (targetNode->getType ())
        {
		case SCALAR_REF:
		/*case ID_REF:*/
            /*printf ("DEBUG: assigning to %s\n", symtab->lookup(targetNode->getVal()));*/
            target.variable = getScalarRef (targetNode);
            head = generateExpr (exprNode, target.variable);
            break;
        case ARRAY_REF:
            /* TODO */
        case STRUCT_REF:
            /* TODO */
        default:
			//printf ("TODO: unimplemented assignment target (%s)\n", nodeNames[targetNode->getType ()]);
			head = buildILNoOp();
        }
        break;
    case LOOP:
    case DWLOOP:
    case COND:
    case RET:
/* TODO: implement these cases */
        //printf ("TODO: control stmt (%s)\n", nodeNames[kind]);
        head = buildILNoOp();
        break;
    case EXPR:
    case FUNCALL:
        //printf ("DEBUG: %s statement\n", nodeNames[kind]);
        setUnspecified (target.variable);
        head = generateExpr (stmtNode, target.variable);
        break;
    default:
        printf ("DEBUG: unexpected statement node, type %d: %s.\n", kind, nodeNames[kind]);
        head = buildILNoOp();
    } /* end switch */
    
    //printf ("DEBUG: generateStmt() returning %p.\n", head);
    return head;
} /* generateStmt() */


/* end of genStmts.C */
