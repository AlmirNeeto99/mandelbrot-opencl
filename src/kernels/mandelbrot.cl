__kernel void mandelbrot(__global const float *plane,
                         __global const int *max_iter, __global int *output,
                         int width, int height) {
  int i = get_global_id(0);
}