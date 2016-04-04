CXX=clang++
CXXFLAGS=-c -g -std=c++14 
LDFLAGS=-lboost_system 
OBJECTS=main.o proxy.o server_connection.o
EXECUTABLE=proxy

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS)
