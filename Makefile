app = desc_autograder

#listing all the source and build ouput directories
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
OUT_DIR := output

#Naming all the final target
EXE_1 := $(BIN_DIR)/server 
EXE_2 := $(BIN_DIR)/client 

#listing all the src files: to list all out me are using wildcard here
SRC_1 := $(wildcard $(SRC_DIR)/s_*.c)
SRC_2 := $(wildcard $(SRC_DIR)/c_*.c)

#generating the list of object by replacing the .c list with .o
OBJ_1 := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_1))
OBJ_2 := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_2))

#hadling all the Flags
CFLAGS := -g -O0 -Wall # -g flag is for debug enable -Wall for seeing warning
CPPFLAGS := -Iinclude -MMD -MP # -I is a preprocessor
LDFLAGS := -Llib  #to link external library
LDLIBS :=  #for external library

#CPP is for C Pre Processor not CPlusPlus flags
#for C++ its CXX
# -MMD -MP are flags are used to generate header dependencies automatically. it is used to trigger
#  compilation when only a header chages
#

.PHONY: all clean

all: $(EXE_1) $(EXE_2)

$(EXE_1): $(OBJ_1) | $(BIN_DIR) $(OUT_DIR)
	@echo Creating Server 
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@
	@cp src/out.txt $(OUT_DIR)/ #coping the refernce output to output location 

$(EXE_2): $(OBJ_2) | $(BIN_DIR) $(OUT_DIR)
	@echo Creating Client 
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR) $(OUT_DIR):
	mkdir -p $@

clean: 
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR) $(OUT_DIR)


-include $(OBJ:.o=.d)
