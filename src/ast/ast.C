#include<stdlib.h>
#include<stdio.h>

#include<ast.h>
#include<astdefs.h>

#include<driver/errors.h>

static void updateChildInfoRecursively(AstNode *child, AstNode *parent, int _childNum) {
  list<AstNode*>::iterator iter;
  for (iter = child->begin(); iter != child->end(); iter++) {
    (*iter)->setParent(parent);
    (*iter)->setChildNum(_childNum);
    updateChildInfoRecursively((*iter), parent, _childNum);
  }
  return;
}

AstNode::AstNode() 
  : type(DUMMY), parent(0), childNum(-1) {
}

AstNode::AstNode(nodeType thisNodeType) 
  : type(thisNodeType), parent(0), childNum(-1) {
}

AstNode* AstNode::deepCopy() {

  AstNode *newNode = new AstNode();
  newNode->type = type;
  newNode->parent = parent;
  newNode->childNum = childNum;
  
  list<AstNode*>::iterator iter;
  for (iter = begin(); iter != end(); iter++) {
    newNode->addSibling((*iter)->deepCopy());
  }
  
  for (iter = children.begin(); iter != children.end(); iter++) {
    newNode->addChild((*iter)->deepCopy());
  }
  return newNode;
}


AstNode::~AstNode() {
  printf("AstNode Destructor Called\n");
  list<AstNode*>::iterator iter;
  for (iter = begin(); iter != end(); iter++) {
    printf("AstNode Destructor Called : siblings\n");
    delete (*iter);
  }
  
  for (iter = children.begin(); iter != children.end(); iter++) {
    printf("AstNode Destructor Called : children\n");
    delete (*iter);
  }
}

void AstNode::addChild(AstNode *child) {
  if (children.max_size() == children.size()) {
    printf("Cannot add any more nodes, exiting ...\n");
    exit(1);
  }

  children.push_back(child);
  child->setParent(this);
  child->setChildNum(children.size() - 1);
  updateChildInfoRecursively(child, this, children.size() - 1);
}

void AstNode::removeChild(int index) {
  if (index >= children.size()) 
    return;
  
  list<AstNode*>::iterator iter;
  int i;

  for (i = 0, iter = children.begin(); i < index; iter++, i++);
  erase(iter);

}



void AstNode::insertChild(AstNode *child, int index) {

  if (index >= children.size()) 
    addChild(child);

  list<AstNode*>::iterator iter;
  int i;

  for (i = 0, iter = children.begin(); i < index; iter++, i++);

  children.insert(iter, 1,  child);
  child->setParent(this);
  child->setChildNum(index);
  
  for (index++; iter != children.end(); iter++, index++) {
#ifdef DEBUG
    printf("index = %d\n", index);
#endif
    (*iter)->setChildNum(index);
  }

  updateChildInfoRecursively(child, this, index);

}

void AstNode::replaceSibling(AstNode *newSib, int index) {
  if (index >= size()) 
    addSibling(newSib);
  
  list<AstNode*>::iterator iter;
  int i;

  for (i = 0, iter = begin(); i < index; iter++, i++);
   
  erase(iter);
  insert(iter, newSib);
    
}

void AstNode::addSibling(AstNode *sibling) {
  push_back(sibling);
  sibling->setParent(parent);
  sibling->setChildNum(childNum);
}

void AstNode::removeSibling(int index) {
  if (index >= size()) 
    return;
  
  list<AstNode*>::iterator iter;
  int i;

  for (i = 0, iter = begin(); i < index; iter++, i++);
  
  // assuming erase calls AstNode destructor
  erase(iter);
}

const char* AstNode::getTypeStr() {
    return nodeNames[type];
}


bool AstNode::isLeafNode() {
  return false;
} 

AstNode* AstNode::getChild(int _childNum) {
    if (_childNum == children.size() || _childNum < 0)
      error("Illegal child request", PROGRAMMER);
	
    list<AstNode*>::iterator iter;
    int i = 0;
    for(iter = children.begin(); i < _childNum; iter++, i++);
    return *iter;
}

AstNode* AstNode::operator[] (const int index) {
    list<AstNode*>::iterator iter;
    unsigned int i = 0;
    for (iter = begin(); i < index; iter++, i++);
    return *iter;
}


AstLeafNode* AstLeafNode::deepCopy() {

  AstLeafNode *newNode = new AstLeafNode(getType(),getVal(),getFVal());
  newNode->setParent(getParent());
  newNode->setChildNum(getChildNum());
  
  list<AstNode*>::iterator iter;
  for (iter = begin(); iter != end(); iter++) {
    newNode->addSibling((*iter)->deepCopy());
  }
  return newNode;
}

AstLeafNode::AstLeafNode(nodeType thisNodeType) :
  AstNode(thisNodeType) {
}

AstLeafNode::AstLeafNode(nodeType thisNodeType, int _val) :
  AstNode(thisNodeType), val(_val) {
}


AstLeafNode::AstLeafNode(nodeType thisNodeType, double _fval) :
  AstNode(thisNodeType), fval(_fval) {
}

AstLeafNode::AstLeafNode(nodeType thisNodeType, int _val,double _fval) :
  AstNode(thisNodeType), val(_val),fval(_fval) {
}

AstLeafNode::~AstLeafNode() {}

bool AstLeafNode::isLeafNode()  {
  return true;
} 

