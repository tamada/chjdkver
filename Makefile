TARGET=chjdkver
CFLAGS=-Wall
CC=gcc
RM=rm

$(TARGET): $(TARGET).o
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).o

$(TARGET).o: $(TARGET).c
	$(CC) -c $(TARGET).c

clean:
	$(RM) -f *~ *.o
