SOURCES = main.cpp utilities.cpp rayCasting.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXE = mpiThreads
CC = mpic++
CFLAGS = -c
FLAGS = -lpthread

all: $(SOURCES) $(EXE)

$(EXE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(FLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXE)
	rm -rf *.vlog
	clear