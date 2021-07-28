include ../Makeinclude.config


OBJS = $(OBJ_PATH)/main.o  $(OBJ_PATH)/errors.o  $(OBJ_PATH)/coarsening.o

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
