/**
 * @author qasem < adopted from mcade > 
 * @date June 2 2011
 *
 * This is the instructions for flex to build the scanner for cuda
*/

/* DEFINITIONS */ 

%option lex-compat

ALPHA_CHAR  [a-zA-Z]
DIGIT       [0-9]
HEXDIGIT    [0-9a-fA-F]
OPS         [=+-]

%{

#include <stdio.h> ///< for error output (& dbug output)
#include <math.h>  ///< for atol etc.
#include <cuda.h> ///< my definitions
#include <ast/ast.h>
#include <cuda.tab.h> ///< token definitions
#include <symTab.h> ///< for my string table

#define STRTBL_SIZE 1024

/*#define SCAN_DBUG*/
#ifdef SCAN_DBUG
   #define scan_dbug(A...) printf(A)
#else
   #define scan_dbug(A...) /* nothing */
#endif


/**
 * the error function (for error reporting)
 */
void lex_error(int err);

int clean_cpy(char *dest, const char *src);

int yycolno = 1;

//extern sym_tab *ts_top;  ///< the string table

extern SymTab *symtab;

%}

%%

 /* RULES */

 /* keywords */
if      { scan_dbug("<KWD_IF>"); yycolno += yyleng; return KWD_IF; }
else    { scan_dbug("<KWD_ELSE>"); yycolno += yyleng; return KWD_ELSE; }
while   { scan_dbug("<KWD_WHILE>"); yycolno += yyleng; return KWD_WHILE; }
do      { scan_dbug("<KWD_DO>"); yycolno += yyleng; return KWD_DO; }
for     { scan_dbug("<KWD_FOR>"); yycolno += yyleng; return KWD_FOR; }
int     { scan_dbug("<KWD_INT>"); yycolno += yyleng; return KWD_INT; }
long    { scan_dbug("<KWD_LONG>"); yycolno += yyleng; return KWD_LONG; }
short   { scan_dbug("<KWD_SHORT>"); yycolno += yyleng; return KWD_SHORT; }
float   { scan_dbug("<KWD_FLOAT>"); yycolno += yyleng; return KWD_FLOAT; }
string  { scan_dbug("<KWD_STRING>"); yycolno += yyleng; return KWD_STRING; }
char    { scan_dbug("<KWD_CHAR>"); yycolno += yyleng; return KWD_CHAR; }
return  { scan_dbug("<KWD_RETURN>"); yycolno += yyleng; return KWD_RETURN; }
switch  { scan_dbug("<KWD_SWITCH>"); yycolno += yyleng; return KWD_SWITCH; }
case    { scan_dbug("<KWD_CASE>"); yycolno += yyleng; return KWD_CASE; }
default { scan_dbug("<KWD_DEFAULT>"); yycolno += yyleng; return KWD_DEFAULT; }
void    { scan_dbug("<KWD_VOID>"); yycolno += yyleng; return KWD_VOID; }
asm     { scan_dbug("<KWD_ASM>"); yycolno += yyleng; return KWD_ASM; }
"__global__" { scan_dbug("<KWD___global__>"); yycolno += yyleng; return KWD___global__; }
"__shared__" { scan_dbug("<KWD___shared__>"); yycolno += yyleng; return KWD___shared__; }
"__device__" { scan_dbug("<KWD___device__>"); yycolno += yyleng; return KWD___device__; }
static  { scan_dbug("<KWD_STATIC>"); yycolno += yyleng; return KWD_STATIC; }
const   { scan_dbug("<KWD_CONST>"); yycolno += yyleng; return KWD_CONST; }
volatile { scan_dbug("<KWD_VOLATILE>"); yycolno += yyleng; return KWD_VOLATILE; }
unsigned { scan_dbug("<KWD_UNSIGNED>"); yycolno += yyleng; return KWD_UNSIGNED; }
inline   { scan_dbug("<KWD_INLINE>"); yycolno += yyleng; return KWD_INLINE; }
register { scan_dbug("<KWD_REGISTER>"); yycolno += yyleng; return KWD_REGISTER; }
extern   { scan_dbug("<KWD_EXTERN>"); yycolno += yyleng; return KWD_EXTERN; }

 /* identifiers */
"__syncthreads"|({ALPHA_CHAR}|_)({ALPHA_CHAR}|_|{DIGIT})* { 
                  int index = symtab->lookupByName(yytext);
                  if (index < 0) { 
                     yylval.num = symtab->insert(yytext);
                     if(yylval.num < 0) {
                       perror("symTab_insert failed");
                       exit(1);
                      }
                      scan_dbug("<ID, %d>", yylval.num);
                   }
                   else {
                     yylval.num = index;
                     scan_dbug("<ID, %d>", yylval.num);
                   }
                  
                  yycolno += yyleng; 
                  return ID; 
               }

 /* constants */

 /* integer const */
0|([1-9]{DIGIT}*) { scan_dbug("<INTCONST, %d>", atoi(yytext));
                    yycolno += yyleng; 
                    yylval.num = atoi(yytext);
                    return INTCONST; }

0[xX]{HEXDIGIT}+    {   /* integer constant in hexadecimal format */
                        long hexVal;
                        char *dontcare;
                        
                        yycolno += yyleng;
                        hexVal = strtol(yytext, &dontcare, 16);
                        yylval.num = (int)hexVal;
                        scan_dbug("<INTCONST, %d>", yylval.num);
                        return INTCONST;
                    }

 /* float const */
((({DIGIT}*)"."({DIGIT}+)|({DIGIT}+)"."({DIGIT}*))([fF]|([eE][-+]?{DIGIT}+))?) {
                        double value;
                        
                        yycolno += yyleng;
                        value = atof (yytext);
                        yylval.fnum = value;
                        scan_dbug("<FLOATCONST, %f>", yylval.fnum);
                        return FLOATCONST;
                    }
 
 /* character const */
'{ALPHA_CHAR}'|'{DIGIT}'|'{OPS}' { yycolno += yyleng;
                 yylval.num = *(yytext+1);
                 scan_dbug("<CHARCONST, %c>", yylval.num);
                 return CHARCONST; }

 /* string const */
\"[^"\n]* { 
              register int c;
              c = yyinput();
              if(c != '"') {
                 lex_error(UNTSTR);
              }
              else if(yytext[yyleng-1] == '\\') {
                 unput(c);
                 yymore();
              }
              else {
                 char *clean_txt;

                 clean_txt = (char*)malloc(yyleng);
                 if(clean_txt == NULL) {
                    perror("malloc failed");
                    exit(1);
                 }

                 clean_cpy(clean_txt, yytext+1);

                 yylval.num = symtab->insert(clean_txt);
                 if(yylval.num < 0) {
                     perror("symTab_insert failed");
                     exit(1);
                  }

                 free(clean_txt);

                 scan_dbug("<STRCONST, %d>", yylval.num);
                 yycolno += yyleng;
                 return STRCONST;
              }
           }
            

 /* operators */
"+"  { scan_dbug("<OPERADD>"); yycolno += yyleng; return OPERADD; }
"-"  { scan_dbug("<OPERSUB>"); yycolno += yyleng; return OPERSUB; }
"*"  { scan_dbug("<OPERMUL>"); yycolno += yyleng; return OPERMUL; }
"/"  { scan_dbug("<OPERDIV>"); yycolno += yyleng; return OPERDIV; }
"%"  { scan_dbug("<OPERMOD>"); yycolno += yyleng; return OPERMOD; }
"<"  { scan_dbug("<OPERLT>"); yycolno += yyleng; return OPERLT; }
">"  { scan_dbug("<OPERGT>"); yycolno += yyleng; return OPERGT; }
"<=" { scan_dbug("<OPERLTE>"); yycolno += yyleng; return OPERLTE; }
">=" { scan_dbug("<OPERGTE>"); yycolno += yyleng; return OPERGTE; }
"==" { scan_dbug("<OPEREQ>"); yycolno += yyleng; return OPEREQ; }
"!=" { scan_dbug("<OPERNEQ>"); yycolno += yyleng; return OPERNEQ; }
"="  { scan_dbug("<OPERASN>"); yycolno += yyleng; return OPERASN; }
"|"  { scan_dbug("<OPERBITOR>"); yycolno += yyleng; return OPERBITOR; }
"<<" { scan_dbug("<OPERSHIFTLEFT>"); yycolno += yyleng; return OPERSHIFTLEFT; }
">>" { scan_dbug("<OPERSHIFTRIGHT>"); yycolno += yyleng; return OPERSHIFTRIGHT; }

 /* shorthand */
"+=" { scan_dbug("<OPERADDASN>"); yycolno += yyleng; return OPERADDASN; }
"-=" { scan_dbug("<OPERSUBASN>"); yycolno += yyleng; return OPERSUBASN; }
"*=" { scan_dbug("<OPERMULASN>"); yycolno += yyleng; return OPERMULASN; }
"/=" { scan_dbug("<OPERDIVASN>"); yycolno += yyleng; return OPERDIVASN; }
"%=" { scan_dbug("<OPERMODASN>"); yycolno += yyleng; return OPERMODASN; }
">>=" { scan_dbug("<OPERRSASN>"); yycolno += yyleng; return OPERRSASN; }
"<<=" { scan_dbug("<OPERLSASN>"); yycolno += yyleng; return OPERLSASN; }
"&=" { scan_dbug("<OPERANDASN>"); yycolno += yyleng; return OPERANDASN; }
"|=" { scan_dbug("<OPERORASN>"); yycolno += yyleng; return OPERORASN; }
"^=" { scan_dbug("<OPERXORASN>"); yycolno += yyleng; return OPERXORASN; }
"++" { scan_dbug("<OPERPREINC>"); yycolno += yyleng; return OPERPREINC; }
"--" { scan_dbug("<OPERDEC>"); yycolno += yyleng; return OPERDEC; }

"||"  { scan_dbug("<OPEROR>"); yycolno += yyleng; return OPEROR; }
"&&"  { scan_dbug("<OPERAND>"); yycolno += yyleng; return OPERAND; }
"!"  { scan_dbug("<OPERNOT>"); yycolno += yyleng; return OPERNOT; }

"&"  { scan_dbug("<OPERREF>"); yycolno += yyleng; return OPERREF; }
"^"  { scan_dbug("<OPERCARAT>"); yycolno += yyleng; return OPERCARAT; }
"?"  { scan_dbug("<OPERCOMP>"); yycolno += yyleng; return OPERCOMP; }

 /* brackets & parens */
"[" { scan_dbug("<LSQBRKT>"); yycolno += yyleng; return LSQBRKT; }
"]" { scan_dbug("<RSQBRKT>"); yycolno += yyleng; return RSQBRKT; }
"{" { scan_dbug("<LCRLYBRKT>"); yycolno += yyleng; return LCRLYBRKT; }
"}" { scan_dbug("<RCRLYBRKT>"); yycolno += yyleng; return RCRLYBRKT; }
"(" { scan_dbug("<LPAREN>"); yycolno += yyleng; return LPAREN; }
")" { scan_dbug("<RPAREN>"); yycolno += yyleng; return RPAREN; }

 /* punctuation */

\. { scan_dbug("<DOT>"); yycolno += yyleng; return DOT; }
, { scan_dbug("<COMMA>"); yycolno += yyleng; return COMMA; }
: { scan_dbug("<COLON>"); yycolno += yyleng; return COLON; }
; { scan_dbug("<SEMICLN>"); yycolno += yyleng; return SEMICLN; }

 /* comments */
"//"[^\n]* { /* do nothing */}

"/*" {
        register int c1, c2;
        int beginLineno = yylineno;
        yycolno += yyleng;
        c2 = yyinput();
        ++yycolno;
        if(c2 <= 0) {
           yylineno = beginLineno;
           lex_error(UNTCMT);
        }
        else {
           c1 = c2;
           c2 = yyinput();
           ++yycolno;
           while(!((c1 == '*') && (c2 == '/')) && (c2 > 0)) {
              c1 = c2; 
              c2 = yyinput();
              ++yycolno;
           }
           scan_dbug("<COMMENT>");
           if(c2 <= 0) {
              yylineno = beginLineno;
              lex_error(UNTCMT);
           }
        }
     }


#[^\n]* { scan_dbug("<PREPROC>"); 
           char *clean_txt;

           clean_txt = (char*)malloc(yyleng);
           if(clean_txt == NULL) {
                    perror("malloc failed");
                    exit(1);
                 }

                 clean_cpy(clean_txt, yytext+1);

                 yylval.num = symtab->insert(clean_txt);
                 if(yylval.num < 0) {
                     perror("symTab_insert failed");
                     exit(1);
                  }

                 free(clean_txt);

                 scan_dbug("<STRCONST, %d>", yylval.num);
        yycolno = 1; 
    return PREPROC; 
         }

 /* some catchalls */
0{DIGIT}* { lex_error(INVTOK); }

\n { scan_dbug("\n"); yycolno = 1; }

(" "|\t)+ { scan_dbug("%s", yytext); yycolno += yyleng; /* do nothing */ }

.  { /* anything not caught by this point is bad */ 
     lex_error(INVTOK); }

%%

void lex_error(int err)
{
   fprintf(stderr, "%d:%d ", yylineno, yycolno);
   switch(err) {
      case INVTOK:
         fprintf(stderr, "invalid token\n");
         break;
      case UNTSTR:
         fprintf(stderr, "unterminated string\n");
         break;
      case UNTCMT:
         fprintf(stderr, "unterminated comment\n");
         break;
      default:
         fprintf(stderr, "INVALID LEX ERROR NUMBER\n");
         exit(1);
   }
}

int clean_cpy(char *dest, const char *src)
{
   int i = 0;
   int k = 0;

   while(src[i]) {
      if(src[i] == '\\') {
         switch(src[i+1]) {
            case 'n':
               dest[k++] = '\n';
               i += 2;
               break;
            case 't':
               dest[k++] = '\t';
               i += 2;
               break;
            case '"':
               dest[k++] = '\"';
               i += 2;
               break;
            case '\\':
               dest[k++] = '\\';
               i += 2;
               break;
            default:
               ++i;
         }
      }
      else {
         dest[k++] = src[i++];
      }
   }
   dest[k] = '\0';

   return k;
}
