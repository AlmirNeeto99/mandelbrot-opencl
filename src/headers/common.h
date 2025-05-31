#include <stddef.h>

void readKernelSource(const char *filename, char **source);
int *create2DSpace(int width, int height);
void logExperiment(int numberComputeUnits, size_t globalGroupSize[2],
                   size_t localGroupSize[2], int maxNumberOfIterations,
                   double elapsedTime);
int save_result_to_png(const char *fileName, int *iterations, int width,
                       int height, int maxIterations);