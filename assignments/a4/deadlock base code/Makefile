SOURCES = main.cpp find_deadlock.cpp
CPPC = g++
CPPFLAGS = -c -Wall -O2
LDLIBS = 
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = deadlock

all: $(TARGET)

find_deadlock.o: find_deadlock.h
main.o: find_deadlock.h
%.o : %.c
$(OBJECTS): Makefile 

.cpp.o:
	$(CPPC) $(CPPFLAGS) $< -o $@

$(TARGET): $(OBJECTS)
	$(CPPC) -o $@ $(OBJECTS) $(LDLIBS)

.PHONY: clean
clean:
	rm -f .*~ *~ *.o $(TARGET)
