#ifndef ASTDEFS_H
#define ASTDEFS_H

#define MAXCHILDREN 1000
#define MAXNODETYPES 256
#define MAXDATATYPES 10
#define MAXTOKENTYPES 256
#define MAXQUALTYPES 8

/* order must match with enum NODE_TYPE in ast.h */
const char *nodeNames[MAXNODETYPES] = {"dummy",
				 "kernel", 
				 "funNameType", "funheader", "formaldecl", "funbody",
				 "type", "user-defined type", "qual", "static", "pointer",
                 "++", "++", "--", "--",
				 "loop", "do-whileLoop", "loopBody", "do-whileLoopBody", "cond", "condBody", "compound",
				 "vardecl", "vardeclAssign", "arrayDecl", "arrayDeclAssign",
				 "varDecls", "varDeclList", "externDecl", "externArrayDecl",
 				 "=", "+=", "-=", "*=", "/=", "%=",
				  ">>=", "<<=", "&=", "|=", "^=", "ret",
				 "formalparamlist", "stmtlist", /*"assignchain",*/ "assignlist", "arglist",
				 "expr", "condExpr",
				 "<=", "<", ">=", ">", "==", "!=",
				 "+", "-", "*", "/", "%",
				 "funcall", /*"kernelCall",*/ "-", "!", "*", "&",
				 "scalarRef", "arrayRef", "structRef", "id",
				 "intRef", "floatRef", "charRef", "strRef",
				 "arrayInitList", "arrayInit",
                 "&&", "||", "&", "|", "^", "<<", ">>",
                 "(", "typecast"};


/* order must match with %token list in cuda.y */
const char *tokenNames[MAXTOKENTYPES] = {"dummy", "id", "intconst", "charconst", "strconst",
				   "if", "else", "while", "for", 
				   "int", "char", "float", "double", "void", "string",
				   "return", "switch", "case", "default", "bool",
				   "__global__", "__shared__", "__device__",
				   "+", "-", "*", "/", "<", ">", ">=", "<=", 
				   "==", "!=", "=", "+=", "-=", ">>=",
				   "++", "||", "&&", "!", "^", "?", "&",
				   "[", "]", "{", "}", "(", ")", ".", ":", ",", ";",
				   "invaild", "unterm comment", "unterm string",
				   "asm", "preproc", "static", "const",
                   "|", "<<", ">>",
                   "short", "unsigned",
				   "--", "floatconst", "volatile", "%",
				   "inline", "register", "extern",
				   "<<=", "*=", "/=", "%=", "&=", "|=", "^=",
                   "long", "do", "long long"
                   };



#endif 
