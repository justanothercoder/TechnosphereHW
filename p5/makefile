CXX=g++
CXXFLAGS=-c -g -std=c++14 
LDFLAGS=
OBJECTS=main.o shell.o commandparser.o processtree.o
EXECUTABLE=shell

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
