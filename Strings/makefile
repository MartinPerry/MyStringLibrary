CC = g++
C_COMPILER = cc

ARCH := $(shell uname -m)
SIMD_FLAGS :=
ifeq ($(ARCH),x86_64)
SIMD_FLAGS += -mavx2 -mfma
endif

INCLUDE_DIRS = -I/usr/include/

LIB_DIRS = -L./ -L/usr/lib/

LIBS = -lm -lpthread -lstdc++ -ldl

LINKER_OPTS = -Wall -g -O3

CFLAGS = -std=c++20 $(SIMD_FLAGS) -g -Wall -O3 -Wunknown-pragmas \
		 -Wno-unused-function -Wno-unused-value \
		 $(INCLUDE_DIRS)

BIN = libstrings.a
APP = strings_demo

#=================================================================================

HASHING_FILES = $(wildcard ./hashing/*.cpp)
LOCALIZATION_FILES = $(wildcard ./SimpleLocalization/*.cpp)
LOCALIZATION_C_FILES = $(wildcard ./SimpleLocalization/*.c)
UNICODE_FILES = $(wildcard ./Unicode/*.cpp)
TEST_FILES = $(wildcard ./Testing/*.cpp)
ROOT_FILES = $(wildcard ./*.cpp)


OBJ = $(addprefix ,$(notdir $(LOCALIZATION_FILES:.cpp=.obj))) \
	  $(addprefix ,$(notdir $(LOCALIZATION_C_FILES:.c=.obj))) \
	  $(addprefix ,$(notdir $(HASHING_FILES:.cpp=.obj))) \
	  $(addprefix ,$(notdir $(UNICODE_FILES:.cpp=.obj))) \
	  $(addprefix ,$(notdir $(TEST_FILES:.cpp=.obj))) \
	  $(addprefix ,$(notdir $(ROOT_FILES:.cpp=.obj)))


.PHONY: sestav
.PHONY: all
.PHONY: clean
.PHONY: vypis
.PHONY: run


vypis:
	@echo $(OBJ)
#-----------------------------------------

clean:
	rm -f *.obj
	rm -f *.o
	rm -f $(BIN)
	rm -f $(APP)


#-----------------------------------------
all: $(BIN) $(APP)
sestav: all


#-----------------------------------------
#$(CC) $(LIB_DIRS) $(LINKER_OPTS) -o $@ $^  $(LIBS)

#$(BIN): $(OBJ)	
#	$(CC) $(LINKER_OPTS) $^ $(LIB_DIRS) $(LIBS) -o $@

$(BIN): $(OBJ)
	ar rc $@ $(OBJ)
	ranlib $@

$(APP): $(OBJ)
	$(CC) $(LINKER_OPTS) -o $@ $^ $(LIB_DIRS) $(LIBS)
#-----------------------------------------


#----------- Strings

%.obj: ./hashing/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $< 

%.obj: ./SimpleLocalization/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $< 
	
%.obj: ./Unicode/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $< 	

%.obj: ./Testing/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

%.obj: ./SimpleLocalization/%.c
	$(C_COMPILER) -g -O3 -Wall $(INCLUDE_DIRS) -c -o $@ $<


%.obj: ./%.cpp
	$(CC) $(CFLAGS) -c -o $@ $< 


run: $(APP)
	./$(APP)
                                
