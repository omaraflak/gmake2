CC = g++-8
ODIR = obj
PROG = gmake
CXXFLAGS = -Wall -Wextra -std=c++17 -lstdc++fs

OBJS = $(ODIR)/gmake.o $(ODIR)/makefile.o $(ODIR)/parser.o $(ODIR)/tools.o
$(PROG) : $(ODIR) $(OBJS)
	$(CC) -o $@ $(OBJS) $(CXXFLAGS)

$(ODIR)/gmake.o : ./root/src/gmake.cpp ./root/include/makefile.h ./root/include/parser.h ./root/include/tools.h ./root/include/gmake_options.h
	$(CC) -c ./root/src/gmake.cpp -o $@ $(CXXFLAGS)

$(ODIR)/makefile.o : ./root/src/makefile.cpp ./root/include/makefile.h
	$(CC) -c ./root/src/makefile.cpp -o $@ $(CXXFLAGS)

$(ODIR)/parser.o : ./root/src/parser.cpp ./root/include/parser.h
	$(CC) -c ./root/src/parser.cpp -o $@ $(CXXFLAGS)

$(ODIR)/tools.o : ./root/src/tools.cpp ./root/include/tools.h ./root/include/gmake_options.h
	$(CC) -c ./root/src/tools.cpp -o $@ $(CXXFLAGS)

$(ODIR) :
	if [ ! -d $(ODIR) ]; then mkdir $(ODIR); fi

.PHONY : clean
clean :
	if [ -d $(ODIR) ]; then rm $(ODIR) -r; fi
	if [ -f $(PROG) ]; then rm $(PROG); fi
