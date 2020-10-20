XX = g++

CXXFLAGS = -Wall -g -std=c++11


SRCS = $(shell find src -maxdepth 1 -name "*.cpp")
OBJS = $(patsubst %.cpp, %.o, $(SRCS))

TARGET = bin/compiler2018

all: $(TARGET)
 
$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)


depend: src/.depend


src/.depend : $(SRCS)
	$(CXX) $(CXXFLAGS) -MM $^ >> src/.depend;

clean:
	rm -f $(OBJS)

include src/.depend

