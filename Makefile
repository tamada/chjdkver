TARGET=chjdkver
CFLAGS=-Wall
CC=gcc
RM=rm

$(TARGET): chjdkver.o init_env.o main.o
	$(CC) $(CFLAGS) -I. -o $(TARGET) chjdkver.o init_env.o main.o

main.o: main.c chjdkver.h
	$(CC) $(CFLAGS) -c -I. main.c

chjdkver.o: chjdkver.c chjdkver.h
	$(CC) $(CFLAGS) -c -I. chjdkver.c

init_env.o: init_env.c chjdkver.h
	$(CC) $(CFLAGS) -c -I. init_env.c

clean:
	$(RM) -f *~ *.o
