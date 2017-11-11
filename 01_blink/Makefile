CC = gcc
OBJS = bcm2835.o main.o
CFLAGS = -I ../00_lib
LFLAGS = 
VPATH = ../00_lib
TARGET = blink

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LFLAGS) 

%.o: %c 
	$(CC) $(CFLAGS) -c $< 

clean:
	rm $(TARGET) *.o 

