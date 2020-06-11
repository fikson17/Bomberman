PROGRAM = Bomberman
CC = gcc
LDLIBS  = -lglut -lGLU -lGL -lm
CFLAGS  = -g -std=c99 -Wall -I/usr/X11R6/include -I/usr/pkg/include
LDFLAGS = -L/usr/X11R6/lib -L/usr/pkg/lib
$(PROGRAM): main.o image.o
	$(CC) $(LDFLAGS) -o $(PROGRAM) main.o image.o $(LDLIBS)
main.o: main.c
	$(CC) -c $(LDFLAGS) -o main.o main.c $(LDLIBS)
image.o: image.c
	$(CC) -c  -o image.o image.c 
.PHONY: clean
clean:
	-rm *.o $(PROGRAM) 
