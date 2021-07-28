#ifndef VARTAB_H
#define VARTAB_H

#include <stdlib.h>
#include <stdio.h>
#include <vector>


using namespace std;


typedef struct variableType {
  char *name;
  char *dataType;
  int scope;
  int readCount;
  int copyVar;
} varTable;
 
class VarTab {

public:

  VarTab();

  ~VarTab();

  unsigned int insert(char *name,char *type,int scope);
  char *lookup(int index);
  int getReadCount(int index);
  int getCopyVar(int index);
  char * getScope(int index);
  char * getDataType(int index);
  int getSize();
  int lookupByName(char *);
  void incReadCount(char *var, int i);
  void setCopyVar(char *rhsVar, char * lhsVar);
  unsigned int insertID(char *name);  
  
private:
  
  vector<varTable *> varList;
    

};


#endif
