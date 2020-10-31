# Adapted from: https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html

CXX	:= g++
CXXFLAGS:= 

INCLUDES:= -Iinclude/
SRCS	:= main.cpp geometry.cpp node.cpp
OBJS 	:= $(SRCS:.cpp=.o)

EXEC_PATH := ./bin/
EXEC	:= $(EXEC_PATH)main

.PHONY: depend clean

all:    $(EXEC)
	@echo Compilation Complete

$(EXEC): $(OBJS) 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
