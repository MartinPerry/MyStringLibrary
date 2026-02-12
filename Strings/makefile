CC = g++

INCLUDE_DIRS = -I/usr/include/

LIB_DIRS = -L./ -L/usr/lib/

LIBS = -lm -lpthread -lstdc++ -ldl

LINKER_OPTS = -Wall -g -O3

CFLAGS = -std=c++20 -mavx2 -mfma -g -Wall -O3 -Wunknown-pragmas \
		 -Wno-unused-function -Wno-unused-value \
		 $(INCLUDE_DIRS)

BIN = libstrings.a

#=================================================================================

HASHING_FILES = $(wildcard ./hashing/*.cpp)
LOCALIZATION_FILES = $(wildcard ./SimpleLocalization/*.cpp)
UNICODE_FILES = $(wildcard ./Unicode/*.cpp)
ROOT_FILES = $(wildcard ./*.cpp)


OBJ = $(addprefix ,$(notdir $(LOCALIZATION_FILES:.cpp=.obj))) \
	  $(addprefix ,$(notdir $(HASHING_FILES:.cpp=.obj))) \
	  $(addprefix ,$(notdir $(UNICODE_FILES:.cpp=.obj))) \
	  $(addprefix ,$(notdir $(ROOT_FILES:.cpp=.obj)))


.PHONY: sestav
.PHONY: clean
.PHONY: vypis


vypis:
	@echo $(OBJ)
#-----------------------------------------

clean:
	rm -f *.obj
	rm -f *.o


#-----------------------------------------
sestav: ${BIN}


#-----------------------------------------
#$(CC) $(LIB_DIRS) $(LINKER_OPTS) -o $@ $^  $(LIBS)

#$(BIN): $(OBJ)	
#	$(CC) $(LINKER_OPTS) $^ $(LIB_DIRS) $(LIBS) -o $@

$(BIN): $(OBJ)
	ar rc $@ $(OBJ)
	ranlib $@
#-----------------------------------------


#----------- Strings

%.obj: ./hashing/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $< 

%.obj: ./SimpleLocalization/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $< 
	
%.obj: ./Unicode/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $< 	


%.obj: ./%.cpp
	$(CC) $(CFLAGS) -c -o $@ $< 

                                
