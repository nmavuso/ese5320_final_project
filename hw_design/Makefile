# Makefile for ESE 5320 final project

# Compiler
CXX = g++

# Target executable
TARGET = p1

# Source files
SRCS = cmd_hw.cpp lzw_hw.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Build target
all: $(TARGET)

# Compile each source file into an object file
%.o: %.cpp
	$(CXX) -c $< -o $@

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Clean up object files and executable
clean:
	rm -f $(OBJS) $(TARGET)
