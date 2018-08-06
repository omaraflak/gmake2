CC = g++-8
ODIR = obj
CXXFLAGS = -std=c++17 -lstdc++fs

OBJS = $(ODIR)/gmake.o $(ODIR)/argument.o $(ODIR)/tools.o $(ODIR)/makefile.o
all : $(ODIR) $(OBJS)

$(ODIR)/gmake.o : src/gmake.cpp include/makefile.h include/argument.h include/tools.h
	$(CC) -c $< -o $@ $(CXXFLAGS)

$(ODIR)/argument.o : src/argument.cpp include/argument.h
	$(CC) -c $< -o $@ $(CXXFLAGS)

$(ODIR)/tools.o : src/tools.cpp include/tools.h
	$(CC) -c $< -o $@ $(CXXFLAGS)

$(ODIR)/makefile.o : src/makefile.cpp include/makefile.h
	$(CC) -c $< -o $@ $(CXXFLAGS)

$(ODIR) :
	if [ ! -d $(ODIR) ]; then mkdir $(ODIR); fi

.PHONY : clean
clean :
	if [ -d $(ODIR) ]; then rm $(ODIR) -r; fi
