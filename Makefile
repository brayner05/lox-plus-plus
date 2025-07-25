# Compiler variables
CXX =	g++
CXXFLAGS =	-g -Wall -Wextra
INCLUDES =	
LIBRARIES =	

# Check for verbose
ifeq ($(VERBOSE),1)
	CFLAGS += -v
else
	Q := @
endif

# Functions
define print_ok
	@echo -n "[ $(TEXT_GREEN)OK$(TEXT_RESET) ]\t"
endef

define success_message
	$(call print_ok)
	@echo -n '[$(shell date)] - '
	@echo '$1'
endef

define create_dir
	$(Q)mkdir -p $1
	$(call success_message,'Created directory: $1')
endef

define remove_dir
	$(Q)rm -rf $1
	$(call success_message,'Removed directory: $1')
endef


# File variables
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

EXECUTABLE_NAME = 	a.out
TARGET = $(BIN_DIR)/$(EXECUTABLE_NAME)

TEXT_GREEN = \033[0;32m
TEXT_RESET = \033[0m

all: $(TARGET)


$(TARGET): $(OBJECTS)
	$(call create_dir,$(BIN_DIR))
	$(Q)$(CXX) -o $@ $^ $(LDFLAGS)
	$(call success_message,"Created target: $@")


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(call create_dir,$(OBJ_DIR))
	$(Q)$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<
	$(call success_message,"Compiled source file: $<")


clean: 
	$(call remove_dir,$(BIN_DIR))
	$(call remove_dir,$(OBJ_DIR))
	$(call success_message,"Clean complete")


.PHONY: clean


