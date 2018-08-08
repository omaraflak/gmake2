CXX = g++-8
ODIR = obj
CXXFLAGS = -std=c++17
OBJS = $(ODIR)/gmake.o $(ODIR)/argument.o $(ODIR)/tools.o $(ODIR)/makefile.o 

all : $(ODIR) $(OBJS) 

$(ODIR)/gmake.o : src/gmake.cpp include/makefile.h include/argument.h include/tools.h 
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(ODIR)/argument.o : src/argument.cpp include/argument.h 
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(ODIR)/tools.o : src/tools.cpp include/tools.h 
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(ODIR)/makefile.o : src/makefile.cpp include/makefile.h 
	$(CXX) -c $< -o $@ $(CXXFLAGS)

$(ODIR) : 
	if [ ! -d $(ODIR) ]; then mkdir $(ODIR); fi

clean : 
	if [ -d $(ODIR) ]; then rm $(ODIR) -r; fi

.PHONY : all
.PHONY : clean

