CC = g++-8
BIN = bin
CXXFLAGS = -std=c++17 -lstdc++fs
OBJS_GMAKE = ./root/obj/makefile.o ./root/obj/argument.o ./root/obj/tools.o ./root/obj/gmake.o

all : gmake

gmake : $(BIN) ./root/include/gmake_options.h
	$(MAKE) -C ./root obj obj/makefile.o obj/argument.o obj/tools.o obj/gmake.o
	$(CC) -o $(BIN)/gmake $(OBJS_GMAKE) $(CXXFLAGS)

$(BIN) :
	if [ ! -d $(BIN) ]; then mkdir $(BIN); fi

clean :
	$(MAKE) -C ./root clean
	if [ -d $(BIN) ]; then rm $(BIN) -r; fi

install :
	sudo cp $(BIN)/gmake /bin/gmake

.PHONY : all
.PHONY : gmake
.PHONY : clean
.PHONY : install
