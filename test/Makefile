#
#
#
#
PLIB=..
INCLUDE = -I./ -I../src
LDFLAGS     = -L./ -L../src -lpydict -lpthread -lm  -g
COMMON_DEFINES = -DLINUX -D_REENTERANT -Wall -D_FILE_OFFSET_BITS=64 $(INCLUDE)  -g

ifeq "$(MAKECMDGOALS)" "release"
	DEFINES=$(COMMON_DEFINES) -DNDEBUG -O3
	CFLAGS= $(DEFINES) 
else
	ifeq "$(MAKECMDGOALS)" "withpg"
		DEFINES=$(COMMON_DEFINES) 
		CFLAGS= -g -pg $(DEFINES) 
	else
		DEFINES=$(COMMON_DEFINES)
		CFLAGS= -g $(DEFINES) 
	endif
endif
CC  = gcc
AR  = ar
#=========================================================================

EXECUTABLE =  test_pdict \
	      test_pdict_create 

TEST_EXEC = 

all	:  $(EXECUTABLE) $(LIBS) $(TEST_EXEC)

deps :
	$(CC) -MM -MG *.c >depends




test_pdict : test_pdict.o
	$(CC) -o $@ $^ $(LDFLAGS)

test_pdict_create : test_pdict_create.o
	$(CC) -o $@ $^ $(LDFLAGS)


rebuild : clean all
clean   :
	/bin/rm -f *.o core.* *~ $(EXECUTABLE) $(TEST_EXEC) 


release : all
withpg  : all

-include depends

