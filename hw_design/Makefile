# Makefile for ESE 5320 final project

# Compiler
CXX = g++

# Target executable
TARGET = p1

# Source files
SRCS = App_hw.c cdc_hw.c cmd_hw.c lzw_hw.c

# Object files
OBJS = $(SRCS:.c=.o)

# Build target
all: $(TARGET)

# Compile each source file into an object file
%.o: %.c
	$(CXX) -c $< -o $@

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Clean up object files and executable
clean:
	rm -f $(OBJS) $(TARGET)
