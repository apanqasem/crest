#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<frontend/symTab.h>
#include<driver/errors.h>
#include "calUsage.h"
#include <frontend/varTab.h>

extern char *tokenNames[];
extern char *nodeNames[];
extern SymTab *symtab;
extern VarTab *vartab = new VarTab();
char  * type = "Empty", *varName = "Empty";
int iteration = 0 , iArray[20], scope = 0;
bool init = 0, limit = 0, step = 0, len =0;

void predictingUsage(AstNode *node) {

  int kind = node->getType();

  list<AstNode*>::iterator i;
  list<AstNode*> *children = node->getChildren();
  int childCnt = node->getNumChildren();
  int sibs = node->getNumSiblings(); 

  unsigned int k;

  //FILE *fp = fopen("varData.txt","a");  

   //fprintf(fp,"\nVariable Data"); 
   //fprintf(fp,"\nType %s",nodeNames[kind]);
   //fprintf(fp,"\nChild %d",childCnt);
   //fprintf(fp,"\nSibs %d",sibs);

   switch(kind) {
   case ASSIGN: 
          //fprintf(fp,"\nASSIGN case");
	  if ( sibs > 0 ) {
		  char * lhsVarName = "Empty", * rhsVarName = "Empty";
		  int flag = 0;  
	   	  for(k = 0,i = node->begin(); i != node->end(); k++,i++){
		       if (k == 1){
		                lhsVarName = symtab->lookup((*i)->getVal());    
		                //fprintf(fp, "\n*******/////*******%s\n", symtab->lookup((*i)->getVal()) );
		       }
		       if ( k == 2 ){
				int z = (*i)->getType();
			        if ( !strcmp(nodeNames[z],"scalarRef") || !strcmp(nodeNames[z],"structRef")) 
		       			flag = 0;
		       }
		       predictingUsage(*i);
		       if ( varName != NULL)
		 		rhsVarName = varName;
		  }     
		  if ( flag > 0){ 
			//fprintf(fp, "\n//////RHS Variable\\\\\\\ %s\n", rhsVarName );
			if( vartab->lookupByName(rhsVarName) >= 0 )
		        	vartab->setCopyVar(rhsVarName,lhsVarName);
		  }
	}

    break;

    case TYPE:
           type = tokenNames[node->getVal()];
    break;
    case ARRDECL:
	//fprintf(fp,"\nARRAYDECL case");
        if (sibs == 2) {         
	       list<AstNode*>::iterator iter0;
	       for(k = 0 ,iter0 = node->begin(); iter0 != node->end(); k++, iter0++) 
		        predictingUsage(*iter0);
        }
        if (sibs == 3) {         
		list<AstNode*>::iterator iter0;
		for(k = 0 ,iter0 = node->begin(); iter0 != node->end(); k++, iter0++) {
			if ( k == 0 ){
		                 if ( !strcmp(tokenNames[(*iter0)->getVal()],"__shared__") ) 
		                                scope = 1;
		        }
		        predictingUsage(*iter0);
		}
        }
        scope = 0;
    break;
    case ARRAY_REF:
       //fprintf(fp,"\nArray case");
	if ( sibs > 0 ) {
		list<AstNode*>::iterator iter_0;     
		 for(k =0 ,iter_0 = node->begin(); iter_0 != node->end(); k ++,iter_0++){
			if( k == 0 ){
		             char *varName = symtab->lookup((*iter_0)->getVal());
			     if( (vartab->lookupByName(varName)) >= 0){
		                int index = vartab->lookupByName(varName);
		                type = vartab->getDataType(index);
		             	vartab->insert(symtab->lookup((*iter_0)->getVal()), type,scope);
			     }
			}
		        int z = (*iter_0)->getType();
		        if ( !strcmp(nodeNames[z],"scalarRef") )
				scope = 0;
		        predictingUsage(*iter_0);             
		  }    
        } 
    break;
   case FUNNAMETYPE:
	 if ( sibs > 0 ) {
		//fprintf(fp,"\nFUNCTION_DECL case");
		list<AstNode*>::iterator iter0;
		 scope = -1;      
		 for(iter0 = node->begin(); iter0 != node->end(); iter0++)
		 {
		          //fprintf(fp,"\n%d",scope);    
			predictingUsage(*iter0);             
		  }
        }
        scope = 0;
    break;
    case FORMALDECL:
        if ( sibs > 0 ) {
		//fprintf(fp,"\nFORMAL_DECL case");
		list<AstNode*>::iterator iter0;
		 scope = 2;      
		 for(k = 0,iter0 = node->begin(); iter0 != node->end(); k++,iter0++) 
		            predictingUsage(*iter0); 
	}
        scope = 0;
    break;
    case ID_REF:
	  //fprintf(fp,"\nID_REF case");
          varName = symtab->lookup((node)->getVal());
          //fprintf(fp,"\n%s",varName);
          //fprintf(fp,"\n%d",scope); 
          if( ( vartab->lookupByName(varName) < 0) && scope != -1){
                //fprintf(fp, "\n*******/////*******%s\n",varName); 
            	vartab->insert(varName,type,scope);
          }
          else
	   	vartab->incReadCount(varName,0);	
    break;
    case VARDECLASSIGN: 
         //fprintf(fp,"\nVARDECLASSIGN case");
         if ( sibs > 0 ) {  
         	char * lhsVarName = "Empty", * rhsVarName = "Empty";
        	int flag = 0;        
	        list<AstNode*>::iterator iter0;
	        if (sibs == 3) {		
			for(k = 0 ,iter0 = node->begin(); iter0 != node->end(); k++, iter0++) {
				   if (k == 1){
				        lhsVarName = symtab->lookup((*iter0)->getVal());    
					//fprintf(fp, "\n*******/////*******%s\n", symtab->lookup((*iter0)->getVal()) );
					   vartab->insert(symtab->lookup((*iter0)->getVal()), type,scope);
				   }
				   if ( k == 2 ){
					int z = (*iter0)->getType();
					if ( !strcmp(nodeNames[z],"scalarRef")) 
					{
						flag = 1;
				                //fprintf(fp, "\n//////k = 2\\\\\\\ %s\n", nodeNames[z] );
					}
					if ( !strcmp(nodeNames[z],"structRef")) 
				        {
				                flag = 2;
				                 //fprintf(fp, "\n//////flag = 2\\\\\\\ %s\n", nodeNames[z] );
				        }
				   }
				   predictingUsage(*iter0);
				   if ( varName != NULL)
					rhsVarName = varName;
			
			}
	      }
	      if (sibs == 4) {
		      list<AstNode*>::iterator iter1; 
		      for(k = 0, iter1 = node->begin(); iter1 != node->end(); k++, iter1++) {
				   if ( k == 0 ){
				                if ( !strcmp(tokenNames[(*iter1)->getVal()],"__shared__") ) 
				                        scope = 1;
				   }
				   if (k == 2)
					   vartab->insert(symtab->lookup((*iter1)->getVal()), type,scope);
				   predictingUsage(*iter1);
			}
	     }
	    if ( flag > 0){ 
		//fprintf(fp, "\n//////RHS Variable\\\\\\\ %s\n", rhsVarName );
		if( vartab->lookupByName(rhsVarName) >= 0 )
	    		vartab->setCopyVar(rhsVarName,lhsVarName);
	     }
        }
    break;
    case LOOP: 
	    //fprintf(fp,"\nLoop case");
	    if (sibs > 0) {
		    list<AstNode*>::iterator iter2; 
		    for(iter2 = node->begin(); iter2 != node->end(); iter2++)
				   predictingUsage(*iter2);  	
	     }

    break;
    case FUNBODY:

	//fprintf(fp,"\nfunbody case");
        //fprintf(fp,"\nChild Count %d",childCnt); 
	if (childCnt > 0){ 
		list<AstNode*>::iterator iter3;
		for ( iter3 = children->begin(); iter3 !=children->end(); iter3++ ){

	  		kind = (*iter3)->getType();
	  		//fprintf(fp,"\nChild Type %s",nodeNames[kind]);
		        predictingUsage(*iter3);
		        
		}
	}
   break;
  case LOOPBODY:

	//fprintf(fp,"\nloopbody case");
	//fprintf(fp,"\nChild Count %d",childCnt); 
        if (childCnt > 0){ 
		list<AstNode*>::iterator iter4;
		for ( iter4 = children->begin(); iter4 !=children->end(); iter4++ ){

		        kind = (*iter4)->getType();
		        //fprintf(fp,"\nChild Type %s",nodeNames[kind]);
			predictingUsage(*iter4);
		}
	}

  break;
  default:
        //fprintf(fp,"\nDefault case");
        //fprintf(fp,"\t%s",nodeNames[kind]);
	kind = node->getType();
	children = node->getChildren();
	childCnt = node->getNumChildren();

	//fprintf(fp,"\nChild Count %d",childCnt); 

	if (childCnt > 0){ 
		for ( i = children->begin(); i !=children->end(); i++ ){
			  kind = (*i)->getType();
			  //fprintf(fp,"\nChild Type %s",nodeNames[kind]);
			  predictingUsage(*i);
		}
	}
	else {
	        for ( i = node->begin(); i !=node->end(); i++ ){
			  kind = (*i)->getType();  
			  //fprintf(fp,"\nNode Type %s",nodeNames[kind]);                                                      
			  predictingUsage(*i);
		}
	}
	break;
   }
   //if(fp)
  	//fclose(fp);
}

void predictUsage(AstNode *node) {
    
       
        int regCount = 0;
        predictingUsage(node);
        FILE *fp = fopen("Data.txt","a");
        fprintf(fp, "\n*************************\n");  
	fprintf(fp,"\nSize = %d",vartab->getSize());  
 	for (int t = 0; t < vartab->getSize() ; t++) {
    		fprintf(fp,"\n%s",vartab->lookup(t));
    		fprintf(fp,"\t\t%d",vartab->getReadCount(t));
                fprintf(fp,"\t%s",vartab->getScope(t));    
                fprintf(fp,"\t%s",vartab->getDataType(t));   
  		fprintf(fp,"\t%d",vartab->getCopyVar(t));
		if ( !strcmp(vartab->getScope(t),"Register") && vartab->getCopyVar(t) == -1 )
			regCount++;
	}

        fprintf(fp,"\n*********************************\n\t No. of Registers = %d",regCount);
        fclose(fp);

}
