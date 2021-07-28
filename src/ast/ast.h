#ifndef AST_H
#define AST_H

#include<list>

#define DUMMYVAL 0

using namespace std;

/* order must match with nodeNames[] in astdefs.h */
typedef enum NODE_TYPE {DUMMY,
			KERNEL,
			FUNNAMETYPE, FUNHEAD, FORMALDECL, FUNBODY,
			TYPE, IDTYPE, QUAL, STATICNODE, PTRNODE,
            PREINCSTMT, POSTINCSTMT, PREDECSTMT, POSTDECSTMT,
			LOOP, DWLOOP, LOOPBODY, DWLOOPBODY, COND, CONDBODY, COMPOUND,
			VARDECL, VARDECLASSIGN, ARRDECL, ARRDECLASSIGN,
			VARDECLS, VARDECLLIST, EXTERNDECL, EXTERNARRDECL,
			ASSIGN, ASSIGNADD, ASSIGNSUB, ASSIGNMUL, ASSIGNDIV, ASSIGNMOD,
			ASSIGNRS, ASSIGNLS, ASSIGNAND, ASSIGNOR, ASSIGNXOR, RET,
			FORMALLIST, STMTLIST, /*ASSIGNCHAIN,*/ ASSIGNLIST, ARGLIST,
			EXPR, CONDEXPR,
			LTEEXPR, LTEXPR, GTEEXPR, GTEXPR, EQEXPR, NEQEXPR,
			ADDEXPR, SUBEXPR, MULEXPR, DIVEXPR, MODEXPR,
			FUNCALL, /*KERNELCALL,*/ NEGEXPR, NOTEXPR, DEREFEXPR, ADDRESSOFEXPR,
			SCALAR_REF, ARRAY_REF, STRUCT_REF, ID_REF,
			INT_REF, FLOAT_REF, CHAR_REF, STR_REF,
			ARRAY_INITLIST, ARRAY_INIT,
            LOGANDEXPR, LOGOREXPR, BITANDEXPR, BITOREXPR, BITXOREXPR, SHIFTLEFTEXPR, SHIFTRIGHTEXPR,
            PARENEXPR, TYPECASTEXPR}
  nodeType;

class AstNode  : public list<AstNode *>  {

public:

  AstNode();
  AstNode(nodeType type);
  
  virtual AstNode *deepCopy();

  virtual ~AstNode();
  
  void addChild(AstNode *child);
  void removeChild(int i);
  void insertChild(AstNode *child, int index);

  void addSibling(AstNode *sibling);
  void removeSibling(int i);
  void replaceSibling(AstNode *newSib, int oldIndex);

  void setParent(AstNode *_parent) { parent = _parent; }
  AstNode *getParent() {return parent;}
  

  nodeType getType() {return type;}
  const char* getTypeStr();
  void setType(nodeType _type) { type = _type;}
  
  int getNumChildren() {return children.size();}
  list<AstNode*> *getChildren() {return &children;}
  
  AstNode* getChild(int i);
  int getChildNum() {return childNum;}
  void setChildNum(int i) { childNum = i;}
  
  int getNumSiblings() {return this->size();}
  


  bool isStmtNode() {return (type == LOOP ||
			     type == DWLOOP || 
			     type == COND || 
			     type == COMPOUND ||
			     type == ASSIGN || 
			     type == ASSIGNADD || 
			     type == ASSIGNSUB|| 
			     type == ASSIGNMUL || 
			     type == ASSIGNDIV || 
			     type == ASSIGNMOD || 
			     type == ASSIGNRS || 
			     type == ASSIGNLS || 
			     type == ASSIGNAND || 
			     type == ASSIGNOR || 
			     type == ASSIGNXOR || 
			     type == VARDECL ||
			     type == VARDECLASSIGN || 
			     type == ARRDECL ||
			     type == ARRDECLASSIGN ||
			     type == EXTERNDECL ||
			     type == EXTERNARRDECL ||
			     type == RET);}

  bool isCompStmtNode() { return (type == LOOP ||
				  type == DWLOOP || 
				  type == COND || 
				  type == COMPOUND);}
  
  bool isLoopNode() { return (type == LOOP || type == DWLOOP); }
  bool isCondNode() { return (type == COND); }
  bool isStmtListNode() {return type == STMTLIST;}  
  bool isArgListNode() {return type == ARGLIST;}  
  

  bool isFormalDeclNode() {return type == FORMALDECL;}
  bool isFormalListNode() {return type == FORMALLIST;}  
  
  
  bool isAssignNode() { return (type == ASSIGN
                                || type == ASSIGNADD
                                || type == ASSIGNSUB
                                || type == ASSIGNMUL
                                || type == ASSIGNDIV
                                || type == ASSIGNMOD
                                || type == ASSIGNRS
                                || type == ASSIGNLS
                                || type == ASSIGNAND
                                || type == ASSIGNOR
                                || type == ASSIGNXOR);}
  bool isAssignStmtListNode() { return type == ASSIGNLIST;}
  /*bool isAssignStmtChainNode() { return type == ASSIGNCHAIN;}*/
  bool isVarDeclNode() { return (type == VARDECL || 
				 type == VARDECLASSIGN ||
				 type == ARRDECL ||
                 type == ARRDECLASSIGN);}

  bool isReferenceNode() {return (type == SCALAR_REF ||
                                    type == ARRAY_REF ||
                                    type == STRUCT_REF ||
                                    type == ID_REF ||
                                    type == INT_REF ||
                                    type == FLOAT_REF ||
                                    type == CHAR_REF ||
                                    type == STR_REF);}
  
  bool isVarNode() {return (type == SCALAR_REF ||
                                    type == ARRAY_REF ||
                                    type == STRUCT_REF);}
                                    
  bool isUnaryOpNode() {return (type == NEGEXPR ||
                                    type == NOTEXPR ||
                                    type == DEREFEXPR ||
                                    type == ADDRESSOFEXPR ||
                                    type == PREINCSTMT ||
                                    type == POSTINCSTMT ||
                                    type == PREDECSTMT ||
                                    type == POSTDECSTMT);}

  bool isBinaryOpNode() {return (type == LTEEXPR || type == LTEXPR ||
                                    type == GTEEXPR || type == GTEXPR ||
                                    type == EQEXPR || type == NEQEXPR ||
                                    type == ADDEXPR || type == SUBEXPR ||
                                    type == MULEXPR || type == DIVEXPR ||
                                    type == MODEXPR ||
                                    type == LOGANDEXPR || type == LOGOREXPR ||
                                    type == BITANDEXPR || type == BITOREXPR || type == BITXOREXPR ||
                                    type == SHIFTLEFTEXPR || type == SHIFTRIGHTEXPR);}

  bool isTernaryOpNode() {return (type == CONDEXPR);}

  bool isFunCallNode() { return type == FUNCALL;}

  AstNode * operator[] (const int index);

  virtual bool isLeafNode(); 
  virtual int getVal() { return 0;} 
  virtual double getFVal() {return 0.0;}
  virtual void setVal(int _val) { return;} 
  virtual void setFVal(double _fval) { return;} 
  
private:
  nodeType type;
  AstNode *parent;
  list<AstNode *> children;
  int childNum;
  
};


class AstLeafNode : public AstNode {
  

public:

  friend class AstNode;

  AstLeafNode();
  AstLeafNode(nodeType type);
  AstLeafNode(nodeType type, int val);
  AstLeafNode(nodeType type, double fval);
  AstLeafNode(nodeType thisNodeType, int _val,double _fval);

  virtual ~AstLeafNode();

  virtual AstLeafNode *deepCopy();
  virtual bool isLeafNode();
  virtual int getVal() {return val;}
  virtual double getFVal() {return fval;}
  virtual void setVal(int _val) {val = _val;} 
  virtual void setFVal(double _fval) {fval = _fval;} 

private:
  int val;
  double fval;
};




#endif 
