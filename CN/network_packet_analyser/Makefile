CXX=g++
CXXFLAGS=-O2 -pthread

SRC = main.cpp packet_capture.cpp packet_parser.cpp stats.cpp flow_table.cpp packet_ring.cpp
OBJ = $(SRC:.cpp=.o)

nic_analyzer: $(OBJ)
	$(CXX) $(CXXFLAGS) -o nic_analyzer $(OBJ)

clean:
	rm -f *.o nic_analyzer