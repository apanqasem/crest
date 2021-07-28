#include<stdio.h>

#include<prettyPrint.h>

#include<driver/errors.h>
#include<frontend/symTab.h>


extern char *tokenNames[];
extern char *nodeNames[];
extern SymTab *symtab;


inline void indent(int nestLevel) {
  for (int n = 0; n < nestLevel; n++) 
    for  (int i = 0; i < INDENT; i++)
      printf(" ");
  return;
}  

inline bool isCondOrLoopNode(AstNode *node) {
  return (node->isLoopNode() || node->isCondNode());
}


void printStmtList(AstNode *node, int nestLevel) {

  list<AstNode*>::iterator i;
  list<AstNode*> *children = node->getChildren();

  for(i = children->begin(); i != children->end(); i++) {
    indent(nestLevel);
    prettyPrintAst(*i, nestLevel);
    if (!isCondOrLoopNode(*i))
      printf(";\n");
  }
} 

void printArrayRef(AstNode *node, int nestLevel) {
  if (!node) 
    error("Encountered null node : array ref", FATAL);

  int dims = node->getNumSiblings() - 1;
  int k;
  list<AstNode*>::iterator i;

  for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
    prettyPrintAst(*i, nestLevel);	
    if (k > 0) 
      printf("]");
    if (k < dims)
      printf("[");
  }    
}

void printExprList(AstNode *node, int nestLeve) {
}


void prettyPrintAst(AstNode *node, int nestLevel) {


  int kind = node->getType();

  list<AstNode*>::iterator i;
  list<AstNode*> *children = node->getChildren();
  int childCnt = node->getNumChildren();
  int sibs = node->getNumSiblings(); 

  unsigned int k;
  switch(kind) {
  case DUMMY:
    break;
  case FUNBODY: 
  case STMTLIST:
    printf(" {\n");
    printStmtList(node, nestLevel + 1);
    printf("}\n");
    break;

  case CONDBODY:
  case LOOPBODY:
    if (childCnt == 1)
      printf("\n");
    else
      printf(" {\n");
    printStmtList(node, nestLevel + 1);
    if (childCnt > 1) {
      indent(nestLevel);
      printf("}\n");
    }
    break;

  case DWLOOPBODY:
    printf(" {\n");
    printStmtList(node, nestLevel + 1);
    indent(nestLevel);
    printf ("}");
    break;
    
  case ASSIGNLIST:
  case ARGLIST:
    for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
      prettyPrintAst(*i, nestLevel + 1);	
      if (k  < (sibs - 1)) 
	printf(", ");
    }
    break;
  case PREINCSTMT:
    printf("%s", nodeNames[PREINCSTMT]);
    for(i = node->begin(); i != node->end(); i++)
	  prettyPrintAst(*i, nestLevel);	
    break;
  case PREDECSTMT:
    printf("%s", nodeNames[PREDECSTMT]);
    for(i = node->begin(); i != node->end(); i++)
	  prettyPrintAst(*i, nestLevel);	
    break;
  case POSTINCSTMT:
    for(i = node->begin(); i != node->end(); i++)
	  prettyPrintAst(*i, nestLevel);	
    printf("%s", nodeNames[POSTINCSTMT]);
    break;
  case POSTDECSTMT:
    for(i = node->begin(); i != node->end(); i++)
	  prettyPrintAst(*i, nestLevel);	
    printf("%s", nodeNames[POSTDECSTMT]);
    break;
  case NEGEXPR:
    printf("%s", nodeNames[NEGEXPR]);
    for(i = node->begin(); i != node->end(); i++)
	  prettyPrintAst(*i, nestLevel);	
    break;
  case NOTEXPR:
    printf("%s", nodeNames[NOTEXPR]);
    for(i = node->begin(); i != node->end(); i++)
	  prettyPrintAst(*i, nestLevel);	
    break;
  case DEREFEXPR:
    printf("%s", nodeNames[DEREFEXPR]);
    for(i = node->begin(); i != node->end(); i++)
	  prettyPrintAst(*i, nestLevel);	
    break;
  case ADDRESSOFEXPR:
    printf("%s", nodeNames[ADDRESSOFEXPR]);
    for(i = node->begin(); i != node->end(); i++)
	  prettyPrintAst(*i, nestLevel);	
    break;
  case PARENEXPR:
    printf ("( ");
    prettyPrintAst(*(node->begin()), nestLevel);
    printf (" )");
    break;
  case TYPECASTEXPR:
    printf ("(");
    i = node->begin();
    prettyPrintAst(*i, nestLevel);
    printf (") ");
    for(k = 1, i++; i != node->end(); k++, i++) {
 	  prettyPrintAst(*i, nestLevel);	
	  if (k  < (sibs - 1)) 
	    printf(" ");
    }
    break;
  case CONDEXPR:
    for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
      prettyPrintAst(*i, nestLevel);
      if (k == 0)
        printf (" ? ");
      else if (k == 1)
        printf (" : ");
    }
    break;
  case ASSIGN:
  case ASSIGNADD:
  case ASSIGNSUB:
  case ASSIGNMUL:
  case ASSIGNDIV:
  case ASSIGNMOD:
  case ASSIGNRS:
  case ASSIGNLS:
  case ASSIGNAND:
  case ASSIGNOR:
  case ASSIGNXOR:
  case MULEXPR:
  case DIVEXPR:
  case MODEXPR:
  case ADDEXPR:
  case SUBEXPR:
  case LTEEXPR:
  case LTEXPR:
  case GTEEXPR:
  case GTEXPR:
  case EQEXPR:
  case NEQEXPR:
  case LOGANDEXPR:
  case LOGOREXPR:
  case BITANDEXPR:
  case BITOREXPR:
  case BITXOREXPR:
  case SHIFTLEFTEXPR:
  case SHIFTRIGHTEXPR:
    if (sibs == 1) {
      for(i = node->begin(); i != node->end(); i++)
 	prettyPrintAst(*i, nestLevel);	
    }
    else if (sibs == 2) {
      for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
 	prettyPrintAst(*i, nestLevel);	
	if (k  < ((sibs) - 1)) 
	  printf(" %s ", nodeNames[node->getType()]);
      }
    } 
    else 
      error("Tree build error : expr", FATAL);
    break;
    
  case FUNCALL:
    for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
      prettyPrintAst(*i, nestLevel);	
      if (k == 0) 
	printf("(");
    }
    printf(")");
    break;
  /*case KERNELCALL:
    for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
      prettyPrintAst(*i, nestLevel);	
      if (k == 0) 
	    printf("<");
      if (k == 1) 
	    printf(">(");
    }
    printf(")");
    break;*/
  case STATICNODE:
    printf("static ");
    for(i = node->begin(); i != node->end(); i++) {
      prettyPrintAst(*i, nestLevel);	
    }
    break; 
  case PTRNODE:
    for(i = node->begin(); i != node->end(); i++) {
      prettyPrintAst(*i, nestLevel);	
    }
    printf("*");
    break; 
  case QUAL:
  case TYPE:
    if (!node->isLeafNode())
      error("Tree build error : type", FATAL);
    else {
      printf("%s", tokenNames[node->getVal()]);		/* by keyword */
      
      /* multi-keyword type spec? */
      if (node->getNumSiblings() > 0) {
        for(i = node->begin(); i != node->end(); i++) {
          printf (" ");
          prettyPrintAst(*i, nestLevel);
          /*printf(" %s", tokenNames[(*i)->getVal()]);*/
        }
      }
    } /* end outer if / else */
    break;
  case IDTYPE:
    if (!node->isLeafNode())
      error("Tree build error : type", FATAL);
    else {
        for(i = node->begin(); i != node->end(); i++) {
		  prettyPrintAst(*i, nestLevel);
        }
    } /* end outer if / else */
    break;

  case ID_REF:
  case SCALAR_REF:
        //if (kind == ID_REF) printf ("IDR");   /* debugging */
        //if (kind == SCALAR_REF) printf ("SR");   /* debugging */
    if (!node->isLeafNode())
      error("Tree build error : id", FATAL);
    else {
      printf("%s", symtab->lookup(node->getVal()));
    }
    break;
  case STRUCT_REF:
    //    for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
    prettyPrintAst((*node)[1], nestLevel);	
    //    }
    break;
  case INT_REF:
  case CHAR_REF:
    if (!node->isLeafNode())
      error("Tree build error : char", FATAL);
    else {
      printf("%d", node->getVal());
    }
    break;
  case FLOAT_REF:
    if (!node->isLeafNode())
      error("Tree build error : float", FATAL);
    else {
        /* often "prettier" to use %g rather than %f, but 255.0f != 255 in C syntax... */
      printf("%f", node->getFVal());
    }
    break;
  case FORMALLIST:
    printf("(");
    for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
      prettyPrintAst(*i, nestLevel);	
      if (k  < ((sibs) - 1)) 
	    printf(", ");
    }
    printf(")");
    break; 
  case VARDECLLIST:
    for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
      prettyPrintAst(*i, nestLevel);	
      if (k  < ((sibs) - 1)) 
	    printf(", ");
    }
    break; 
  case VARDECL:
  case FORMALDECL:
        //printf ("VD/FD");   /* debugging */
    if (sibs == 2 || sibs == 3) {
      for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
 	    prettyPrintAst(*i, nestLevel);	
	    if (k  < (sibs - 1)) 
	      printf(" ");
      }
    }
    else {
      error("Tree build error : formal decl", FATAL);
    }
    break;
  case VARDECLASSIGN:
        //printf ("VDA");   /* debugging */
    if (sibs == 3) {
      for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
	    prettyPrintAst(*i, nestLevel);	
	    if (k == 1)
	      printf(" =");
        if (k  < ((sibs) - 1)) 
	      printf(" ");
      }
    }
    if (sibs == 4) {
      for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
 	    prettyPrintAst(*i, nestLevel);	
	    if (k == 2)
	      printf(" =");
	    printf(" ");
      }
    }
    break;
  case EXTERNDECL:
    printf ("extern ");
    for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
      prettyPrintAst(*i, nestLevel);
      if (k  < (sibs - 1))
        printf(" ");
    }
    break;
  case EXTERNARRDECL:
    printf ("extern ");
    for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
      prettyPrintAst(*i, nestLevel);
      if (k  < (sibs - 1))
        printf(" ");
      else
        printf ("[]");
    }
    break;
  case ARRAY_REF: 
    printArrayRef(node, nestLevel);
    break;
  case ARRDECL:
    for(k = 0, i = node->begin(); k < (sibs - 1) && i != node->end(); k++, i++) {
      prettyPrintAst(*i, nestLevel);	
      printf(" ");
    }
    prettyPrintAst(*i, nestLevel);	
    break;
  case ARRDECLASSIGN:
    for(k = 0, i = node->begin(); k < (sibs - 1) && i != node->end(); k++, i++) {
      prettyPrintAst(*i, nestLevel);	
      printf(" ");
    }
    printf ("= ");
    prettyPrintAst(*i, nestLevel);	
    break;
  case ARRAY_INITLIST:
    printf ("{");
    for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
 	  prettyPrintAst(*i, nestLevel);
    }
    printf ("}");
    break;
  case ARRAY_INIT:
    for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
 	  prettyPrintAst(*i, nestLevel);
	  if (k  < (sibs - 1)) 
	    printf(", ");
    }
    break;
  case RET:
    /*indent(nestLevel);*/
    printf ("return");
    if (sibs > 0)
      printf(" ");
    for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
 	  prettyPrintAst(*i, nestLevel);	
	  if (k  < (sibs - 1)) 
	    printf(" ");
    }
    /*printf (";\n");*/
    break;
  case COND:
    printf ("if (");
    for(i = node->begin(); i != node->end(); i++)
      prettyPrintAst(*i, nestLevel);
    printf(")");
    i = children->begin();
    prettyPrintAst(*i, nestLevel);
    if (childCnt == 2)
    {
      indent(nestLevel);
      printf ("else");
      ++i;
      prettyPrintAst(*i, nestLevel);
    }
    break;
  case LOOP: 
    if (sibs == 2) {  /* one or two siblings for while loop nodes? Two; an expression and a body. */
      printf("while (");
      /*for(i = node->begin(); i != node->end(); i++) */
      i = node->begin();  /* expression */
      prettyPrintAst(*i, nestLevel);
      printf(")");
      ++i;  /* body */
      prettyPrintAst(*i, nestLevel);
    }
    else if (sibs == 3 || sibs == 4 || sibs == 5) {
      printf("for (");
      for(k = 0, i = node->begin(); k < sibs - 1 && i != node->end(); k++, i++) {
	prettyPrintAst(*i, nestLevel);	
	if (k  < (sibs - 2)) 
	  printf("; ");
      }
      printf(")");
     
      prettyPrintAst(*i, nestLevel);	
    }
    else 
      error("Tree build error : loop", FATAL);
    break;
  case DWLOOP:      /* do-while loops */
    printf ("do");
    i = node->begin();      /* body */
    prettyPrintAst(*i, nestLevel);
    ++i;
    printf (" while (");
    prettyPrintAst(*i, nestLevel);
    printf (");\n");
    break;

  default:
    indent(nestLevel);
    for(k = 0, i = node->begin(); i != node->end(); k++, i++) {
 	prettyPrintAst(*i, nestLevel);	
	if (k  < (sibs - 1)) 
	  printf(" ");
    }
    if (node->isStmtNode()) {
      printf(";\n");
    }
  }
    
}

void prettyPrintSubtree(AstNode *node, int nestLevel) {
  AstNode *parent = node->getParent();
  if (!parent)
    return;
  
  for (int i = node->getChildNum() + 1; i < parent->getNumChildren(); i++) {
    AstNode *child = parent->getChild(i);

    indent(nestLevel);
    prettyPrintAst(child, nestLevel);
    printf(";\n");
  }
  
  prettyPrintSubtree(parent, nestLevel);
}
