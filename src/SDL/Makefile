
INCLUDES = \
    -I. \
    -I/usr/include/SDL


LIBS = \
	-lGL -lGLU -lSDL -lSDL_mixer

GPPFLAGS = \
    $(INCLUDES) \


ICCFLAGS = \
    $(INCLUDES) 


CPP = g++ $(GPPFLAGS)
#CPP = g++ $(IPPFLAGS)


.cpp.o:
	$(CPP) -g -c -arch i386 -o $@ $<
.c.o:
	$(CC) -g -c -o $@ $<

OBJS = \
    SDL_OGL.o \
    main.o \
    SoundManager.o


compileobjs: $(OBJS)

HEADERS = \
    SDL_OGL.h \
    SoundManager.h



lib_tutorial: $(HEADERS) $(OBJS) Makefile
	ar rvs libsm $(OBJS)
	mv libsm libsm.a

sample_tutorial: $(HEADERS) $(OBJS) Makefile
	$(CPP) -o test $(OBJS) $(LIBS)

clean:
	-rm *.o core
