SRC_DIR := src
OBJ_DIR := .cache
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

CXX := g++
CXXFLAGS := -g -I include

run : link
	./VM

link: source
	g++ $(CXXFLAGS) -o VM $(OBJS) $(LDFLAGS)

all: source inst
	g++ $(CXXFLAGS) -o VM $(OBJS) $(LDFLAGS)

source: $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

inst: instructions
	$(MAKE) -C instructions

clean:
	rm $(OBJS)

.PHONY: all clean inst source
