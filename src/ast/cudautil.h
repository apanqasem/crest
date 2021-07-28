#ifndef CUDAUTIL_H
#define CUDAUTIL_H

#include<ast/ast.h>


AstNode *findThreadIdFirstRef(AstNode *tree, int dim);
const char* dimToAxis(int dim);
bool isThreadId(AstNode *node, int dim);
AstNode *creatThreadIdRef(int dim);
int lookupThreadIdinST(int dim);

list<AstNode*>findSyncThreadRef(AstNode *node,int test);
#endif
