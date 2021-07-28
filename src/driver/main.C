#include <stdio.h>
#include <fstream>

#include <stdlib.h>
#include <unistd.h>

#include <ast/ast.h>
#include <ast/astutil.h>
#include <ast/cudautil.h>
#include <ast/prettyPrint.h>
#include <frontend/cuda.h>
#include <frontend/symTab.h>
#include <ast/calUsage.h>
#include <extraction/genILOC.h>
#include <extraction/extractAST.h>
#include <extraction/featureVector.h>

#define DEFAULT_OUT_NAME "a.out"  // default output file name
#define YYDEBUG 1

#ifdef PARSE_DBUG
   FILE *parse_dbug;
   #define dbug_parser(A...) fprintf(parse_dbug, A)
#else
   #define dbug_parser(A...) /* nothing */
#endif


/* thread-coarsening function defined in coarsening.C */
void coarsening (AstNode *ast, SymTab *symtab, int coarseningFactor, int blockSize);


char *outputName;
unsigned char do_asm;
unsigned char do_link;
unsigned char display_tree;
unsigned char feature_extraction;
unsigned char do_regs;
unsigned char do_coarsening;
SymTab *symtab;

//extern char *nodeNames[];
extern AstNode *ast; 
extern FILE *yyin;
extern int yylineno;
extern int yyparse();

/* prints usage info for mcc */
void usage(char *exe) {
  fprintf(stderr, "%s [-o outfile] [-d] [-S] [-c] infile\n"
	  "\n outfile -- the name of the output file\n"
	  "-d -- include debugging print macros\n"
	  "-S -- stop after compile (do not assemble)\n"
	  "-c -- stop after assembly (do not link)\n"
	  "infile -- the name of the input file\n"
	  "\n the only required argument is infile\n",
	  exe);
}

/* parses command line args for mcc and sets up global variables
   according to the user's arguments */
void parseArgs(int argc, char **argv) {
    signed char c;
    
    outputName = DEFAULT_OUT_NAME;
    do_asm = 1;
    do_link = 1;
    display_tree = 0;
    feature_extraction = 0;
    do_regs = 0;
    do_coarsening = 1;      // may switch to off by default in future

    while((c = getopt(argc, argv, "o:dSchiTFl:RC")) > 0) {
        switch(c) {
            case 'o' :
               outputName = optarg;
               break;

            case 'd' :
                break;

            case 'S' :
               do_asm = 0;
               break;
            
            case 'c' :
               do_link = 0;
               break;

			case 'T':
				display_tree = 1;
				break;

			case 'F':
				feature_extraction = 1;
				break;

			case 'l':
				yylineno = atoi (optarg);
				break;

            case 'R' :
               do_regs = 1;
               break;
            
            case 'C' :
               do_coarsening = 0;       // may switch to enabling in future
               break;
            
            case 'i' :
            case 'h' :
            default :
               usage(argv[0]);
               exit(0);
        }
    }

    if (argv[optind]) {
	    yyin = fopen(argv[optind], "r");
	    if (!yyin) {
            perror("couldn't open input file");
            exit(1);
        }
    }
    else {
        fprintf(stderr, "No input file\n");
        exit(0);
    }

} // parseArgs()


int main(int argc, char **argv) {

    parseArgs(argc, argv);

    symtab = new SymTab();

    yyparse();

    if (ast) {
        if (do_coarsening)
        {
            /* thread coarsening */
            
            int coarseningFactor = (int)atoi (argv[optind + 1]);
            int blockSize = (int)atoi(argv[optind + 2]);
            
            coarsening (ast, symtab, coarseningFactor, blockSize);
        }
        
        /* output the transformed code */
        prettyPrintAst(ast, 0);
        
        /* estimate register usage, if requested */
        if (do_regs)
            predictUsage(ast);
        
        /* display AST if requested */
        if (display_tree)
            printAST (ast, 0, 0);
        
        /* do feature extraction if requested */
        if (feature_extraction)
        {
            /* set up feature vector */
            FeatureVector fv;
            
            /* AST-level extraction */
            extractAST (ast, fv);
        
            /* ILOC-level extraction */
            /* DEBUG: printing partial ILOC */
            //printf ("DEBUG: generating ILOC...\n");
            struct ILLine *kernel = generateKernel (ast);
            //printILList (kernel);
            /* TODO: extract information from the ILOC to our feature vector */
        
            /* CFG-level extraction ... */
            
            
            /* write out the feature vector to designated file */
            /* TODO: make filename configurable? */
            //printf ("DEBUG: writing out FV.txt...\n");
            ofstream featureFile;
            featureFile.open ("FV.txt");
            fv.write (featureFile);
            featureFile.close ();
        }
    } // end if (ast)
        
    return 0;
} // end of main()

