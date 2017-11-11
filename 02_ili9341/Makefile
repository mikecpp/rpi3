CC = gcc
OBJS = bcm2835.o spi.o rpi_spi.o ili9341.o gui.o font24.o main.o
CFLAGS = -I ../00_lib
LFLAGS = 
VPATH = ../00_lib
TARGET = ili9341

$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LFLAGS) 

%.o: %c 
	$(CC) $(CFLAGS) -c $< 

clean:
	rm $(TARGET) *.o 

