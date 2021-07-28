include ../Makeinclude.config

SCAN_DBUG= #-DSCAN_DBUG   -DSTRTAB_DBUG
PARSE_DBUG= #-DP_TREE -DPARSE_DBUG 

OBJS=$(OBJ_PATH)/lex.yy.o $(OBJ_PATH)/symTab.o $(OBJ_PATH)/cuda.tab.o $(OBJ_PATH)/varTab.o 

all: $(OBJS)

$(EXE): 
	cd $(TOP_SRC_PATH); $(MAKE)

$(OBJ_PATH)/cuda.tab.o: cuda.tab.c cuda.h
	$(CXX) $(CXXFLAGS) $(PARSE_DBUG) -c cuda.tab.c -o $(OBJ_PATH)/cuda.tab.o

cuda.tab.h: cuda.y
	bison -v -d -b cuda cuda.y 

cuda.tab.c: cuda.y
	bison -v -d -b cuda cuda.y 

$(OBJ_PATH)/lex.yy.o: lex.yy.c cuda.h
	$(CXX) $(CXXFLAGS) $(SCAN_DBUG) -c lex.yy.c -o $(OBJ_PATH)/lex.yy.o

lex.yy.c: cuda.l cuda.tab.h
	flex $(SCAN_DBUG) cuda.l

$(OBJ_PATH)/symTab.o: symTab.C symTab.h 
	$(CXX) $(CXXFLAGS) -c symTab.C -o $(OBJ_PATH)/symTab.o

$(OBJ_PATH)/varTab.o: varTab.C varTab.h
	$(CXX) $(CXXFLAGS) -c varTab.C -o $(OBJ_PATH)/varTab.o

clean: clean_all

clean_all:
	rm -f $(EXE_PATH)/$(EXE) $(OBJS) cuda.tab.* lex.yy.c cuda.output *~
