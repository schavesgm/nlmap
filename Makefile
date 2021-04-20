# -- Target executable
TARGET = denoise_map

# -- Include directories
INCLUDE := -I include -I external

# -- Name of the directories where the data is stored
SRC_DIR := src
OBJ_DIR := obj

# -- Obtain all the lib files and their corresponding obj files
SRC_FILES := $(wildcard $(SRC_DIR)/*/*.cpp $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# -- Define the CXXFLAGS
CXXFLAGS = -std=c++17 -O3
CXX = g++

.PHONY: all
all: obj main $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ_DIR)/*.o $(OBJ_DIR)/*/*.o
	@rm -r obj

main:
	$(CXX) $(CXXFLAGS) -c $@.cpp -o $(OBJ_DIR)/$@.o $(INCLUDE)

# -- Compile the Map library in the src folder
$(OBJ_DIR)/Map/%.o: $(SRC_DIR)/Map/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

# -- Compile all small libraries inside the src folder
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCLUDE)

# - Rule to create the obj directory
obj:
	@mkdir -p $@
	@mkdir -p $@/Map
