include ../Makeinclude.config

OBJS = $(OBJ_PATH)/ast.o  $(OBJ_PATH)/astutil.o $(OBJ_PATH)/cudautil.o $(OBJ_PATH)/prettyPrint.o $(OBJ_PATH)/calUsage.o

all : $(OBJS)

$(EXE): 
	cd $(TOP_SRC_PATH); $(MAKE)

$(OBJ_PATH)/%.o: %.C
	$(CXX) -c $(CXXFLAGS) $< -o $@
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@





clean: clean_all

clean_all:
	rm -f $(EXE_PATH)/$(EXE) $(OBJS) *~
