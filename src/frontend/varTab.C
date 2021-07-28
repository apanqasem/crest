#include<string.h>
#include "varTab.h"

#define MAXKEYWORD 10 

const char *keyword[MAXKEYWORD] = { "blockIdx","threadIdx","__syncthreads","gridDim","blockDim" };

VarTab::VarTab() {
}

VarTab::~VarTab() {
}



unsigned int VarTab::insert(char *name, char *type, int scope) {

    bool flag = 1;
    for ( int i = 0; i <5; i ++ )
    { 
	if ( !strncmp(name,keyword[i],7) ){
		flag = 0;
		break;
	}
    }
    if ( flag ) {
    varTable *thisVar = (varTable *) malloc(sizeof(varTable));


    thisVar->name = (char *) malloc(sizeof(char) * strlen(name) + 1);
    strncpy(thisVar->name, name, strlen(name));
    thisVar->name[strlen(name)] = '\0';

    thisVar->scope = scope;

       //if(strlen(type)>0){
    thisVar->dataType = (char *) malloc(sizeof(char) * strlen(type) + 1);
    strncpy(thisVar->dataType, type, strlen(type));
    thisVar->dataType[strlen(type)] = '\0';
    //}
    thisVar->readCount = 0;
    thisVar->copyVar = -1;
    varList.push_back(thisVar);
    }
    
    return (varList.size() - 1);
}

unsigned int VarTab::insertID(char *name) {

    
    varTable *thisVar = (varTable *) malloc(sizeof(varTable));

     char* type ="int";

    thisVar->name = (char *) malloc(sizeof(char) * strlen(name) + 1);
    strncpy(thisVar->name, name, strlen(name));
    thisVar->name[strlen(name)] = '\0';

    thisVar->scope = 0;

       //if(strlen(type)>0){
    thisVar->dataType = (char *) malloc(sizeof(char) * strlen(type) + 1);
    strncpy(thisVar->dataType, type, strlen(type));
    thisVar->dataType[strlen(type)] = '\0';
    //}
    thisVar->readCount = 0;
    thisVar->copyVar = 0;
    varList.push_back(thisVar);
  
    
    return (varList.size() - 1);
}


char *VarTab::lookup(int index) {
    return varList[index]->name;
}

int VarTab::getReadCount(int index) {
    return varList[index]->readCount;
}

int VarTab::getCopyVar(int index) {
    return varList[index]->copyVar;
}
char* VarTab::getScope(int index) {

    char *scopeVal;

	if ( varList[index]->scope == 0 )
		scopeVal = "Register";
	else if ( varList[index]->scope == 1 )
                scopeVal = "SharedMemory";
	else if ( varList[index]->scope == 2 )
                scopeVal = "Global";
        else
                scopeVal = "Unknown";

	return scopeVal;
}

char *VarTab::getDataType(int index) {
    return varList[index]->dataType;
}
int VarTab::getSize() {
	
    return (varList.size() - 1);
}

int VarTab::lookupByName(char *var) {
  
  for (int i = 0; i < varList.size(); i++) {
      if (!strcmp(var, varList[i]->name))
	return i;
  }
  return -1;
}

void VarTab::incReadCount(char *var, int iteration){

   for (int i = 0; i < varList.size(); i++) {
      if (!strcmp(var, varList[i]->name))
      {
	if(iteration > 0)
		varList[i]->readCount = varList[i]->readCount + iteration;
        else
		varList[i]->readCount = varList[i]->readCount + 1;
      }
		//break;
   }
}

void VarTab::setCopyVar(char *rhsVar, char * lhsVar){

   int rhsIndex = lookupByName(rhsVar);      
   int lhsIndex = lookupByName(lhsVar);      

  varList[rhsIndex]->copyVar = lhsIndex;
}
