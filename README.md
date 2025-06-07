# Mandelbrot - OpenCL

This repository keeps a **Mandelbrot** set implementation in `C` using `OpenCL`.

## Structure

- `src/main.c`: the main source containing all OpenCL handling and execution code
- `headers/common.h`: contains the definition of functions with general common objetive
- `src/common.c`: contains the `commons.h` implementations
- `headers/cl-utils.h`: a header file containing useful OpenCL functions declarations
- `src/cl-utils.c`: containg the header implementation
- `Makefile`: contains the rules for build both serial and parallel executables
- `experiment`: a script for helping executing code for different situations iterations count.

## General information

We divided our problem into a two dimensional structure.

Since we are working on image composed of 7680x4320 pixels, we created a grid of `7680x4320 work items` where each work group computed `32x16 work items` a time.
This value was used so we get a perfectly distributed work items per work group (for the device used in execution tests).

## Building

Run:

```bash
make build-opencl
```

## Input:

The input file should have a single integer number that defines the maximum number of iterations for the **mandelbrot** algorithm.

You can feed the application by running:

```bash
cat input.txt | ./mandelbrot
```

## Output:

A PNG file will be rendered in the folder the application is running.

Default filename: `mandelbrot.png`

_NOTE: The image will be overwritten in consecutive runs_

## Requirements

- An **OpenCL** capable hardware
- **OpenCL** drivers installed on machine
- `libpng`: for rendering the mandelbrot result (update when tested and validated)
