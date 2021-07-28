/*
	line.c
	
	code for maintaining lines and lists of Intermediate Language
	
	originally for CS 4318 / 5331 Spring 2012
	adapted for CREST Summer 2012
    
    cs1629@txstate.edu
*/


#include <stdio.h>
#include "extraction/line.h"
#include "driver/errors.h"
#include<frontend/symTab.h>     /* for displaying names */
extern SymTab *symtab;


union Operand opDontCare = {0};


/* build a line of IL (all fields) */
struct ILLine *buildILLine (enum Opcode operation, union Operand destination, union Operand sourceA, union Operand sourceB)
{
    struct ILLine *line;

    line = (struct ILLine *)malloc (sizeof (struct ILLine));

    if (line == NULL)
        error ("out of memory", MEMORY);

    line->next = NULL;
    line->prev = NULL;
    line->operation = operation;
    line->dest = destination;
    line->source1 = sourceA;
    line->source2 = sourceB;

    return line;
}


/*
    appends the list starting with beta after node alpha
        anything that was after alpha will be after the end of list beta
    returns pointer to last node added / inserted
        (if alpha was an insertion point instead of a tail
            this allows of continued insertions)
*/
struct ILLine *addILList (struct ILLine *alpha, struct ILLine *beta)
{
    if (alpha == NULL)
        return seekILTail (beta);

    if (beta == NULL)
        return alpha;

    /* gamma is sub-list (possibly null) that followed alpha */
    struct ILLine *gamma = alpha->next;

    /* append list beta after node alpha */
    alpha->next = beta;
    beta->prev = alpha;

    /* seek until end of list beta */
    while (beta->next != NULL)
        beta = beta->next;

    /* anything that was after alpha now goes after end of beta */
    if (gamma != NULL)
    {
        /* beta's tail <-> gamma's head*/
        beta->next = gamma;
        gamma->prev = beta;
    }

    return beta;
}


/* delete a line */
void deleteILLine (struct ILLine *doomed)
{
    if (doomed == NULL)
        error ("unable to delete null pointer", PROGRAMMER);

    free (doomed);
}


/* remove line from list */
void removeILLine (struct ILLine *pulled)
{
    if (pulled == NULL)
        error ("unable to remove null pointer", PROGRAMMER);

    if (pulled->prev != NULL)
        pulled->prev->next = pulled->next;

    if (pulled->next != NULL)
        pulled->next->prev = pulled->prev;
}


/* delete list starting from given node */
void deleteILList (struct ILLine *head)
{
    struct ILLine *curr;

    while (head != NULL)
    {
        curr = head->next;

        deleteILLine (head);

        head = curr;
    }
}


struct ILLine *seekILTail (struct ILLine *start)
{
    struct ILLine *current = start;

    if (start == NULL)
        return NULL;

    while (current->next != NULL)
        current = current->next;

    return current;
}


void printILLine (const struct ILLine *display)
{
    if (display == NULL)
        return;

    /*printf ("%p  -->  %p  -->  %p\n", display->prev, display, display->next);	/* DEBUG */

#define ID              variable.identifier
#define IDENT(foo)      (display->foo.variable.identifier)
#define INSTANCE(foo)   (display->foo.variable.instance)
#define NAME(foo)       (symtab->lookup(display->foo.variable.identifier))

    switch (display->operation)
    {
    /* ILOC instructions */
    case ILOC_NOP:
        puts ("\t nop");
        break;

    /* arithmatic */
    case ILOC_ADD:
        //printf ("\t %d.%d <- %d + %d\n", IDENT(dest), INSTANCE(dest), IDENT(source1), IDENT(source2));
        printf ("\t %s.%d <- %s.%d + %s.%d\n", NAME(dest), INSTANCE(dest), NAME(source1), INSTANCE(source1), NAME(source2), INSTANCE(source2));
        break;
    case ILOC_ADDI:
        printf ("\t %s.%d <- %s.%d + ", NAME(dest), INSTANCE(dest), NAME(source1), INSTANCE(source1), display->source1.ID);
        if (display->source2.longValue >= 0)
            printf ("%ld\n", display->source2.longValue);
        else
            printf ("(%ld)\n", display->source2.longValue);
        break;
    case ILOC_SUB:
        printf ("\t %s.%d <- %s.%d - %s.%d\n", NAME(dest), INSTANCE(dest), NAME(source1), INSTANCE(source1), NAME(source2), INSTANCE(source2));
        break;
    case ILOC_SUBI:
        printf ("\t %s.%d <- %s.%d - ", NAME(dest), INSTANCE(dest), NAME(source1), INSTANCE(source1), display->source1.ID);
        if (display->source2.longValue >= 0)
            printf ("%ld\n", display->source2.longValue);
        else
            printf ("(%ld)\n", display->source2.longValue);
        break;
    case ILOC_RSUBI:
        printf ("\t %s.%d <- ", NAME(dest), INSTANCE(dest));
        if (display->source2.longValue >= 0)
            printf ("%ld", display->source2.longValue);
        else
            printf ("(%ld)", display->source2.longValue);
        printf (" - %s.%d\n", NAME(source1), INSTANCE(source1));
        break;
    case ILOC_MULT:
        printf ("\t %s.%d <- %s.%d * %s.%d\n", NAME(dest), INSTANCE(dest), NAME(source1), INSTANCE(source1), NAME(source2), INSTANCE(source2));
        break;
    case ILOC_MULTI:
        printf ("\t %d <- %d * %ld\n", display->dest.variable.identifier, display->source1.variable.identifier, display->source2.longValue);
        break;
    case ILOC_DIV:
        printf ("\t %s.%d <- %s.%d / %s.%d\n", NAME(dest), INSTANCE(dest), NAME(source1), INSTANCE(source1), NAME(source2), INSTANCE(source2));
        break;
    case ILOC_DIVI:
        printf ("\t %d <- %d / %ld\n", display->dest.variable.identifier, display->source1.variable.identifier, display->source2.longValue);
        break;
    case ILOC_RDIVI:
        printf ("\t %d <- %ld / %d\n", display->dest.variable.identifier, display->source2.longValue, display->source1.variable.identifier);
        break;
    case ILOC_MOD:
        printf ("\t %s.%d <- %s.%d %% %s.%d\n", NAME(dest), INSTANCE(dest), NAME(source1), INSTANCE(source1), NAME(source2), INSTANCE(source2));
        break;
    case ILOC_MODI:
        printf ("\t %d <- %d %% %ld\n", display->dest.variable.identifier, display->source1.variable.identifier, display->source2.longValue);
        break;
    case ILOC_RMODI:
        printf ("\t %d <- %ld %% %d\n", display->dest.variable.identifier, display->source2.longValue, display->source1.variable.identifier);
        break;

    /* shifts */
    case ILOC_LSHIFT:
        printf ("\t %d << %d => %d", display->source1.variable.identifier, display->source2.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_LSHIFTI:
        printf ("\t %d << %ld => %d", display->source1.variable.identifier, display->source2.longValue, display->dest.variable.identifier);
        break;
    case ILOC_RSHIFT:
        printf ("\t %d >> %d => %d", display->source1.variable.identifier, display->source2.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_RSHIFTI:
        printf ("\t %d >> %ld => %d", display->source1.variable.identifier, display->source2.longValue, display->dest.variable.identifier);
        break;

    /* logical */
    case ILOC_AND:
        printf ("\t %d and %d => %d", display->source1.variable.identifier, display->source2.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_ANDI:
        printf ("\t %d and %ld => %d", display->source1.variable.identifier, display->source2.longValue, display->dest.variable.identifier);
        break;
    case ILOC_OR:
        printf ("\t %d or %d => %d", display->source1.variable.identifier, display->source2.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_ORI:
        printf ("\t %d or %ld => %d", display->source1.variable.identifier, display->source2.longValue, display->dest.variable.identifier);
        break;
    case ILOC_XOR:
        printf ("\t %d xor %d => %d", display->source1.variable.identifier, display->source2.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_XORI:
        printf ("\t %d xor %ld => %d", display->source1.variable.identifier, display->source2.longValue, display->dest.variable.identifier);
        break;

    /* memory operations */
    case ILOC_LOADI:
        printf ("\t %s.%d <- immediate value %ld\n", NAME(dest), INSTANCE(dest), display->source1.longValue);
        break;
    case ILOC_LOAD:
        printf ("\t load %s.%d from Memory(%s.%d)\n", NAME(dest), INSTANCE(dest), NAME(source1), INSTANCE(source1));
        break;
    case ILOC_LOADAI:
        printf ("\t load %d from Memory(%d + %ld)\n", display->dest.variable.identifier, display->source1.variable.identifier, display->source2.longValue);
        break;
    case ILOC_LOADAO:
        printf ("\t load %d from Memory(%d + %d)\n", display->dest.variable.identifier, display->source1.variable.identifier, display->source2.variable.identifier);
        break;
    case ILOC_CLOADI:
        printf ("\t load %d with character value %ld\n", display->dest.variable.identifier, display->source1.longValue);
        break;
    case ILOC_CLOAD:
        printf ("\t load a char to %d from Memory(%d)\n", display->dest.variable.identifier, display->source1.variable.identifier);
        break;
    case ILOC_CLOADAI:
        printf ("\t load a char to %d from Memory(%d + %ld)\n", display->dest.variable.identifier, display->source1.variable.identifier, display->source2.longValue);
        break;
    case ILOC_CLOADAO:
        printf ("\t load a char to %d from Memory(%d + %d)\n", display->dest.variable.identifier, display->source1.variable.identifier, display->source2.variable.identifier);
        break;
    case ILOC_FLOADI:
        printf ("\t %s.%d <- immediate value %lf\n", NAME(dest), INSTANCE(dest), display->source1.doubleValue);
        break;
    case ILOC_FLOAD:
        printf ("\t load %d from Memory(%d)\n", display->dest.variable.identifier, display->source1.variable.identifier);
        break;
    case ILOC_FLOADAI:
        printf ("\t load %d from Memory(%d + %ld)\n", display->dest.variable.identifier, display->source1.variable.identifier, display->source2.longValue);
        break;
    case ILOC_FLOADAO:
        printf ("\t load %d from Memory(%d + %d)\n", display->dest.variable.identifier, display->source1.variable.identifier, display->source2.variable.identifier);
        break;
    case ILOC_STORE:
        printf ("\t store %d to Memory(%d)\n", display->dest.variable.identifier, display->source1.variable.identifier);
        break;
    case ILOC_STOREAI:
        printf ("\t store %d to Memory(%d + %ld)\n", display->dest.variable.identifier, display->source1.variable.identifier, display->source2.longValue);
        break;
    case ILOC_STOREAO:
        printf ("\t store %d to Memory(%d + %d)\n", display->dest.variable.identifier, display->source1.variable.identifier, display->source2.variable.identifier);
        break;
    case ILOC_CSTORE:
        printf ("\t store char in %d to Memory(%d)\n", display->dest.variable.identifier, display->source1.variable.identifier);
        break;
    case ILOC_CSTOREAI:
        printf ("\t store char in %d to Memory(%d + %ld)\n", display->dest.variable.identifier, display->source1.variable.identifier, display->source2.longValue);
        break;
    case ILOC_CSTOREAO:
        printf ("\t store char in %d to Memory(%d + %d)\n", display->dest.variable.identifier, display->source1.variable.identifier, display->source2.variable.identifier);
        break;

    /* register to register copy operations */
    case ILOC_I2I:
        printf ("\t %s.%d <- %s.%d\t// (integer)\n", NAME(dest), INSTANCE(dest), NAME(source1), INSTANCE(source1));
        //printf ("\t (%d).%d <- %d.%d\t// (integer)\n", IDENT(dest), INSTANCE(dest), IDENT(source1), INSTANCE(source1));
        break;
    case ILOC_C2C:
        printf ("\t %d => %d\t// (character)\n", display->source1.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_F2F:
        printf ("\t %d => %d\t// (float)\n", display->source1.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_C2I:
        printf ("\t %d => %d\t// (character to integer)\n", display->source1.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_I2C:
        printf ("\t %d => %d\t// (integer to character)\n", display->source1.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_I2F:
        printf ("\t %d => %d\t// (integer to float)\n", display->source1.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_F2I:
        printf ("\t %d => %d\t// (float to integer)\n", display->source1.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_C2F:
        printf ("\t %d => %d\t// (character to float)\n", display->source1.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_F2C:
        printf ("\t %d => %d\t// (float to character)\n", display->source1.variable.identifier, display->dest.variable.identifier);
        break;

    /* control-flow operations */
    case ILOC_CMP_LT:
        printf ("\t %d < %d => true -> %d, else false ->%d\n", display->source1.variable.identifier, display->source2.variable.identifier, display->dest.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_CMP_LE:
        printf ("\t %d <= %d => true -> %d, else false ->%d\n", display->source1.variable.identifier, display->source2.variable.identifier, display->dest.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_CMP_EQ:
        printf ("\t %d == %d => true -> %d, else false ->%d\n", display->source1.variable.identifier, display->source2.variable.identifier, display->dest.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_CMP_NE:
        printf ("\t %d != %d => true -> %d, else false ->%d\n", display->source1.variable.identifier, display->source2.variable.identifier, display->dest.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_CMP_GT:
        printf ("\t %d > %d => true -> %d, else false ->%d\n", display->source1.variable.identifier, display->source2.variable.identifier, display->dest.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_CMP_GE:
        printf ("\t %d >= %d => true -> %d, else false ->%d\n", display->source1.variable.identifier, display->source2.variable.identifier, display->dest.variable.identifier, display->dest.variable.identifier);
        break;
    case ILOC_CBR:
        printf ("\t %d == true  => %d -> PC\n", display->source1.variable.identifier, display->source1.variable.identifier);
        printf ("\t %d == false => %d -> PC\n", display->source1.variable.identifier, display->source2.variable.identifier);
        break;
    case ILOC_JUMPI:
        printf ("\t %d -> PC\n", display->dest.variable.identifier);
        break;
    case ILOC_JUMP:
        printf ("\t %d -> PC\n", display->dest.variable.identifier);
        break;
    case ILOC_TBL:
        printf ("\t %d may hold %d", display->source1.variable.identifier, display->source2.variable.identifier);
        break;

    default:
        printf ("DEBUG: Unexpected OpCode: %d... %d  %d  %d\n", display->operation, display->dest.variable.identifier, display->source1.variable.identifier, display->source2.variable.identifier);
    }
}


void printILList (const struct ILLine *display)
{
    int lineno = 1;

    while (display)
    {
        printf ("%03d: ", lineno);
        printILLine (display);
        display = display->next;
        ++lineno;
    }
}


/* end of line.c */

