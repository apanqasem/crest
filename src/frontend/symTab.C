#include<string.h>
#include<symTab.h>


SymTab::SymTab() {
}

SymTab::~SymTab() {
}


unsigned int SymTab::insertStr(const char *name) {
    symbol *thisSym = (symbol *) malloc(sizeof(symbol));


    thisSym->name = (char *) malloc(sizeof(char) * strlen(name) + 1);
    strncpy(thisSym->name, name, strlen(name));
    thisSym->name[strlen(name)] = '\0';

    thisSym->scope = 0;

    symbols.push_back(thisSym);

    return (symbols.size() - 1);
}

unsigned int SymTab::insert(char *name) {
    symbol *thisSym = (symbol *) malloc(sizeof(symbol));


    thisSym->name = (char *) malloc(sizeof(char) * strlen(name) + 1);
    strncpy(thisSym->name, name, strlen(name));
    thisSym->name[strlen(name)] = '\0';

    thisSym->scope = 0;

    symbols.push_back(thisSym);

    return (symbols.size() - 1);
}


char *SymTab::lookup(int index) {
    return symbols[index]->name;
}

int SymTab::lookupByName(const char *var) {
  
  for (int i = 0; i < symbols.size(); i++) {
      if (!strcmp(var, symbols[i]->name))
	return i;
  }
  return -1;
}
