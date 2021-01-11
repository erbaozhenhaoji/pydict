TARGET=libpydict.a
PREFIX=/usr/lib
CC= g++

INCLUDES = -I./ -I./code/
LIBS =
DEFINES = -DLINUX -D_REENTERANT -Wall -D_FILE_OFFSET_BITS=64 

ifeq ($(MAKECMDGOALS), release)
	CFLAGS= $(DEFINES) -DNDEBUG -O3
else
	ifeq ($(MAKECMDGOALS), withpg)
		CFLAGS= $(DEFINES) -g -pg 
	else
		CFLAGS= $(DEFINES) -g
	endif
endif
C_SOURCES = $(wildcard *.cpp code/*.cpp)
C_OBJS = $(patsubst %.cpp, %.o, $(C_SOURCES))

.cpp.o:
	$(CC) -c -o $*.o $(CFLAGS) $(INCLUDES) $*.cpp

$(TARGET): $(C_OBJS)
	rm -f $@
	ar cr $@ $(C_OBJS)
	ranlib $@
clean:
	rm -f $(C_OBJS)
	rm -f $(TARGET)
	rm -f *~

install: $(TARGET)
	cp $(TARGET) $(PREFIX)

uninstall:
	rm -f $(PREFIX)/$(TARGET)

rebuild : clean $(TARGET) 
release : $(TARGET)
withpg  : $(TARGET)
