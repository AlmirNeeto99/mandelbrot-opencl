__kernel void mandelbrot(__const int yMin, __const int yMax, __const int xMin,
                         __const int xMax, __const int width,
                         __const int height, __const int maxIterations,
                         __global int *mandelbrotSpace) {
  int i = get_global_id(0);
  int j = get_global_id(1);
  if (i >= width || j >= height) {
    return;
  }

  int iterations = i % 100 == 0 ? maxIterations : 0;

  mandelbrotSpace[i * height + j] = iterations;
}
