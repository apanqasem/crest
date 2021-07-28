include ../Makeinclude.config

SSA_DBUG= #-DSSA_DBUG
CFG_DBUG= #-DCFG_DBUG

OBJS=$(OBJ_PATH)/line.o $(OBJ_PATH)/cfg.o $(OBJ_PATH)/ssa.o $(OBJ_PATH)/genKernel.o $(OBJ_PATH)/genStmts.o $(OBJ_PATH)/genExprs.o $(OBJ_PATH)/genRefs.o $(OBJ_PATH)/extractAST.o $(OBJ_PATH)/featureVector.o

all: $(OBJS)

$(EXE): 
	cd $(TOP_SRC_PATH); $(MAKE)

$(OBJ_PATH)/line.o: line.c line.h
	$(CXX) $(CXXFLAGS) -c line.c -o $(OBJ_PATH)/line.o

$(OBJ_PATH)/cfg.o: cfg.C cfg.h 
	$(CXX) $(CXXFLAGS) $(CFG_DBUG) -c cfg.C -o $(OBJ_PATH)/cfg.o

$(OBJ_PATH)/ssa.o: ssa.C ssa.h
	$(CXX) $(CXXFLAGS) $(SSA_DBUG) -c ssa.C -o $(OBJ_PATH)/ssa.o

$(OBJ_PATH)/genKernel.o: genKernel.C genILOC.h
	$(CXX) $(CXXFLAGS) -c genKernel.C -o $(OBJ_PATH)/genKernel.o

$(OBJ_PATH)/genStmts.o: genStmts.C genILOC.h
	$(CXX) $(CXXFLAGS) -c genStmts.C -o $(OBJ_PATH)/genStmts.o

$(OBJ_PATH)/genExprs.o: genExprs.C genILOC.h
	$(CXX) $(CXXFLAGS) -c genExprs.C -o $(OBJ_PATH)/genExprs.o

$(OBJ_PATH)/genRefs.o: genRefs.C genILOC.h
	$(CXX) $(CXXFLAGS) -c genRefs.C -o $(OBJ_PATH)/genRefs.o

$(OBJ_PATH)/extractAST.o: extractAST.C extractAST.h
	$(CXX) $(CXXFLAGS) -c extractAST.C -o $(OBJ_PATH)/extractAST.o

$(OBJ_PATH)/featureVector.o: featureVector.C featureVector.h
	$(CXX) $(CXXFLAGS) -c featureVector.C -o $(OBJ_PATH)/featureVector.o

clean: clean_all

clean_all:
	rm -f $(EXE_PATH)/$(EXE) $(OBJS) *~
