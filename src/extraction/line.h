/*
    line.h

    header for lines of Intermediate Language
        intended to be compatible with ILOC
        as described in 'Engineering a Compiler'
        by Keith D. Cooper and Linda Torczon
        Appendix A, page 659

    originally for CS 4318 / 5331 Spring 2012
    adapted for CREST Summer 2012

    cs1629@txstate.edu
*/


#ifndef LINE_H
#define LINE_H

#include <stdlib.h>


/* operation codes */
enum Opcode
{
	/* ILOC instructions */
	ILOC_NOP,
	
	/* arithmatic */
	ILOC_ADD, ILOC_ADDI,
	ILOC_SUB, ILOC_SUBI, ILOC_RSUBI,
	ILOC_MULT, ILOC_MULTI,
	ILOC_DIV, ILOC_DIVI, ILOC_RDIVI,
	ILOC_MOD, ILOC_MODI, ILOC_RMODI,
	
	/* shifts */
	ILOC_LSHIFT, ILOC_LSHIFTI,
	ILOC_RSHIFT, ILOC_RSHIFTI,
	
	/* logical */
	ILOC_AND, ILOC_ANDI,
	ILOC_OR, ILOC_ORI,
	ILOC_XOR, ILOC_XORI,
	
	/* memory operations */
	ILOC_LOADI, ILOC_LOAD, ILOC_LOADAI, ILOC_LOADAO,
	ILOC_CLOADI, ILOC_CLOAD, ILOC_CLOADAI, ILOC_CLOADAO,
	ILOC_FLOADI, ILOC_FLOAD, ILOC_FLOADAI, ILOC_FLOADAO,
	ILOC_STORE, ILOC_STOREAI, ILOC_STOREAO,
	ILOC_CSTORE, ILOC_CSTOREAI, ILOC_CSTOREAO,
	ILOC_FSTORE, ILOC_FSTOREAI, ILOC_FSTOREAO,
	
	/* register to register copy operations */
	ILOC_I2I, ILOC_C2C, ILOC_F2F,
	ILOC_C2I, ILOC_I2C,
	ILOC_I2F, ILOC_F2I,
	ILOC_C2F, ILOC_F2C,        /* needed? or just go through I? */
	
	/* control-flow operations */
	ILOC_CMP_LT, ILOC_CMP_LE,
	ILOC_CMP_EQ, ILOC_CMP_NE,
	ILOC_CMP_GT, ILOC_CMP_GE,
	ILOC_CBR,
	ILOC_JUMPI, ILOC_JUMP,
	ILOC_TBL
}; /* end of enum Opcode */


/* encapsulate a Symbol for SSA */
struct ILOCSymbol
{
    int identifier;		/* hash code of identifier */
    int instance;		/* which instance of the value */
    					/* an instance of -1 indicates not (yet) known */
};
typedef struct ILOCSymbol ILOCSymbol;


union Operand
{
    struct ILOCSymbol variable;

    long longValue;             /* immediate values */
    double doubleValue;
};

extern union Operand opDontCare;    /* defined in line.c */


/* encapsulate one line of Intermediate Code */
struct ILLine
{
    struct ILLine *next, *prev;

    enum Opcode operation;

    union Operand dest;
    union Operand source1;
    union Operand source2;
};


/* prototypes */

/* line builders */
struct ILLine *buildILLine (enum Opcode operation, union Operand destination, union Operand sourceA, union Operand sourceB);
#define buildILLine0(opcode)					buildILLine (opcode, opDontCare, opDontCare, opDontCare)
#define buildILLine1(opcode, dest)				buildILLine (opcode, (union Operand)(dest), opDontCare, opDontCare)
#define buildILLine2(opcode, dest, src)			buildILLine (opcode, (union Operand)(dest), (union Operand)(src), opDontCare)
#define buildILLine3(opcode, dest, srcA, srcB)	buildILLine (opcode, (union Operand)(dest), (union Operand)(srcA), (union Operand)(srcB))
#define buildILNoOp()							buildILLine (ILOC_NOP, opDontCare, opDontCare, opDontCare)

/* list maintenance */
struct ILLine *addILList (struct ILLine *alpha, struct ILLine *beta);
void deleteILLine (struct ILLine *doomed);
void removeILLine (struct ILLine *pulled);
void deleteILList (struct ILLine *head);
struct ILLine *seekILTail (struct ILLine *start);

/* display / debugging */
void printILLine (const struct ILLine *display);
void printILList (const struct ILLine *display);


#endif	/* end of line.h */
