# -- Target executable
TARGET = denoise_map

# -- Include directories
INCLUDE := -I include -I external

# -- Name of the directories where the data is stored
SRC_DIR := src
OBJ_DIR := obj

# -- Obtain all the CXX files and their corresponding object files
CXX_SRC := $(wildcard $(SRC_DIR)/*/*.cpp $(SRC_DIR)/*.cpp)
CXX_OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CXX_SRC))

# -- Obtain all the CUDA files and their corresponding object files
NVC_SRC := $(wildcard $(SRC_DIR)/*.cu)
NVC_OBJ := $(patsubst $(SRC_DIR)/%.cu, $(OBJ_DIR)/%.o, $(NVC_SRC))

# -- Extensions used by the different files
SRC_EXT = cpp cu

# -- Define the CXXFLAGS
CXXFLAGS = -std=c++14 -O3 --gpu-architecture=sm_52
CXX = nvcc

.PHONY: all
all: obj $(CXX_OBJ) $(NVC_OBJ) main
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ_DIR)/*.o $(OBJ_DIR)/*/*.o
	@rm -r obj

main:
	$(CXX) $(CXXFLAGS) -dc $@.cu -o $(OBJ_DIR)/$@.o $(INCLUDE)

# -- Compile the CXX Map library in the src folder
$(OBJ_DIR)/Map/%.o: $(SRC_DIR)/Map/%.cpp
	$(CXX) $(CXXFLAGS) -dc $< -o $@ $(INCLUDE)

# -- Define a rule to compile small libraries
define compile_small_lib
$$(OBJ_DIR)/%.o: $$(SRC_DIR)/%.$1
	$$(CXX) $$(CXXFLAGS) -dc $$< -o $$@ $$(INCLUDE)
endef

# -- Compile all small libraries inside the src folder
EXTENSIONS = cpp cu
$(foreach EXT, $(SRC_EXT), $(eval $(call compile_small_lib,$(EXT))))

# -- Simple rule to create the obj file
obj:
	@mkdir -p $@
	@mkdir -p $@/Map
