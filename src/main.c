#include <stdio.h>
#include <stdlib.h>

#include "headers/cl-utils.h"
#include "headers/common.h"

int main(int argc, char const* argv[]) {
    cl_uint numberOfPlatforms = getNumberOfPlatforms();
    if (numberOfPlatforms > 0) {
        printf("-> Number of available OpenCL platforms: %d\n",
               numberOfPlatforms);
    } else {
        printf("-> No OpenCL platform found!\n");
        return 1;
    }

    int width = 800, height = 800;
    double* mandelbrotSpace = create2DSpace(width, height);

    cl_platform_id platforms[numberOfPlatforms];
    clGetPlatformIDs(numberOfPlatforms, platforms, NULL);

    listAvailablePlatforms(platforms, numberOfPlatforms);

    cl_device_id deviceWithHighestComputeUnits =
        getDeviceWithHighestComputeUnits(platforms, numberOfPlatforms);

    printDeviceInfo(deviceWithHighestComputeUnits);

    cl_context context = clCreateContext(
        NULL, 1, &deviceWithHighestComputeUnits, NULL, NULL, NULL);

    cl_command_queue commandQueue = clCreateCommandQueueWithProperties(
        context, deviceWithHighestComputeUnits, NULL, NULL);

    char* kernelSource = NULL;
    readKernelSource("mandelbrot", &kernelSource);

    if (kernelSource == NULL) {
        return 1;
    }

    cl_program program = clCreateProgramWithSource(
        context, 1, (const char**)&kernelSource, NULL, NULL);

    cl_kernel kernel = clCreateKernel(program, "mandelbrot", NULL);

    cl_int programBuilt = clBuildProgram(
        program, 1, &deviceWithHighestComputeUnits, NULL, NULL, NULL);

    if (programBuilt != CL_SUCCESS) {
        printf("-> Error building program!\n");
    }

    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(commandQueue);
    clReleaseContext(context);
    clReleaseDevice(deviceWithHighestComputeUnits);

    free(kernelSource);
    free(mandelbrotSpace);

    return 0;
}
