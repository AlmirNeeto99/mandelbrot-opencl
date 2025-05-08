__kernel void mandelbrot(__const float xMin, __const float xMax,
                         __const float yMin, __const float yMax,
                         __const int width, __const int height,
                         __const int maxIterations,
                         __global int *mandelbrotSpace) {

  int groupIdX = get_group_id(0);
  int groupIdY = get_group_id(1);
  int i = get_global_id(0);
  int j = get_global_id(1);

  if (i >= width || j >= height) {
    return;
  }

  if (groupIdX == 0 && groupIdY == 0 && i == 0 && j == 0) {
    printf("-> Mandelbrot kernel launched with %d x %d work items\n", width,
           height);
    printf("-> Mandelbrot kernel launched with X = %f x %f\n", xMin, xMax);
    printf("-> Mandelbrot kernel launched with Y = %f x %f\n", yMin, yMax);
    printf("-> Mandelbrot kernel launched with %d iterations\n", maxIterations);
    printf("-> Mandelbrot kernel launched with %zu x %zu x %zu x %zu\n",
           get_local_size(0), get_local_size(1), get_num_groups(0),
           get_num_groups(1));
  }

  const float xSize = (xMax - xMin) / width;
  const float ySize = (yMax - yMin) / height;

  const float x0 = xMin + xSize * i;
  const float y0 = yMin + ySize * j;

  int iterations = 0;
  float zx = 0, zy = 0;
  bool escaped = false;

  while (iterations < maxIterations) {

    if (sqrt(pow(zx, 2) + pow(zy, 2)) > 2) {
      escaped = true;
      break;
    }

    zx = pow(zx, 2) - pow(zy, 2) + x0;
    zy = 2 * zx * zy + y0;

    iterations++;
  }

  mandelbrotSpace[j * width + i] = escaped ? iterations : 0;
}
