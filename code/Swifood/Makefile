CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wno-deprecated-declarations


TARGET = swifood

SRCS = 				\
    main.cpp 		\
    action.cpp 		\
    error.cpp 		\
    ingredient.cpp 	\
    recipe.cpp 		\
    swifood.cpp 	\
    utils.cpp


BISON_SRC = swifood.y
FLEX_SRC = swifood.l

BISON_GEN_SRC = swifood.tab.cpp
BISON_GEN_HDR = swifood.tab.hpp
FLEX_GEN_SRC = swifood.yy.cpp

OBJS = $(SRCS:.cpp=.o) $(BISON_GEN_SRC:.cpp=.o) $(FLEX_GEN_SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BISON_GEN_SRC) $(BISON_GEN_HDR): $(BISON_SRC)
	bison -d -o $(BISON_GEN_SRC) $(BISON_SRC)

$(FLEX_GEN_SRC): $(FLEX_SRC) $(BISON_GEN_HDR)
	flex -o $(FLEX_GEN_SRC) $(FLEX_SRC)

clean:
	rm -f $(TARGET) $(OBJS) $(BISON_GEN_SRC) $(BISON_GEN_HDR) $(FLEX_GEN_SRC)

.PHONY: all clean