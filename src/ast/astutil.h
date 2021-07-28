#ifndef ASTUTIL_H
#define ASTUTIL_H

#include<ast/ast.h>

#define TYPEINT 9

bool isVarDefInStmt(int var, AstNode *stmt);
bool isVarRefInStmt(int var, AstNode *stmt);

void replaceVar(AstNode *node, int oldVar, int newVar);
AstNode *findFirstVarRef(AstNode *node, int var);
AstNode *makeVarDeclAssignNode(AstNode *lhs, AstNode *rhs, int type);

AstNode *getNextStmt(AstNode *subtree);
void removeStmt(AstNode *subtree);

void insertStmtAfter(AstNode *node, AstNode *subtree);
void insertStmtBefore(AstNode *node, AstNode *subtree);
AstNode *getNextStmt(AstNode *subtree);

void traverseSubtree(AstNode *node);
void replaceVarSubtree(AstNode *node, int oldVar, int newVar);


bool isScalarRef(AstNode *node);
bool isStructRef(AstNode *node);
bool isArrayRef(AstNode *node);
bool isIdRef(AstNode *node);


bool idMatch(AstNode *node, const char *id);

AstNode *makeAssignNode(AstNode *lhs, AstNode *rhs);
AstNode *makeRelExprNode(AstNode *lhs, AstNode *rhs, int relOp);

AstNode* makeSimpleForLoopNode(const char *indexVar, int iters);

AstNode* makeLoopBodyFromStmt(AstNode *firstStmt);
void setLoopBody(AstNode *loop, AstNode *body);

AstNode *makeAssignAddNode(AstNode *lhs, AstNode *rhs);

AstNode *makeLoopBodyFromStmtToStmt(AstNode *stmt,AstNode *syncNode, int increment);
AstNode* makeThreadCoarseningForLoopNode(const char *indexVar, int iters, int increment);

#endif
