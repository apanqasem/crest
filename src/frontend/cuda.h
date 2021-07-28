#ifndef MC_H
#define MC_H

/* ##################### */
/* All the Non terminals */
/* ##################### */
#define PROGRAM          128  /* 0x80 */
#define DECL_LIST        129  /* 0x81 */
#define DECL             130  /* 0x82 */
#define VAR_DECL         131  /* 0x83 */
#define TYPE_SPECIFIER   132  /* 0x84 */
#define FUN_START        133  /* 0x85 */
#define FUN_DECL         134  /* 0x86 */
#define FORMAL_DECL_LIST 135  /* 0x87 */
#define FORMAL_DECL      136  /* 0x88 */
#define FUN_BODY         137  /* 0x89 */
#define LOCAL_DECL_LIST  138  /* 0x8A */
#define STATEMENT_LIST   139  /* 0x8B */
#define STATEMENT        140  /* 0x8C */
#define COMPOUND_STMT    141  /* 0x8D */
#define ASSIGN_STMT      142  /* 0x8E */
#define COND_STMT        143  /* 0x8F */
#define LOOP_STMT        144  /* 0x90 */
#define RETURN_STMT      209  /* 0xD1 */ /* 145 | 64 --> makes this an expr node */
#define VAR              146  /* 0x92 */
#define EXPRESSION       147  /* 0x93 */
#define REL_OP           148  /* 0x94 */
#define ADD_EXPR         149  /* 0x95 */
#define ADD_OP           150  /* 0x96 */
#define TERM             151  /* 0x97 */
#define MUL_OP           152  /* 0x98 */
#define FACTOR           153  /* 0x99 */
#define FUN_CALL_EXPR    154  /* 0x9A */
#define ARG_LIST         155  /* 0x9B */
#define RELOP_EXPR       156  /* 0x9C */
#define EMPTY            157  /* 0x9D */
#define ASM_STMT_LIST    158  /* 0x9E */
#define FUN_HEADER       159  /* 0x9F */
/* okay, can't have any more node types, hopefully we won't need them */
/* can only have 1 more node type. Can't go to A on the 
 * second nibble, that would indicate an expr node
 * (could add more nodes if we change the flag for expr node) */

#define TYPE_ERR         -1


#define DWORD            4



/* terminals are < 128,
 * nonterms are between 128 and 255
 * if I want to encode both a node 'name'
 * and a node type in the same int, types are > 256 */

#define EXPRNODE      0x00040  ///< expr node flag

#define DTYPE         0xf0000  ///< data type field
#define DCLASS        0x00f00  ///< data class field
#define NODETYPE      0x000ff  ///< node type field

#define UNDECL        0x00000  ///< symbol::s_type for an undeclared/undefined symbol
#define SCALAR        0x00100    
#define ARR           0x00200
#define ARR_REF       0x00400
#define CONST         0x00800

#define INT           0x10000  ///< integer data
#define INT_SCALAR    0x10100  ///< integer scalar data
#define INT_ARR       0x10200  ///< integer array 
#define INT_ARR_REF   0x10400  ///< integer array reference
#define INT_CONST     0x10800  ///< integer constant data

#define CHAR          0x20000  ///< char data
#define CHAR_SCALAR   0x20100  ///< char scalar data
#define CHAR_ARR      0x20200  ///< char array
#define CHAR_ARR_REF  0x20400  ///< char array ref
#define CHAR_CONST    0x20800  ///< char constant data

#define STRING         0x40000  ///< string data
#define STRING_SCALAR  0x40100  ///< string scalar data
#define STRING_ARR     0x40200  ///< string array
#define STRING_ARR_REF 0x40400 ///< string array ref
#define STRING_CONST   0x40800  ///< string constant data

#define VOID          0x80000  ///< void data
#define VOID_SCALAR   0x80100  ///< void scalar data -- makes no sense, here for completeness
#define VOID_ARR      0x80200  ///< void array data -- makes no sense, here for completeness
#define VOID_ARR_REF  0x80400  ///< void array ref  -- makes no sense
#define VOID_CONST    0x80800  ///< void constant data -- makes no sense, here for completeness??

#define ARRAY_IFY(x)     (((x) & ~(DCLASS)) | ARR)   ///< clears existing DCLASS bits and turns on the ARR bit
#define ARRAY_REF_IFY(x) (((x) & ~(DCLASS)) | ARR_REF) ///< clears existing DCLASS bits and turns on the ARR_REF bit

#define IS_DTYPE(x)         ((x) > NODETYPE) ///< evalutes to true if x is a data-type token
#define GET_STRICT_DTYPE(x) ((x) & DTYPE)    ///< evaluates to x's strict data type (does not include array info)
#define GET_DTYPE(x)        ((x) & (DTYPE | ARR))  ///< evaluates to x's data type && whether it's an array

#define GET_DCLASS(x) ((x) & DCLASS)  ///< evaluates to x's data class (scalar, arr, const)
#define IS_SCALAR(x)  ((x) & SCALAR)  ///< evaluates to true if x is a scalar type
#define IS_ARRAY(x)   ((x) & ARR)   ///< evaluates to true if x is an array type
#define IS_ARR_REF(X) ((x) & ARR_REF) ///< evaluates to true if x is an array ref type
#define IS_CONST(x)   ((x) & CONST)   ///< evaluates to true if x is a constant

#define GET_NODE_TYPE(x) ((x) & NODETYPE)  ///< strips of the data type info and evaluates to just the AST node type
#define IS_EXPR_NODE(x)  ((x) & EXPRNODE)  ///< evaluates to true if x is an expression node (operators and return statements)
#define IS_FNCALL(x)     (((x) & NODETYPE) == FUN_CALL_EXPR) ///< evaluates to true if x is a funCall node



/**
 * gets an array element's offset in mem
 */
#define GET_ARR_OFFSET(x) \
      (x)->an_childList[0]->an_sym->s_offset + (x)->an_childList[1]->an_value


#endif /* ifndef MC_H */
