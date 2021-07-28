/*
    genExprs.C
    
    do initial ILOC generation at the expression level
    
    first draft Summer 2012 cs1629@txstate.edu
*/


#include "extraction/genILOC.h"
#include "driver/errors.h"


/* generate ILOC for an expression */
struct ILLine *generateExpr (AstNode *exprNode, struct ILOCSymbol &result)
{
    int kind = exprNode->getType();
    struct ILLine *head = NULL;
    struct ILLine *tail = NULL;
    struct ILLine *code = NULL;
    AstNode *leftSide, *rightSide;
    union Operand leftResult, rightResult, exprResult;
    enum Opcode opCode;
    list<AstNode*>::iterator iter;
    
    /* handle references */
    if (exprNode->isReferenceNode ())
    {
        //printf ("DEBUG: reference node (%s)\n", nodeNames[kind]);
        return generateReference (exprNode, result);
    }
    
    /* handle binary operations */
    if (exprNode->isBinaryOpNode ())
    {
    	//printf ("DEBUG: binary operation %s\n", nodeNames[kind]);
        return generateBiOpExpr (exprNode, result);
    }
    
    switch (kind)
    {
    case CONDEXPR:
        //printf ("DEBUG: %s conditional expression\n", nodeNames[kind]);
        /* if caller doesn't have a specific place for result, create a temp */
        if (notSpecified (result))
            nextKernelTemp (result);

        head = buildILNoOp();
/* TODO: implement this */
        break;
    case LTEEXPR:
    case LTEXPR:
    case GTEEXPR:
    case GTEXPR:
    case EQEXPR:
    case NEQEXPR:
    case ADDEXPR:
    case SUBEXPR:
    case MULEXPR:
    case DIVEXPR:
    case MODEXPR:
    case LOGANDEXPR:
    case LOGOREXPR:
    case BITANDEXPR:
    case BITOREXPR:
    case BITXOREXPR:
    case SHIFTLEFTEXPR:
    case SHIFTRIGHTEXPR:
        /* should never get here - handled above */
        printf ("DEBUG: (should not get here) binary expression %s\n", nodeNames[kind]);
        head = buildILNoOp();
        break;
    case NEGEXPR:
    case NOTEXPR:
    case DEREFEXPR:
    case ADDRESSOFEXPR:
        //printf ("DEBUG: unary operation %s\n", nodeNames[kind]);
        head = generateUnaryExpr (exprNode, result);
        break;
    case FUNCALL:
    /*case KERNELCALL:*/
        //printf ("DEBUG: %s expression\n", nodeNames[kind]);
        /* if caller doesn't have a specific place for result, create a temp */
        if (notSpecified (result))
            nextKernelTemp (result);

        head = buildILNoOp();
/* TODO: figure out exactly how we want to compile function calls in ILOC... */
        break;
    case PARENEXPR:
        //printf ("DEBUG: parenthesized expression (%s)\n", nodeNames[kind]);
        head = generateExpr (*(exprNode->begin ()), result);
        break;
    case TYPECASTEXPR:
        //printf ("DEBUG: type cast expression (%s)\n", nodeNames[kind]);
        /* if caller doesn't have a specific place for result, create a temp */
        if (notSpecified (result))
            nextKernelTemp (result);

        head = buildILNoOp();
/* TODO: implement this */
        break;
    case ASSIGN:
        //printf ("DEBUG: (stand-alone?) assign expression (%s)\n", nodeNames[kind]);
        head = generateExpr (*(exprNode->begin ()), result);
        break;
    default:
        printf ("DEBUG: unexpected expression node, type %d: %s.\n", kind, nodeNames[kind]);
        head = buildILNoOp();
    }
    
    return head;
} /* end of generateExpr() */


/* generate ILOC for a binary expression */
struct ILLine *generateBiOpExpr (AstNode *exprNode, struct ILOCSymbol &result)
{
    int kind = exprNode->getType();
    struct ILLine *head = NULL;
    struct ILLine *tail = NULL;
    struct ILLine *code = NULL;
    AstNode *leftSide, *rightSide;
    union Operand leftResult, rightResult, exprResult;
    enum Opcode opCode;
    list<AstNode*>::iterator iter;
    
    /* sanity check while developing */
    if (!exprNode->isBinaryOpNode ())
    {
    	error ("Function generateBiOpExpr() called without valid binary operator.", PROGRAMMER);
    }
        
    /* if caller doesn't have a specific place for result, create a temp */
    if (notSpecified (result))
        nextKernelTemp (result);

    iter = exprNode->begin ();
    leftSide = *iter;
    iter++;
    rightSide = *iter;
    
    tail = head = buildILNoOp();
    
    setUnspecified (leftResult.variable);
    code = generateExpr (leftSide, leftResult.variable);
    tail = addILList (tail, code);
    
    setUnspecified (rightResult.variable);
    code = generateExpr (rightSide, rightResult.variable);
    tail = addILList (tail, code);
    
    switch (kind)
    {
    case LTEEXPR:           opCode = ILOC_CMP_LE;   break;
    case LTEXPR:            opCode = ILOC_CMP_LT;   break;
    case GTEEXPR:           opCode = ILOC_CMP_GE;   break;
    case GTEXPR:            opCode = ILOC_CMP_GT;   break;
    case EQEXPR:            opCode = ILOC_CMP_EQ;   break;
    case NEQEXPR:           opCode = ILOC_CMP_NE;   break;
    case ADDEXPR:           opCode = ILOC_ADD;      break;
    case SUBEXPR:           opCode = ILOC_SUB;      break;
    case MULEXPR:           opCode = ILOC_MULT;     break;
    case DIVEXPR:           opCode = ILOC_DIV;      break;
    case MODEXPR:           opCode = ILOC_MOD;      break;
    case LOGANDEXPR:        opCode = ILOC_AND;      break;  /* TODO ?? */
    case LOGOREXPR:         opCode = ILOC_OR;       break;   /* TODO ?? */
    case BITANDEXPR:        opCode = ILOC_AND;      break;
    case BITOREXPR:         opCode = ILOC_OR;       break;
    case BITXOREXPR:        opCode = ILOC_XOR;      break;
    case SHIFTLEFTEXPR:     opCode = ILOC_LSHIFT;   break;
    case SHIFTRIGHTEXPR:    opCode = ILOC_RSHIFT;   break;
    default:
        printf ("DEBUG: binary operator %s not yet implemented?\n", nodeNames[kind]);
        opCode = ILOC_NOP;
    }
    
    exprResult.variable = result;
    code = buildILLine (opCode, exprResult, leftResult, rightResult);
    tail = addILList (tail, code);
    
    stripHead (head);
    return head;
} /* end of generateBiOpExpr() */


/* generate ILOC for a unary expression */
struct ILLine *generateUnaryExpr (AstNode *exprNode, struct ILOCSymbol &result)
{
    int kind = exprNode->getType();
    struct ILLine *head = NULL;
    struct ILLine *tail = NULL;
    struct ILLine *code = NULL;
    AstNode *subExpr = *(exprNode->begin ());
    union Operand exprResult, subResult, zero;
    
    zero.longValue = 0;
    
    /* generate the sub-expression to operate on */
    head = buildILNoOp ();
    setUnspecified (subResult.variable);
    code = generateExpr (subExpr, subResult.variable);
    addILList (head, code);
    tail = seekILTail (head);

    /* if caller doesn't have a specific place for result, create a temp */
    if (notSpecified (result))
        nextKernelTemp (result);
    exprResult.variable = result;
    
    switch (kind)
    {
    case NEGEXPR:
		/*printf ("DEBUG: generateUnaryExpr(): NEGEXPR on %s\n", symtab->lookup(subResult.variable.identifier));*/
        code = buildILLine3 (ILOC_RSUBI, exprResult, subResult, zero);
        tail = addILList (tail, code);
        break;
    case DEREFEXPR:
        code = buildILLine2 (ILOC_LOAD, exprResult, subResult);
        tail = addILList (tail, code);
        break;
    case NOTEXPR:
        /* TODO - fall through to DEBUG message for now... */
    case ADDRESSOFEXPR:
        /* TODO - fall through to DEBUG message for now... */
    default:
        printf ("DEBUG: unary operator %s not yet implemented.\n", nodeNames[kind]);
        tail = addILList (tail, buildILNoOp());
        code = buildILLine2 (ILOC_I2I, exprResult, subResult);
        tail = addILList (tail, code);
    }
    
    stripHead(head);	/* strip off the placeholder NOP we started with */
    return head;
} /* end of generateUnaryExpr() */


/* end of genExprs.C */
