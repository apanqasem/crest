#include<cudautil.h>
#include<astutil.h>
#include<string.h>
#include<frontend/symTab.h>

#define MAXTHREADIDVAR 16

extern SymTab *symtab;
extern char *nodeNames[];

const char* dimToAxis(int dim) {
  switch(dim) {
  case 0:
    return "x";
  case 1: 
    return "y";
  case 2:
    return "z";
  default:
    return "x";
  }
}


bool isThreadId(AstNode *node, int dim) {
  if (!isStructRef(node))
    return false;
  AstNode *structName = (*node)[0];
  AstNode *fieldName = (*node)[1];

  char *comboName = (char *) malloc(sizeof(char) * MAXTHREADIDVAR);
  sprintf(comboName, "threadIdx.%s", dimToAxis(dim));
  
  bool match = idMatch(fieldName, comboName);
  free(comboName);

  return match;
}  


int lookupThreadIdinST(int dim) {
  char *comboName = (char *) malloc(sizeof(char) * MAXTHREADIDVAR);
  sprintf(comboName, "threadIdx.%s", dimToAxis(dim));
  
  int index = symtab->lookupByName(comboName);
  free(comboName);
  return index;
}


AstNode *creatThreadIdRef(int dim) {
  char *comboName = (char *) malloc(sizeof(char) * MAXTHREADIDVAR);
  sprintf(comboName, "threadIdx.%s", dimToAxis(dim));
 
  int index = symtab->lookupByName(comboName);
  if (index < 0) 
    index = symtab->insert(comboName);

  AstNode *structure = new AstLeafNode(ID_REF, index);
  AstNode *field = new AstLeafNode(ID_REF, index);

  AstNode *structNode = new AstNode(STRUCT_REF);
  structNode->addSibling(structure);
  structNode->addSibling(field);
  
  return structNode;
}

AstNode *findThreadIdFirstRef(AstNode *node, int dim) {
  //FILE *fp = fopen("SyncData2.txt","a");
  int kind = (node)->getType();
  //fprintf(fp,"\nStarting Loop  %s",nodeNames[kind]);
  if (isStructRef(node)) {
    if (isThreadId(node, dim)) {
      return node;
    }
    else 
      return NULL;
  }
  
  list<AstNode*>::iterator i;

  AstNode *foundNode;
  for(i = node->begin(); i != node->end(); i++) {
    int kind = (*i)->getType();
  //fprintf(fp,"\nnode  %s",nodeNames[kind]);
    foundNode = findThreadIdFirstRef(*i, dim);
    if (foundNode) 
      return foundNode;
  }

  list<AstNode*> *children = node->getChildren();
  for(i = children->begin(); i != children->end(); i++) {
  int kind = (*i)->getType();
  //fprintf(fp,"\nChildnode  %s",nodeNames[kind]);
    foundNode = findThreadIdFirstRef(*i, dim);
    if (foundNode) 
      return foundNode;
  }
//fclose(fp);
  return NULL;
}


bool CheckParent(AstNode *node){

//FILE *fp = fopen("SyncData5.txt","a");
  AstNode* parent = node->getParent();      
int kind = (parent)->getType();
//fprintf(fp,"\n Parent %s",nodeNames[kind]);
//fclose(fp);
  if (parent){
  
    if (parent->getType() == LOOPBODY)
         return false;
    if (parent->getType() == FUNBODY)
         return true;
  }

  return CheckParent(parent);       
} 


bool CheckSyncThread(AstNode *node) {

//FILE *fp = fopen("SyncData3.txt","a");
//fprintf(fp,"\nHi");   
   if (isIdRef(node)) 
   {
  int kind = (node)->getType();
  char * varName = symtab->lookup((node)->getVal());
  //fprintf(fp,"\nStarting Loop  %s\t%s",nodeNames[kind],varName);
  //fclose(fp); 
 	if (!strcmp(varName,"__syncthreads") )
   	{
       	 	if(CheckParent(node))            
                     return true;
		else{ 
	             //return false;
		     fprintf(stderr,"\nViolation : __syncthread call inside loop\n\n");
                     exit(0);
                }
   	}
   	else
      	   return false;
  }
  //fclose(fp); 
  return false;
}

list<AstNode*> syncList;

list<AstNode*> findSyncThreadRef(AstNode *node, int test) {
 
  //FILE *fp = fopen("SyncData8.txt","a");
  if( test == 0 ){
    //fprintf(fp,"\n New Call");
  	  while (!syncList.empty())
       	       syncList.pop_front();
 }
  
  list<AstNode*>::iterator i;

  
  for(i = node->begin(); i != node->end(); i++) {
    
   int kind = (*i)->getType();
   AstNode* foundNode = (*i);
   bool iRet = CheckSyncThread(foundNode); 
   if (iRet)
   {
      //fprintf(fp,"\nnode  %s",nodeNames[kind]);
      syncList.push_front(*i);
   }
    findSyncThreadRef(*i,1);
 }
  list<AstNode*> *children = node->getChildren();
  
  for(i = children->begin(); i != children->end(); i++) {
     
     int kind = (*i)->getType();
     AstNode* foundNode = (*i);
     bool iRet = CheckSyncThread(foundNode); 
     if (iRet)
     {
        //fprintf(fp,"\nnode  %s",nodeNames[kind]); 
        syncList.push_front(*i);
     }
      findSyncThreadRef(*i,1);    
  }
  
  //fclose(fp);
  return syncList;

}



