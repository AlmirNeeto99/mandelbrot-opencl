CC=gcc

LINK=-lOpenCL -lpng -lm

DEPS=src/cl-utils.c src/common.c

build-opencl: src/main.c $(DEPS)
		$(CC) $(DEPS) src/main.c -o mandelbrot $(LINK)

build-opencl-hgx: src/main.c $(DEPS)
		$(CC) -I/usr/local/cuda-11.8/include -L/usr/local/cuda-11.8/lib64 $(DEPS) src/main.c -o mandelbrot $(LINK)
clean:
	rm mandelbrot
