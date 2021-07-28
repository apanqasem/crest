/*
   parser for extracted cuda kernel

   @author  Apan Qasem
   @date: June 07 2011
*/

%{
   #include<unistd.h>
   #include<stdio.h>
   #include<stdlib.h>

   #include<symTab.h>
   #include<ast/ast.h>
   #include<driver/errors.h>

   /* imports from flex-generated scanner */
   extern int yylineno;
   /*extern int yycolno;*/
   extern int yylex(void);          /* needed for cuda.tab.c */

   extern SymTab *symtab;

   extern const char *nodeNames[];	/* for debugging print outs */

   AstNode *ast = NULL;

   #define MAXVARLENGTH 100
   int errors = 0;

   /* prints an error message with line number to stdout */
   void yyerror(char *S) {
          fprintf(stderr, "line %d: %s\n", yylineno, S);
          ++errors;
   }

%}


%union {
  AstNode *astnode;
  AstLeafNode *astleafnode;
  int num;
  double fnum;
}


/* ##################### */
/*     all tokens        */
/* ##################### */

/* token order must match with tokenNames[] in astdefs.h */

/* major stuff */
%token <num> ID         1
%token INTCONST   2
       CHARCONST  3
       STRCONST   4

/* keywords */
       KWD_IF     5
       KWD_ELSE   6
       KWD_WHILE  7
       KWD_FOR    8
       KWD_INT    9
       KWD_CHAR   10
       KWD_FLOAT  11
       KWD_DOUBLE 12
       KWD_VOID  13
       KWD_STRING 14
       KWD_RETURN 15
       KWD_SWITCH 16
       KWD_CASE   17
       KWD_DEFAULT 18
       KWD_BOOL   19
       KWD___global__ 20
       KWD___shared__ 21
       KWD___device__ 22

/* operators */
       OPERADD    23    /* operator bit set (0x40) */
       OPERSUB    24    /* operator bit set (0x40) */
       OPERMUL    25    /* operator bit set (0x40) */
       OPERDIV    26    /* operator bit set (0x40) */
       OPERLT     27
       OPERGT     28
       OPERGTE    29
       OPERLTE    30
       OPEREQ     31
       OPERNEQ    32
       OPERASN    33
       OPERADDASN 34
       OPERSUBASN 35    /* -= subtract assign */
       OPERRSASN  36	/* >>= right shift assignment */
       OPERPREINC 37
       OPEROR     38
       OPERAND    39
       OPERNOT    40
       OPERCARAT  41
       OPERCOMP   42
       OPERREF    43

/* brackets & parens */
       LSQBRKT    44
       RSQBRKT    45
       LCRLYBRKT  46
       RCRLYBRKT  47
       LPAREN     48
       RPAREN     49

/* punctuation */
       DOT        50
       COLON      51
       COMMA      52
       SEMICLN    53

/* errors */
       INVTOK     54
       UNTCMT     55
       UNTSTR     56

/* new keyword */
       KWD_ASM    57
       PREPROC    58

       KWD_STATIC 59
       KWD_CONST  60

/* bitwise, and bit-shift operators */
       OPERBITOR      61
       OPERSHIFTLEFT  62
       OPERSHIFTRIGHT 63
       
       KWD_SHORT      64
       KWD_UNSIGNED   65
       OPERDEC		  66	/* decrement */
       FLOATCONST     67    /* floating point constant */
       KWD_VOLATILE   68
       OPERMOD        69    /* modulus */
       KWD_INLINE     70	/* not yet supported by parser */
       KWD_REGISTER   71	/* not yet supported by parser */
       KWD_EXTERN     72	/* not yet supported by parser */
       OPERLSASN      73	/* <<= left shift assignment */
       OPERMULASN     74	/* *= multiply and assign */
       OPERDIVASN     75	/* /= divide and assign */
       OPERMODASN     76	/* %= modulus and assign */
       OPERANDASN     77	/* &= and and assign */
       OPERORASN      78	/* |= or and assign */
       OPERXORASN     79	/* ^= xor and assign */
       KWD_LONG       80
       KWD_DO         81    /* for do-while loops */
       KWD_LONG_LONG  82    /* synthesized "keyword" */

%type <astleafnode> id funid
%type <astnode> kernel funDecl funHeader  funStart formalDeclList formalDecl funBody
%type <astnode> statement statementList
%type <astnode> varDecl varDeclList externDecl
%type <astnode> assignStmt condStmt loopStmt returnStmt
%type <astnode> compoundstmt assignNoSemi assignop /*assignStmtChain*/ assignStmtList
%type <astnode> funTypeSpecifier ptrTypeSpecifier typeSpecifier builtinTypeSpec integerTypeSpec
%type <astnode> exp condExpr var logExpr bitExpr relExpr shiftExpr addExpr term factor varExpr
%type <astnode> funCallExpr /*kernelCallExpr*/ argList
%type <astnode> arrayRef arrayInitializer arrayInitList
%type <astnode> mulop addop relop logop bitop shiftop

/* We know about the dangling else. */
%expect 1


%start kernel

%%


kernel : funDecl {
        /*printf ("kernel parsed.\n"); /* debugging */
        ast = $1;
    }
    ;

funDecl : funHeader funBody {
        AstNode *kernel = new AstNode(KERNEL);
	    kernel->addSibling($1);
	    kernel->addSibling($2);
	    $$ = kernel;
    }
    ;

funHeader : funStart formalDeclList RPAREN {
       AstNode *funheaderNode = new AstNode(FUNHEAD);
       AstNode *formalListNode;

       funheaderNode->addSibling($1);
       if ($2->isFormalDeclNode()) {
	     formalListNode = new AstNode(FORMALLIST);
	     formalListNode->addSibling($2);
	   }
	   else {
         formalListNode = $2;
       }
       funheaderNode->addSibling(formalListNode);
       $$ = funheaderNode;
    }
    | funStart RPAREN {
       AstNode *funheaderNode = new AstNode(FUNHEAD);
       funheaderNode->addSibling($1);
       
       /* add an empty formal list (so that prettyPrint dosen't skip parens) */
       AstNode *formalListNode = new AstNode(FORMALLIST);
       funheaderNode->addSibling(formalListNode);
       
       $$ = funheaderNode;
    }
    ;

funStart : funTypeSpecifier id LPAREN {
       AstNode *funNameTypeNode = new AstNode(FUNNAMETYPE);
       funNameTypeNode->addSibling($1);
       funNameTypeNode->addSibling($2);
       $$ = funNameTypeNode;
    }
    | KWD___global__ funTypeSpecifier id LPAREN {
       AstNode *funNameTypeNode = new AstNode(FUNNAMETYPE);
       AstLeafNode *globalNode = new AstLeafNode(QUAL, KWD___global__);
       funNameTypeNode->addSibling(globalNode);
       funNameTypeNode->addSibling($2);
       funNameTypeNode->addSibling($3);
       $$ = funNameTypeNode;
    }
    | KWD___device__ funTypeSpecifier id LPAREN {
       AstNode *funNameTypeNode = new AstNode(FUNNAMETYPE);
       AstLeafNode *deviceNode = new AstLeafNode(QUAL, KWD___device__);
       funNameTypeNode->addSibling(deviceNode);
       funNameTypeNode->addSibling($2);
       funNameTypeNode->addSibling($3);
       $$ = funNameTypeNode;
    }
    ;

formalDeclList : formalDecl  {
       $$ = $1;
    }
    | formalDeclList COMMA formalDecl {
	AstNode* firstNode = $1;
	AstNode *formalListNode;
	if (firstNode->isFormalDeclNode()) {
	    formalListNode = new AstNode(FORMALLIST);
	    formalListNode->addSibling($1);
	    formalListNode->addSibling($3);
	}
	else if (firstNode->isFormalListNode()) {
	    formalListNode = firstNode;
	    formalListNode->addSibling($3);
	}
	else
	  error("Tree build error : formaldecl list", FATAL);
	$$ = formalListNode;
    }
    ;

formalDecl : typeSpecifier id  {
       AstNode *formalDecl = new AstNode(FORMALDECL);
       formalDecl->addSibling($1);
       formalDecl->addSibling($2);
       $$ = formalDecl;
    }
    | typeSpecifier OPERREF id  {
       AstNode *formalDecl = new AstNode(FORMALDECL);
       AstLeafNode *byRef = new AstLeafNode (TYPE, OPERREF);
       $1->addSibling(byRef);
       formalDecl->addSibling($1);
       formalDecl->addSibling($3);
       $$ = formalDecl;
    }
    | ptrTypeSpecifier id {
       AstNode *formalDecl = new AstNode(FORMALDECL);
       formalDecl->addSibling($1);
       formalDecl->addSibling($2);
       $$ = formalDecl;
    }
    | typeSpecifier id LSQBRKT RSQBRKT {
       AstNode *formalDecl = new AstNode(FORMALDECL);
       formalDecl->addSibling($1);
       formalDecl->addSibling($2);
       $$ = formalDecl;
    }
    ;

funBody : LCRLYBRKT statementList RCRLYBRKT {
       /*AstNode *funBodyNode = $2;
       funBodyNode->setType(FUNBODY);
       $$ = funBodyNode;*/
       AstNode *funBodyNode;
       if ($2->isStmtListNode ())
       {
           funBodyNode = $2;
           funBodyNode->setType (FUNBODY);
       }
       else if ($2->isStmtNode ())
       {
           funBodyNode = new AstNode (FUNBODY);
           funBodyNode->addChild ($2);
       }
       else
       {
           fprintf (stderr, "statement or statement list node expected (found '%s')\n", nodeNames[$2->getType()]);
           error("Tree build error: funBody", FATAL);
       }
       $$ = funBodyNode;
    }
    ;

statementList : statement {
	$$ = $1;
    }
    | statementList statement {
	AstNode* firstNode = $1;
	AstNode *stmtListNode;
	if (firstNode->isStmtNode()) {
	    stmtListNode = new AstNode(STMTLIST);
	    stmtListNode->addChild($1);
	    stmtListNode->addChild($2);

	}
	else if (firstNode->isStmtListNode()) {
	    stmtListNode = firstNode;
	    stmtListNode->addChild($2);
	}
	else {
	  fprintf (stderr, "statement node expected.  found: %s\n", nodeNames[firstNode->getType()]);
	  error("Tree build error : stmt list", FATAL);
    }
	$$ = stmtListNode;
    }
    ;

statement : compoundstmt {
       $$ = $1;
    }
    | varDecl {
       $$ = $1;
    }
    | externDecl {
       $$ = $1;
    }
    | assignStmt {
       $$ = $1;
    }
    /*| assignStmtChain {
        $$ = $1;
    }*/
    | condStmt {
       $$ = $1;
    }
    | loopStmt {
       $$ = $1;
    }
    | returnStmt {
       $$ = $1;
    }
    ;

compoundstmt : LCRLYBRKT statementList RCRLYBRKT {
       $$ = $2;
    }
    ;


arrayRef :  id LSQBRKT addExpr RSQBRKT {
        AstNode *arrayRefNode = new AstNode(ARRAY_REF);
	arrayRefNode->addSibling($1);
	arrayRefNode->addSibling($3);
	$$ = arrayRefNode;
    }
    | id LSQBRKT addExpr RSQBRKT LSQBRKT addExpr RSQBRKT {
        AstNode *arrayRefNode = new AstNode(ARRAY_REF);
	arrayRefNode->addSibling($1);
	arrayRefNode->addSibling($3);
	arrayRefNode->addSibling($6);
	$$ = arrayRefNode;
    }
    | id LSQBRKT addExpr RSQBRKT LSQBRKT addExpr RSQBRKT LSQBRKT addExpr RSQBRKT {
        AstNode *arrayRefNode = new AstNode(ARRAY_REF);
	arrayRefNode->addSibling($1);
        arrayRefNode->addSibling($3);
        arrayRefNode->addSibling($6);
        arrayRefNode->addSibling($6);
        arrayRefNode->addSibling($9);
	$$ = arrayRefNode;
    }
    ;


arrayInitializer : LCRLYBRKT arrayInitList RCRLYBRKT {
        AstNode *arrayInitNode = new AstNode(ARRAY_INITLIST);
        arrayInitNode->addSibling ($2);
        $$ = arrayInitNode;
    }
    ;
    
arrayInitList : {
        /* make an empty node as placeholder */
        AstNode *arrayInitListNode = new AstNode(ARRAY_INIT);
        $$ = arrayInitListNode;
    }
    | exp {
        AstNode *arrayInitListNode = new AstNode(ARRAY_INIT);
        arrayInitListNode->addSibling ($1);
        $$ = arrayInitListNode;
    }
    | exp COMMA arrayInitList {
        AstNode *arrayInitListNode = new AstNode(ARRAY_INIT);
        arrayInitListNode->addSibling ($1);
        arrayInitListNode->addSibling ($3);
        $$ = arrayInitListNode;
    }
    ;

varDecl : typeSpecifier arrayRef  SEMICLN {
        AstNode *varDecl = new AstNode(ARRDECL);
        varDecl->addSibling($1);
        varDecl->addSibling($2);
        $$ = varDecl;
    }
    | typeSpecifier arrayRef  OPERASN arrayInitializer SEMICLN {
        AstNode *varDecl = new AstNode(ARRDECLASSIGN);
        varDecl->addSibling($1);
        varDecl->addSibling($2);
        varDecl->addSibling($4);
        $$ = varDecl;
    }
    /*| typeSpecifier var OPERASN  exp  SEMICLN {*/
	| typeSpecifier id OPERASN  exp  SEMICLN {
        //fprintf (stderr, "varDeclAssign: typeSpec id = exp ;\n"); /* debugging */
        AstNode *varDecl = new AstNode(VARDECLASSIGN);
        varDecl->addSibling($1);
        varDecl->addSibling($2);
        varDecl->addSibling($4);
        $$ = varDecl;
    }
    /*| typeSpecifier id SEMICLN {
        //fprintf (stderr, "varDecl: typeSpec id ;\n");   // debugging
       AstNode *varDecl = new AstNode(VARDECL);
       varDecl->addSibling($1);
       varDecl->addSibling($2);
       $$ = varDecl;
    }*/
    | KWD___shared__ typeSpecifier arrayRef SEMICLN {
      AstNode *varDecl = new AstNode(ARRDECL);
       AstLeafNode *sharedNode = new AstLeafNode(QUAL, KWD___shared__);
       varDecl->addSibling(sharedNode);
       varDecl->addSibling($2);
       varDecl->addSibling($3);
       $$ = varDecl;
    }
    | KWD___shared__ typeSpecifier id SEMICLN {
       AstNode *varDecl = new AstNode(VARDECL);
       AstLeafNode *sharedNode = new AstLeafNode(QUAL, KWD___shared__);
       varDecl->addSibling(sharedNode);
       varDecl->addSibling($2);
       varDecl->addSibling($3);
       $$ = varDecl;
    }
    | KWD___shared__ typeSpecifier var OPERASN  exp  SEMICLN {
       AstNode *varDecl = new AstNode(VARDECLASSIGN);
       AstLeafNode *sharedNode = new AstLeafNode(QUAL, KWD___shared__);
       varDecl->addSibling(sharedNode);
       varDecl->addSibling($2);
       varDecl->addSibling($3);
       varDecl->addSibling($5);
       $$ = varDecl;
    }
    | typeSpecifier varDeclList SEMICLN {
       AstNode *varDecl = new AstNode(VARDECL);
       varDecl->addSibling($1);
       varDecl->addSibling($2);
       $$ = varDecl;
    }
    ;

varDeclList : id {
        $$ = $1;
    }
    | varDeclList COMMA id {
        AstNode *varDeclList;
        if ($1->getType() == VARDECLLIST)
        {
            varDeclList = $1;
            varDeclList->addSibling($3);
        }
        else
        {
            varDeclList = new AstNode(VARDECLLIST);
            varDeclList->addSibling($1);
            varDeclList->addSibling($3);
        }
        $$ = varDeclList;
    }
    ;

externDecl : KWD_EXTERN varDecl  {
        AstNode *externDecl = new AstNode (EXTERNDECL);
        externDecl->addSibling ($2);
        $$ = externDecl;
    }
    | KWD_EXTERN KWD___shared__ typeSpecifier id LSQBRKT RSQBRKT SEMICLN  {
        AstNode *externDecl = new AstNode (EXTERNARRDECL);
        AstNode *arrDecl = new AstNode(ARRDECL);
        AstLeafNode *sharedNode = new AstLeafNode(QUAL, KWD___shared__);
        arrDecl->addSibling(sharedNode);
        arrDecl->addSibling($3);
        arrDecl->addSibling($4);
        externDecl->addSibling (arrDecl);
        $$ = externDecl;
    }
    | KWD_EXTERN typeSpecifier id LSQBRKT RSQBRKT SEMICLN  {
        AstNode *externDecl = new AstNode (EXTERNARRDECL);
        AstNode *arrDecl = new AstNode(ARRDECL);
        arrDecl->addSibling($2);
        arrDecl->addSibling($3);
        externDecl->addSibling (arrDecl);
        $$ = externDecl;
    }
    ;

funTypeSpecifier : typeSpecifier {
		$$ = $1;
	}
	| KWD_STATIC typeSpecifier {
		/* just about a mirror image of ptrTypeSpecifier */
		/*fprintf (stderr, "DEBUG: funTypeSpecifier <- KWD_STATIC typeSpecifier\n");*/
		AstNode *staticNode = new AstNode(STATICNODE);
		staticNode->addSibling($2);
		$$ = staticNode;
	}
	;

/*ptrTypeSpecifier : typeSpecifier OPERMUL {
       AstNode *ptrNode = new AstNode(PTRNODE);
       ptrNode->addSibling($1);
       $$ = ptrNode;
    }
    ;*/

ptrTypeSpecifier : id OPERMUL {
    /*
        this rule is only used by formalDecl
        we cannot accept pointers to user-defined data types in typeSpecifier
            as this creates grammar ambiguity
            e.g. the text " x * y; " could be an expression or a declaration.
    */
      AstNode *ptrNode = new AstNode(PTRNODE);
      AstNode *typeNode = new AstLeafNode(IDTYPE);
      typeNode->addSibling ($1);
      ptrNode->addSibling(typeNode);
      $$ = ptrNode;
    }
    ;

typeSpecifier : id {
	  $$ = new AstLeafNode(IDTYPE);
      $$->addSibling ($1);
    }
    | builtinTypeSpec {
      $$ = $1;
    }
    | KWD_CONST builtinTypeSpec {
      $$ = new AstLeafNode (TYPE, KWD_CONST);
      $$->addSibling ($2);
    }
    | KWD_VOLATILE builtinTypeSpec {
      $$ = new AstLeafNode (TYPE, KWD_VOLATILE);
      $$->addSibling ($2);
    }
    ;

builtinTypeSpec :  integerTypeSpec {
      $$ = $1;
    }
    | KWD_UNSIGNED integerTypeSpec {
      $$ = new AstLeafNode(TYPE, KWD_UNSIGNED);
      $$->addSibling ($2);
    }
    | KWD_FLOAT {
      $$ = new AstLeafNode(TYPE, KWD_FLOAT);
    }
    | KWD_VOID {
      $$ = new AstLeafNode(TYPE, KWD_VOID);
    }
    | KWD_STRING {
      $$ = new AstLeafNode(TYPE, KWD_STRING);
    }
    | builtinTypeSpec OPERMUL {
       AstNode *ptrNode = new AstNode(PTRNODE);
       ptrNode->addSibling($1);
       $$ = ptrNode;
    }
    ;

integerTypeSpec : KWD_SHORT {
      $$ = new AstLeafNode(TYPE, KWD_SHORT);
    }
    | KWD_INT {
      $$ = new AstLeafNode(TYPE, KWD_INT);
    }
    | KWD_LONG {
      $$ = new AstLeafNode(TYPE, KWD_LONG);
    }
    | KWD_LONG KWD_LONG {
      $$ = new AstLeafNode(TYPE, KWD_LONG_LONG);
    }
    | KWD_CHAR {
      $$ = new AstLeafNode(TYPE, KWD_CHAR);
    }
    ;

assignStmt : var assignop exp SEMICLN {
      AstNode *assignNode = $2;
      assignNode->addSibling($1);
      assignNode->addSibling($3);
      $$ = assignNode;
    }
    | funCallExpr OPERASN exp SEMICLN {
      AstNode *assignNode = new AstNode(ASSIGN);
      assignNode->addSibling($1);
      assignNode->addSibling($3);
      $$ = assignNode;
    }
    | exp SEMICLN {
      AstNode *assignNode = new AstNode(ASSIGN);
      assignNode->addSibling($1);
      $$ = assignNode;
    }
    ;

assignNoSemi :  var assignop exp {
      AstNode *assignNode = $2;
      assignNode->addSibling($1);
      assignNode->addSibling($3);
      $$ = assignNode;
    }
    | typeSpecifier var OPERASN  exp {
       AstNode *varDecl = new AstNode(VARDECLASSIGN);
       varDecl->addSibling($1);
       varDecl->addSibling($2);
       varDecl->addSibling($4);
       $$ = varDecl;
    }
    | typeSpecifier id {
       AstNode *varDecl = new AstNode(VARDECL);
       varDecl->addSibling($1);
       varDecl->addSibling($2);
       $$ = varDecl;
    }
    | exp {     /* allows for update parts of for loops, et cetera */
      $$ = $1;
    }
    ;

assignop : OPERASN {
        $$ = new AstNode(ASSIGN);
    }
    | OPERADDASN {
        $$ = new AstNode(ASSIGNADD);
    }
    | OPERSUBASN {
        $$ = new AstNode(ASSIGNSUB);
    }
    | OPERMULASN {
        $$ = new AstNode(ASSIGNMUL);
    }
    | OPERDIVASN {
        $$ = new AstNode(ASSIGNDIV);
    }
    | OPERMODASN {
        $$ = new AstNode(ASSIGNMOD);
    }
    | OPERRSASN {
        $$ = new AstNode(ASSIGNRS);
    }
    | OPERLSASN {
        $$ = new AstNode(ASSIGNLS);
    }
    | OPERANDASN {
        $$ = new AstNode(ASSIGNAND);
    }
    | OPERORASN {
        $$ = new AstNode(ASSIGNOR);
    }
    | OPERXORASN {
        $$ = new AstNode(ASSIGNXOR);
    }
    ;

/*
assignStmtChain : var OPERASN assignStmt {
        AstNode *assignStmtChainNode;
        assignStmtChainNode = new AstNode(ASSIGNCHAIN);
        assignStmtChainNode->addSibling($1);
        assignStmtChainNode->addSibling($3);
    	$$ = assignStmtChainNode;
    }
    | var OPERASN assignStmtChain {
        AstNode *assignStmtChainNode;
        assignStmtChainNode = new AstNode(ASSIGNCHAIN);
        assignStmtChainNode->addSibling($1);
        assignStmtChainNode->addSibling($3);
    	$$ = assignStmtChainNode;
    }
    ;
*/

assignStmtList : assignNoSemi COMMA assignStmtList     {

	AstNode* firstNode = $1;
	AstNode *assignStmtListNode;
	if (firstNode->isAssignNode() || firstNode->isVarDeclNode()) {
	    assignStmtListNode = new AstNode(ASSIGNLIST);
	    assignStmtListNode->addSibling($1);
	    assignStmtListNode->addSibling($3);
	}
	else if (firstNode->isAssignStmtListNode()) {
	    assignStmtListNode = firstNode;
	    assignStmtListNode->addSibling($3);
	}
	else
	  error("Tree build error : assign list", FATAL);
	$$ = assignStmtListNode;
    }
    ;
    |  assignNoSemi {
      AstNode *assignStmtListNode = new AstNode(ASSIGNLIST);
      assignStmtListNode->addSibling($1);
      $$ = assignStmtListNode;
    }
    ;

condStmt : KWD_IF  LPAREN  exp  RPAREN  statement {
       AstNode *ifNode = new AstNode(COND);
       ifNode->addSibling($3);
       AstNode *body = $5;
       if (!body->isStmtListNode()) {
         AstNode *condBodyList = new AstNode(CONDBODY);
         condBodyList->addChild(body);
         ifNode->addChild(condBodyList);
       }
       else {
         body->setType(CONDBODY);
         ifNode->addChild(body);
       }
       $$ = ifNode;
    }
    | KWD_IF  LPAREN  exp  RPAREN  statement  KWD_ELSE  statement {
       AstNode *ifNode = new AstNode(COND);
       ifNode->addSibling($3);
       AstNode *condBodyList;
       AstNode *ontrue = $5;
       if (!ontrue->isStmtListNode()) {
         condBodyList = new AstNode(CONDBODY);
         condBodyList->addChild(ontrue);
         ifNode->addChild(condBodyList);
       }
       else {
         ontrue->setType(CONDBODY);
         ifNode->addChild(ontrue);
       }
       AstNode *onfalse = $7;
       if (!onfalse->isStmtListNode()) {
         condBodyList = new AstNode(CONDBODY);
         condBodyList->addChild(onfalse);
         ifNode->addChild(condBodyList);
       }
       else {
         onfalse->setType(CONDBODY);
         ifNode->addChild(onfalse);
       }
       $$ = ifNode;
    }
    ;

loopStmt : KWD_WHILE LPAREN exp RPAREN statement {
       AstNode *loopNode = new AstNode(LOOP);
       loopNode->addSibling($3);
       /*loopNode->addChild($5);*/
       AstNode *body = $5;
       AstNode *loopBodyList;
       if (!body->isStmtListNode()) {
	 loopBodyList = new AstNode(LOOPBODY);
	 loopBodyList->addChild(body);
	 loopNode->addSibling(loopBodyList);
       }
       else {
	 body->setType(LOOPBODY);
	 loopNode->addSibling(body);
       }
       $$ = loopNode;
    }
    | KWD_DO LCRLYBRKT statementList RCRLYBRKT KWD_WHILE LPAREN exp RPAREN SEMICLN {
        AstNode *loopNode = new AstNode(DWLOOP);
        AstNode *body = $3;
        AstNode *loopBodyList;
        if (!body->isStmtListNode()) {
            loopBodyList = new AstNode(DWLOOPBODY);
            loopBodyList->addChild(body);
            loopNode->addSibling(loopBodyList);
        }
        else {
            body->setType(DWLOOPBODY);
            loopNode->addSibling(body);
        }
        loopNode->addSibling($7);
        $$ = loopNode;
    }
    | KWD_FOR  LPAREN  assignStmtList  SEMICLN  exp  SEMICLN  assignStmtList  RPAREN  statement {
       AstNode *loopNode = new AstNode(LOOP);
       loopNode->addSibling($3);
       loopNode->addSibling($5);
       loopNode->addSibling($7);
       AstNode *body = $9;
       AstNode *loopBodyList;
       if (!body->isStmtListNode()) {
	 loopBodyList = new AstNode(LOOPBODY);
	 loopBodyList->addChild(body);
	 loopNode->addSibling(loopBodyList);
       }
       else {
	 body->setType(LOOPBODY);
	 loopNode->addSibling(body);
       }
       $$ = loopNode;
    }
    ;

returnStmt : KWD_RETURN SEMICLN {
      $$ = new AstNode(RET);
    }
    | KWD_RETURN exp SEMICLN {
      AstNode *retNode =  new AstNode(RET);
      retNode->addSibling($2);
      $$ = retNode;
    }
    ;

var : id  {
       AstLeafNode *scalarNode = new AstLeafNode(SCALAR_REF, yylval.num);
       scalarNode->addSibling($1);
       $$ = scalarNode;
    }
    | arrayRef {
      $$ = $1;
    }
    | id DOT id {
      AstNode *structNode = new AstNode(STRUCT_REF);
      structNode->addSibling($1);
      structNode->addSibling($3);
      char *comboName = (char *) malloc(sizeof(char) * MAXVARLENGTH);
      sprintf(comboName, "%s.%s", symtab->lookup($1->getVal()),
	      symtab->lookup($3->getVal()));
      int newIndex = symtab->lookupByName(comboName);
      if (newIndex < 0)
	newIndex = symtab->insert(comboName);
      ((*structNode)[1])->setVal(newIndex);
      free(comboName);
      $$ = structNode;
    }
    ;

exp : condExpr {
        $$ = $1;
    }
    ;

condExpr : logExpr {
        $$ = $1;
    }
    | logExpr OPERCOMP exp COLON condExpr {
        AstNode *condExpr = new AstNode (CONDEXPR);
        condExpr->addSibling ($1);
        condExpr->addSibling ($3);
        condExpr->addSibling ($5);
        $$ = condExpr;
    }
    ;

logExpr : bitExpr {
        $$ = $1;
    }
    | logExpr logop bitExpr {
        AstNode *logExpr = $2;
        logExpr->addSibling($1);
        logExpr->addSibling($3);
        $$ = logExpr;
    }
    ;

logop : OPERAND {
        $$ = new AstNode(LOGANDEXPR);
    }
    | OPEROR {
        $$ = new AstNode(LOGOREXPR);
    }
    ;

bitExpr : relExpr {
        $$ = $1;
    }
    | bitExpr bitop relExpr {
        AstNode *bitExpr = $2;
        bitExpr->addSibling($1);
        bitExpr->addSibling($3);
        $$ = bitExpr;
    }
    ;

bitop : OPERREF {
        $$ = new AstNode(BITANDEXPR);
    }
    | OPERBITOR {
        $$ = new AstNode(BITOREXPR);
    }
    | OPERCARAT {
        $$ = new AstNode(BITXOREXPR);
    }
    ;

relExpr : shiftExpr {
        $$ = $1;
    }
    | relExpr relop shiftExpr {
        AstNode *relExpr = $2;
        relExpr->addSibling($1);
        relExpr->addSibling($3);
        $$ = relExpr;
    }
    ;

relop : OPERLTE {
        $$ = new AstNode(LTEEXPR);
    }
    | OPERLT {
        $$ = new AstNode(LTEXPR);
    }
    | OPERGT {
        $$ = new AstNode(GTEXPR);
    }
    | OPERGTE {
        $$ = new AstNode(GTEEXPR);
    }
    | OPEREQ {
        $$ = new AstNode(EQEXPR);
    }
    | OPERNEQ {
        $$ = new AstNode(NEQEXPR);
    }
    ;

shiftExpr : addExpr {
       $$ = $1;
    }
    | shiftExpr shiftop addExpr  {
	AstNode *shiftExpr = $2;
	shiftExpr->addSibling($1);
	shiftExpr->addSibling($3);
	$$ = shiftExpr;
    }
    ;

shiftop : OPERSHIFTLEFT {
        $$ = new AstNode(SHIFTLEFTEXPR);
    }
    | OPERSHIFTRIGHT {
        $$ = new AstNode(SHIFTRIGHTEXPR);
    }
    ;

addExpr : term {
       $$ = $1;
    }
    | addExpr addop term {
	AstNode *addExpr = $2;
	addExpr->addSibling($1);
	addExpr->addSibling($3);
	$$ = addExpr;
    }
    ;

addop : OPERADD {
        $$ = new AstNode(ADDEXPR);
    }
    | OPERSUB {
        $$ = new AstNode(SUBEXPR);
    }
    ;

term : factor {
       $$ = $1;
    }
    | term mulop factor {
	AstNode *mulExpr = $2;
	mulExpr->addSibling($1);
	mulExpr->addSibling($3);
	$$ = mulExpr;
    }
    ;

mulop : OPERMUL {
        $$ = new AstNode(MULEXPR);
    }
    | OPERDIV {
        $$ = new AstNode(DIVEXPR);
    }
    | OPERMOD {
        $$ = new AstNode(MODEXPR);
    }
    ;

factor : LPAREN exp RPAREN {
        $$ = new AstNode (PARENEXPR);
        $$->addSibling ($2);
    }
    | varExpr {
       $$ = $1;
    }
    | funCallExpr {
       $$ = $1;
    }
    | INTCONST {
	$$ = new AstLeafNode(INT_REF, yylval.num);
    }
    | FLOATCONST {
	$$ = new AstLeafNode(FLOAT_REF, yylval.fnum);
    }
    | STRCONST {
	$$ = new AstLeafNode(STR_REF, yylval.num);
    }
    | CHARCONST {
	$$ = new AstLeafNode(CHAR_REF, yylval.num);
    }
    | LPAREN builtinTypeSpec RPAREN factor {
        $$ = new AstNode (TYPECASTEXPR);
        $$->addSibling ($2);
        $$->addSibling ($4);
    }
    | OPERSUB factor {
        $$ = new AstNode(NEGEXPR);
        $$->addSibling ($2);
    }
    | OPERNOT factor {
        $$ = new AstNode(NOTEXPR);
        $$->addSibling ($2);
    }
    | OPERMUL factor {
        $$ = new AstNode(DEREFEXPR);
        $$->addSibling ($2);
	}
    | OPERREF factor {
        $$ = new AstNode(ADDRESSOFEXPR);
        $$->addSibling ($2);
	}
    ;

varExpr : var {
			$$ = $1;
		}
	    | OPERPREINC var {
			AstNode *assignNode = new AstNode(PREINCSTMT);
      		assignNode->addSibling($2);
      		$$ = assignNode;
    	}
	    | var OPERPREINC {
      		AstNode *assignNode = new AstNode(POSTINCSTMT);
      		assignNode->addSibling($1);
      		$$ = assignNode;
     	}
	    | OPERDEC var {
			AstNode *assignNode = new AstNode(PREDECSTMT);
      		assignNode->addSibling($2);
      		$$ = assignNode;
    	}
	    | var OPERDEC {
      		AstNode *assignNode = new AstNode(POSTDECSTMT);
      		assignNode->addSibling($1);
      		$$ = assignNode;
     	}
		;

funid : ID  {
       $$ = new AstLeafNode(ID_REF, yylval.num);
    }
    ;

funCallExpr : funid LPAREN argList RPAREN {
       AstNode *funCallNode = new AstNode(FUNCALL);
       funCallNode->addSibling($1);
       funCallNode->addSibling($3);
       $$ = funCallNode;
    }
    ;

/*kernelCallExpr : funid OPERLT assignStmtList OPERGT LPAREN argList RPAREN {
       AstNode *kernelCallNode = new AstNode(KERNELCALL);
       kernelCallNode->addSibling($1);
       kernelCallNode->addSibling($3);
       kernelCallNode->addSibling($6);
       $$ = kernelCallNode;
    }
    ;*/

argList : { $$ = new AstLeafNode(DUMMY, DUMMYVAL); } 
    | exp {
      $$ = $1;
    }
    | argList COMMA exp {
	AstNode* firstNode = $1;
	AstNode *argListNode;
	if (firstNode->isArgListNode()) {
	    argListNode = firstNode;
	    argListNode->addSibling($3);
	}
	// TODO : check for expr nodes
	else {
	    argListNode = new AstNode(ARGLIST);
	    argListNode->addSibling($1);
	    argListNode->addSibling($3);
	}
	$$ = argListNode;
    }
    ;

id : ID  {
       $$ = new AstLeafNode(ID_REF, $1);
   }
   ;


%%

