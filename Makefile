CC = g++-8
BIN = bin
CXXFLAGS = -std=c++17 -lstdc++fs
PROG = gmake
SUBMAKEFILES = submakefiles

OBJS = $(wildcard ./root/obj/*.o)
$(PROG) : $(BIN) $(SUBMAKEFILES)
	$(CC) -o $(BIN)/$@ $(OBJS) $(CXXFLAGS)

$(BIN) :
	if [ ! -d $(BIN) ]; then mkdir $(BIN); fi

.PHONY : $(SUBMAKEFILES)
$(SUBMAKEFILES) :
	$(MAKE) -C ./root

.PHONY : clean
clean :
	$(MAKE) -C ./root clean
	if [ -d $(BIN) ]; then rm $(BIN) -r; fi

.PHONY : install
install :
	sudo cp gmake /bin/gmake
