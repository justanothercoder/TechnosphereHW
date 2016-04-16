CXX=clang++
CXXFLAGS=-c -g -std=c++14 
LDFLAGS=
OBJECTS=main.o sorter.o binarywriter.o binaryreader.o
EXECUTABLE=index_sort

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
