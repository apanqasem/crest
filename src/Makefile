include Makeinclude.config

SUBDIRS = driver frontend ast extraction

OBJS = ../obj/*.o

all: $(EXE_PATH)/$(EXE)

$(EXE_PATH)/$(EXE)::
	@for i in $(SUBDIRS) ; do \
	    cd "$$i" && $(MAKE); \
            cd ..; \
	done
	$(CXX) -o $(EXE_PATH)/$(EXE) $(CXXFLAGS) $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	@for i in $(SUBDIRS) ; do \
	    cd "$$i" && $(MAKE) clean; \
            cd ..; \
	done

install: $(EXE_PATH)/$(EXE)
	cp $(EXE_PATH)/$(EXE) $(INSTALL_PATH)/$(EXE)

test: $(EXE_PATH)/$(EXE)
	$(EXE_PATH)/test_crest.sh


