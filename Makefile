loc: loc.c

test: loc
	./test.sh

clean:
	rm -f loc *.o *~

.PHONT: test clean
