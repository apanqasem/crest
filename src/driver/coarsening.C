#include <ast/ast.h>
#include <ast/astutil.h>
#include <ast/cudautil.h>
#include <frontend/symTab.h>


void coarsening (AstNode *ast, SymTab *symtab, int coarseningFactor, int blockSize) {
    if (ast) {
        /* thread coarsening */
        AstNode *varLoc = findThreadIdFirstRef(ast, 0);
        if (varLoc) {
            int newVar = symtab->insertStr("__i");
            AstNode *idNode = new AstLeafNode(ID_REF, newVar);
       

            AstLeafNode *scalarNode = new AstLeafNode(SCALAR_REF, newVar);
            scalarNode->addSibling(idNode);
       
            AstNode *expNode = creatThreadIdRef(0);
            AstNode *typeNode = new AstLeafNode(TYPE, 9);
       
            AstNode *varDecl = new AstNode(VARDECLASSIGN);
            varDecl->addSibling(typeNode);
            varDecl->addSibling(scalarNode);
            varDecl->addSibling(expNode);	 
       
            insertStmtBefore(varDecl, varLoc);

            int oldVar = lookupThreadIdinST(0);
            replaceVarSubtree(varLoc, oldVar, newVar);

 
            //FILE *fp = fopen("SyncData6.txt","a");
            int inc = blockSize/coarseningFactor;

            list<AstNode*> sync = findSyncThreadRef(ast,0);

            list<AstNode*>::iterator i;
            list<AstNode*> syncParentList;
            int arraySize = sync.size();

            int syncChildNum[arraySize], syncParentNum[arraySize];
  
            int kind,iSync,iParent; 
            int increment = arraySize; // used in astutil.c
            //fprintf(fp,"\nSize%d",sync.size());
            int childNum = 0;    
            for(i = sync.begin(); i != sync.end(); i++) {
                kind = (*i)->getType();
                //fprintf(fp,"\nSyncNods = %s",nodeNames[kind]);
                syncParentList.push_front((*i)->getParent());
                syncChildNum[childNum] = (*i)->getChildNum();
                syncParentNum[childNum] = ((*i)->getParent())->getNumChildren();
                childNum++;
            }
            childNum--;
            for (int k = 0; k <= childNum; k++){
                //fprintf(fp,"\nSyncNods Child Num = %d",syncChildNum[k] );
                //fprintf(fp,"\nSyncNods Parent Num = %d",syncParentNum[k] );
            }
            //fclose(fp);
            //fp = fopen("SyncData6.txt","a");
            int size = sync.size();
            int orig_size = sync.size();    

            if(sync.size() > 0){  
                AstNode*  firstSync = sync.back();
                AstNode* syncParent = firstSync->getParent();
                kind = (syncParent)->getType();
                //fprintf(fp,"\nSyncNode Parent = %s",nodeNames[kind]);
                iSync = firstSync->getChildNum();
                iParent = syncParent->getNumChildren();
                //fprintf(fp,"\nSyncNode pos = %d, Total = %d",iSync,iParent);
            }

            AstNode* loop  = makeThreadCoarseningForLoopNode("__j", coarseningFactor, inc);
            insertStmtAfter(loop, varLoc);
            AstNode *firstStmt = getNextStmt(loop);
            AstNode *body;
            if(sync.size() > 0)
            {
           	    body = makeLoopBodyFromStmtToStmt(firstStmt,sync.back(),iSync);
                increment = increment + arraySize;
            }
            else
                body = makeLoopBodyFromStmt(firstStmt);

            setLoopBody(loop, body);

            int i_1 = varLoc->getChildNum();
            kind = varLoc->getType();
            //fprintf(fp,"\nvarLoc pos = %d,kind = %s",i_1,nodeNames[kind]);
            //fclose(fp);
            //fp = fopen("SyncData6.txt","a");

            //fclose(fp);
            //fp = fopen("SyncData6.txt","a");

            if ( size > 0 ){
                do{
                    //fprintf(fp,"\n Do Loop");

                    list<AstNode*> syncNode = findSyncThreadRef(ast,0);
                    //fprintf(fp,"\nDo loop SyncNode size = %d",syncNode.size());

                    size--;
                    AstNode* firstSyncNode;
                    AstNode* nextSyncNode;
                    for(int k = 0; k <= (orig_size-size); k++){
		     
                        if ( k == (orig_size-size) )
                            nextSyncNode = syncNode.back();
                        else
                            firstSyncNode = syncNode.back();
                    }

                    //fprintf(fp,"\n FirstSync = %d, NextSync = %d\n", firstSyncNode->getChildNum(),nextSyncNode->getChildNum());

                    AstNode* parent = firstSyncNode->getParent();
                    kind = (parent)->getType();
                    //fprintf(fp,"\n2. SyncNode Parent = %s",nodeNames[kind]);      
                    int iCurr = firstSyncNode->getChildNum();
                    //fprintf(fp,"\n childNum = %d",childNum);
                    //fprintf(fp,"\n iCurr = %d",iCurr);    
                    int iSync_Curr = syncChildNum[childNum];
                    int ip = parent->getNumChildren();
                    //fprintf(fp,"\n2 SyncNode pre pos = %d, Total = %d, iParent = %d",iSync_Curr,ip,iParent);
                    int diff;
                    diff  = iSync_Curr - (iParent-ip-2);
                    //fprintf(fp,"\n2 Diff = %d, Diff2 = %d\n",diff,(iSync_Curr - (iParent-ip)));
                    childNum--;         
                    
                    //fclose(fp);
                    //fp = fopen("SyncData6.txt","a");
                    
                    //fprintf(fp,"\n2 size = %d",size);
                    //fprintf(fp,"\n2. Next SyncNode  = %s",nodeNames[(nextSyncNode)->getType()]);
                    
                    AstNode* syncLoop  = makeThreadCoarseningForLoopNode("__j", coarseningFactor, inc);
                    parent->insertChild(syncLoop,diff);
                    
                    //insertStmtAfter(syncLoop, firstSyncNode);
                    
                    AstNode *firstStmt_sync = getNextStmt(syncLoop);
                    
                    AstNode *body_sync;
                    if ( size >= 1 )
                    {
                        int nxtno = syncChildNum[childNum];
                        //fprintf(fp,"\n next SyncNode ****************\t nxtno = %d\n",nxtno);
                    	body_sync = makeLoopBodyFromStmtToStmt(firstStmt_sync,nextSyncNode,(nxtno - (iParent-ip)) );
                        increment = increment + arraySize;
                    }    
                    else {
                        //fprintf(fp,"\n Else part ****************"); 
                        body_sync = makeLoopBodyFromStmt(firstStmt_sync);
                    }
                    setLoopBody(syncLoop, body_sync);
 
                }while(size > 0);
            }
            //fclose(fp);

        } // end if (varLoc)
    } // end if (ast)

} // end of coarsening()

