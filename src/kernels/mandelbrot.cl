__kernel void mandelbrot(__const float yMin, __const float yMax,
                         __const float xMin, __const float xMax,
                         __const int width, __const int height,
                         __const int maxIterations,
                         __global int *mandelbrotSpace) {

  int i = get_global_id(0);
  int j = get_global_id(1);

  if (i >= width || j >= height) {
    return;
  }

  int iterations = 0;

  const float xSize = (xMax - xMin) / width;
  const float ySize = (yMax - yMin) / height;

  const float x0 = xMin + xSize * i;
  const float y0 = yMin + ySize * j;

  float zx = 0, zy = 0;
  int escaped = 0;

  while (iterations < maxIterations) {

    if (sqrt(pow(zx, 2) + pow(zy, 2)) > 2) {
      escaped = 1;
      break;
    }

    zx = pow(zx, 2) - pow(zy, 2) + x0;
    zy = 2 * zx * zy + y0;

    iterations++;
  }

  mandelbrotSpace[j * width + i] = escaped ? iterations : 0;
}
