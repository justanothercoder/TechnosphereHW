CXX=g++
CXXFLAGS=-c -g -std=c++14
LDFLAGS=
OBJECTS=main.o client.o
EXECUTABLE=client

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
