CC=gcc

LINK=-lOpenCL -lpng -lm

DEPS=src/cl-utils.c src/common.c

build-opencl: src/main.c $(DEPS)
		$(CC) $(DEPS) src/main.c -o opencl $(LINK)

clean:
	rm opencl