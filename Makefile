CC = gcc
LD = $(CC)
CFLAGS = -ansi -Wall -W -O2 -pedantic -g
LIBS = -lmpd
TARGET = mpdump
OBJS = bool.o xalloc.o mpdump.o

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET) : $(OBJS)
	$(LD) $(OBJS) -o $@ $(LIBS)

clean:
	@rm -vf $(OBJS) $(TARGET) *~
