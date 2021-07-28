#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdlib.h>
#include <stdio.h>
#include <vector>


using namespace std;


typedef struct symbolType {
  char *name;
  int scope;
} symbol;
 
class SymTab {

public:

  SymTab();

  ~SymTab();

  unsigned int insert(char *name);
  unsigned int insertStr(const char *name);
  char *lookup(int index);
  int lookupByName(const char *);

private:
  
  vector<symbol *> symbols;
  

};


#endif
