SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)


loc: $(OBJS)
	$(CC) -o loc $(OBJS)

$(OBJS): loc.h

test: loc
	./test.sh

clean:
	rm -f loc *.o *~

.PHONT: test clean
