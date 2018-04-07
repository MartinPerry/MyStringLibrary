CC = g++

INCLUDE_DIRS = 

LIB_DIRS = -L./ -L/usr/lib64/

LIBS = -lm -lpthread -lstdc++ -ldl

LINKER_OPTS = -Wall -O2

CFLAGS = -g -std=c++11 $(INCLUDE_DIRS)



BIN = string_test


STRING_FILES = $(wildcard ./Strings/*.cpp)
ROOT_FILES = $(wildcard ./*.cpp)


OBJ = $(addprefix ,$(notdir $(STRING_FILES:.cpp=.obj))) \
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

$(BIN): $(OBJ)	
	$(CC) $(LINKER_OPTS) $^ $(LIB_DIRS) $(LIBS) -o $@
#-----------------------------------------


#----------- Strings

%.obj: ./Strings/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $< 



%.obj: ./%.cpp
	$(CC) $(CFLAGS) -c -o $@ $< 

                                
