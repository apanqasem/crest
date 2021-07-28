#include<string.h>
#include<astutil.h>

#include<frontend/symTab.h>
#include<driver/errors.h>

extern SymTab *symtab; 


bool isScalarRef(AstNode *node) { return (node->getType() == SCALAR_REF); }
bool isStructRef(AstNode *node) { return (node->getType() == STRUCT_REF); }
bool isArrayRef(AstNode *node) { return (node->getType() == ARRAY_REF); }
bool isIdRef(AstNode *node) { return (node->getType() == ID_REF); }


bool idMatch(AstNode *node, const char *id) {
  if (!isIdRef(node))
    return false;
  int index = node->getVal();
  if (!strcmp(symtab->lookup(index), id))
    return true;
  else 
    return false;
}


AstNode *makeIncDecNode(AstNode *var, bool inc) {
  
  AstNode *assignNode;
  if (inc) {
    assignNode = new AstNode(PREINCSTMT);
  }
  else {
    assignNode = new AstNode(PREDECSTMT);
  }
  assignNode->addSibling(var);
  return assignNode;  
}


AstNode *makeAssignNode(AstNode *lhs, AstNode *rhs) {
  AstNode *assignNode = new AstNode(ASSIGN);
  assignNode->addSibling(lhs);
  assignNode->addSibling(rhs);
  return assignNode;
}

AstNode *makeAssignAddNode(AstNode *lhs, AstNode *rhs) {
  AstNode *assignAddNode = new AstNode(ASSIGNADD);
  assignAddNode->addSibling(lhs);
  assignAddNode->addSibling(rhs);
  return assignAddNode;
}

AstNode *makeVarDeclAssignNode(AstNode *lhs, AstNode *rhs, int type) {
  AstNode *assignNode = new AstNode(VARDECLASSIGN);
  AstLeafNode *typeNode = new AstLeafNode(TYPE, type);
  assignNode->addSibling(typeNode);
  assignNode->addSibling(lhs);
  assignNode->addSibling(rhs);
  return assignNode;
}

AstNode *makeRelExprNode(AstNode *lhs, AstNode *rhs, nodeType relOp) {
  AstNode *relExprNode = new AstNode(relOp);
  relExprNode->addSibling(lhs);
  relExprNode->addSibling(rhs);
  return relExprNode;
}

AstNode* makeForLoopHeaderNode(AstNode* initNode, AstNode* compNode, AstNode* incNode) {
  AstNode *loopNode = new AstNode(LOOP);

  AstNode *assignStmtListNode = new AstNode(ASSIGNLIST);
  assignStmtListNode->addSibling(initNode);
  loopNode->addSibling(assignStmtListNode);
  loopNode->addSibling(compNode);
  
  assignStmtListNode = new AstNode(ASSIGNLIST);
  assignStmtListNode->addSibling(incNode);
  loopNode->addSibling(assignStmtListNode);

  return loopNode;
}


AstNode* makeThreadCoarseningForLoopHeaderNode(AstNode* initNode, AstNode* compNode, AstNode* incNode, AstNode* assignAddNode) {
  AstNode *loopNode = new AstNode(LOOP);

  AstNode *assignStmtListNode = new AstNode(ASSIGNLIST);
  assignStmtListNode->addSibling(initNode);
  loopNode->addSibling(assignStmtListNode);
  loopNode->addSibling(compNode);
  
  assignStmtListNode = new AstNode(ASSIGNLIST);
  assignStmtListNode->addSibling(incNode);
  assignStmtListNode->addSibling(assignAddNode);
  loopNode->addSibling(assignStmtListNode);
  return loopNode;
}

AstNode* makeSimpleForLoopNode(const char *indexVar, int iters) {
  int stIndex = symtab->lookupByName(indexVar);
  if (stIndex < 0) 
    stIndex = symtab->insertStr(indexVar); 
  
  AstNode *inVarNode = new AstLeafNode(ID_REF, stIndex);
  AstNode *rhs = new AstLeafNode(INT_REF, 0);
  
  AstNode *initNode = makeVarDeclAssignNode(inVarNode, rhs, TYPEINT);
  
  inVarNode = new AstLeafNode(ID_REF, stIndex);
  rhs = new AstLeafNode(INT_REF, iters);

  AstNode *compNode = makeRelExprNode(inVarNode, rhs, LTEXPR);

  inVarNode = new AstLeafNode(ID_REF, stIndex);
  AstNode *incNode = makeIncDecNode(inVarNode, 1);

  AstNode *header = makeForLoopHeaderNode(initNode, compNode, incNode);
  

  AstNode *body = new AstNode(LOOPBODY);
  body->addChild(new AstLeafNode(DUMMY, 0));

  header->addSibling(body);
  return header;
}

AstNode* makeThreadCoarseningForLoopNode(const char *indexVar, int iters, int increment) {
  int stIndex = symtab->lookupByName(indexVar);
  if (stIndex < 0) 
    stIndex = symtab->insertStr(indexVar); 

  int incIndex = symtab->lookupByName("__i");

  AstNode *inVarNode = new AstLeafNode(ID_REF, stIndex);
  AstNode *rhs = new AstLeafNode(INT_REF, 0);           
  
  AstNode *initNode = makeVarDeclAssignNode(inVarNode, rhs, TYPEINT);
  
  inVarNode = new AstLeafNode(ID_REF, stIndex);
  rhs = new AstLeafNode(INT_REF, iters);       

  AstNode *compNode = makeRelExprNode(inVarNode, rhs, LTEXPR);

  inVarNode = new AstLeafNode(ID_REF, stIndex);
  AstNode *incNode = makeIncDecNode(inVarNode, 1);

  inVarNode = new AstLeafNode(ID_REF, incIndex);
  rhs = new AstLeafNode(INT_REF, increment);           
  
  AstNode *assignAddNode = makeAssignAddNode(inVarNode, rhs);

  AstNode *header = makeThreadCoarseningForLoopHeaderNode(initNode, compNode, incNode, assignAddNode);
  

  AstNode *body = new AstNode(LOOPBODY);   
  body->addChild(new AstLeafNode(DUMMY, 0));

  header->addSibling(body);                             
  return header;
}


bool isVarDefInStmt(int varId, AstNode *stmt) {
  if (!stmt->isStmtNode())
    error("Not a statement node : ", PROGRAMMER);
  if (stmt->isCompStmtNode())
    error("Compound statement, not processing : ", PROGRAMMER);
  
  int kind = stmt->getType();
  if (kind == VARDECL || kind == ARRDECL || kind == RET)
    return false;

  if (kind == ASSIGN || kind == ASSIGNADD) {
    AstNode *varNode = (*stmt)[0];
    if (isScalarRef(varNode))
      return (varId == varNode->getVal());
    else if (isStructRef(varNode) || isArrayRef(varNode)) {
      return ((*varNode)[0]->getVal() == varId);
    }
  }
  
}
  
void replaceVar(AstNode *node, int oldVar, int newVar) {

  if (isIdRef(node) || isScalarRef(node)) {
    if (node->getVal() == oldVar)
      node->setVal(newVar);
    return;
  }
  
  list<AstNode*>::iterator i;

  for(i = node->begin(); i != node->end(); i++)
    replaceVar(*i, oldVar, newVar);
  
  list<AstNode*> *children = node->getChildren();
  for(i = children->begin(); i != children->end(); i++)
    replaceVar(*i, oldVar, newVar);

  return;
}

void replaceVarSubtree(AstNode *node, int oldVar, int newVar) {
  AstNode *parent = node->getParent();
  if (!parent)
    return;
  
  for (int i = node->getChildNum() + 1; i < parent->getNumChildren(); i++) {
    AstNode *child = parent->getChild(i);
    replaceVar(child, oldVar, newVar);
  }
  replaceVarSubtree(parent, oldVar, newVar);
}

void setLoopBody(AstNode *loop, AstNode *body) {
  loop->replaceSibling(body, 3);
  loop->addSibling(body);
}

/*
 * create loop body from a statement list and return body 
 * 
 * originial statement list is destroyoed, so this routine in effect does a "move"
 * 
 * argument is expected to point to the first stmt in the list 
 */ 
AstNode *makeLoopBodyFromStmt(AstNode *stmt) {

  AstNode *body = new AstNode(LOOPBODY);
  AstNode *newStmt;
  
  AstNode *firstStmt = stmt;

  AstNode *parent = stmt->getParent();
  if (!parent)
    return NULL;
  int i = stmt->getChildNum();
  list<AstNode*>::iterator iter;

  int j = 0; 

  /* copy stmts from list into body, this is a deep copy */
  for(iter = (parent->getChildren())->begin(); j < i; iter++, j++);
  for(; iter != (parent->getChildren())->end(); iter++) {
    newStmt = (*iter)->deepCopy();    
    body->addChild(newStmt);
  }


  /* remove old list */
  j = 0; 
  for(iter = (parent->getChildren())->begin(); j < i; iter++, j++);
  int childIndex = j;
  int childrenToRemove = parent->getNumChildren() - j + 1;
  for(int k = 0; k < childrenToRemove; k++) {
    parent->removeChild(childIndex);
  }

  return body;
}

AstNode *makeLoopBodyFromStmtToStmt(AstNode *stmt,AstNode *syncNode, int syncNo) {

  //FILE * fp = fopen("SyncData7.txt","a");

  AstNode *body = new AstNode(LOOPBODY);
  AstNode *newStmt;

  AstNode *firstStmt = stmt;

  AstNode *parent = stmt->getParent();
  if (!parent)
    return NULL;
  int i = stmt->getChildNum();
  list<AstNode*>::iterator iter;
  //fprintf(fp,"\n iPos = %d", i);
  int j = 0;
  int t = syncNo;

  //fprintf(fp,"\n t Pos = %d", t);
  /* copy stmts from list into body, this is a deep copy */
  for(iter = (parent->getChildren())->begin(); j < i; iter++, j++);
  //fprintf(fp,"\n j Pos = %d", j);


 int childcount = 0;
 for(; iter != (parent->getChildren())->end(); iter++) {
      if ( ( syncNo + 2) != (*iter)->getChildNum() ){
   
	    newStmt = (*iter)->deepCopy();
	    body->addChild(newStmt);
	    childcount++;
	    }
	    else
		break;
      }

	  /* remove old list */
	  j = 0;
	  for(iter = (parent->getChildren())->begin(); j < i; iter++, j++);
	  int childIndex = j;
	  int childrenToRemove = childcount;
	  for(int k = 0; k < childrenToRemove; k++) {
	    parent->removeChild(childIndex);
  }

  //fprintf(fp,"\n childrenToRemove = %d", childrenToRemove);
  //fclose(fp);
  return body;
}


void insertStmtAfter(AstNode *node, AstNode *subtree) {
  AstNode *parent = subtree->getParent();
  if (!parent)
    return;
  int i = subtree->getChildNum() + 1;
  parent->insertChild(node, i);
  
  return;
}

void insertStmtBefore(AstNode *node, AstNode *subtree) {
  AstNode *parent = subtree->getParent();
  if (!parent)
    return;
  int i = subtree->getChildNum();
  parent->insertChild(node, i);
  
  return;
}

void removeStmt(AstNode *subtree) {
  AstNode *parent = subtree->getParent();
  if (!parent)
    return;
  int i = subtree->getChildNum();
  printf("removing child no %d\n", i);
  parent->removeChild(i);
}

AstNode *getNextStmt(AstNode *subtree) {
  AstNode *parent = subtree->getParent();
  if (!parent)
    return NULL;
  int i = subtree->getChildNum();
#ifdef DEBUG
  printf("child %d ***\n", i);
  printf("children  %d ***\n",  parent->getNumChildren());
#endif
  if (i >= parent->getNumChildren() - 1)
    return NULL;
  else 
    return parent->getChild(i + 1);
}


AstNode *findFirstVarRef(AstNode *node, int var) {

  if (isIdRef(node) || isScalarRef(node)) {
    if (node->getVal() == var)
      return node;
  }
  
  list<AstNode*>::iterator i;

  AstNode *foundNode;
  for(i = node->begin(); i != node->end(); i++) {
    foundNode = findFirstVarRef(*i, var);
    if (foundNode) 
      return foundNode;
  }

  list<AstNode*> *children = node->getChildren();
  for(i = children->begin(); i != children->end(); i++) {
    foundNode = findFirstVarRef(*i, var);
    if (foundNode) 
      return foundNode;
  }

  return NULL;
}

void traverseSubtree(AstNode *node) {
  AstNode *parent = node->getParent();
  if (!parent)
    return;
  
  //  process children starting at current child
  for (int i = node->getChildNum(); i < parent->getNumChildren(); i++) {
    AstNode *child = parent->getChild(i);
    printf("%s\n", child->getTypeStr());
  }

  traverseSubtree(parent);
}
