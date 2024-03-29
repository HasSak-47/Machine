SRCS := $(wildcard *.cpp)

CXX := g++
CXXFLAGS := -I ../include -fPIC -shared
INST_DIR := ../inst

OBJS := $(patsubst %.cpp, $(INST_DIR)/%.so, $(SRCS))

all: $(OBJS)

$(INST_DIR)/%.so: %.cpp | $(INST_DIR)
	@echo "Building $@..."
	@$(CXX) $(CXXFLAGS)  $< -o ../inst/$@

clean:
	rm $(OBJS)

.PHONY: all clean
