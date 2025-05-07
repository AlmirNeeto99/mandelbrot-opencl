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

    cl_int err;

    int width = 7680, height = 4320;
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
    printf("============================================\n");

    cl_program program = clCreateProgramWithSource(
        context, 1, (const char**)&kernelSource, NULL, &err);
    printError(err, "Creating program");

    err = clBuildProgram(program, 1, &deviceWithHighestComputeUnits, NULL, NULL,
                         NULL);
    printError(err, "Building program");

    cl_kernel kernel = clCreateKernel(program, "mandelbrot", &err);
    printError(err, "Creating kernel");

    cl_mem mandelbrotBuffer =
        clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
                       width * height * sizeof(double), mandelbrotSpace, NULL);

    err = clSetKernelArg(kernel, 7, sizeof(cl_mem), &mandelbrotBuffer);

    printError(err, "Setting kernel argument 7");

    size_t globalSize[2] = {width, height};
    size_t localSize[2] = {32, 16};

    clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, globalSize, localSize,
                           0, NULL, NULL);

    printf("-> Kernel enqueued!\n");
    err = clFinish(commandQueue);

    if (err != CL_SUCCESS) {
        printf("-> Error finishing command queue!\n");
    }
    printf("-> Command queue finished!\n");

    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(commandQueue);
    clReleaseContext(context);
    clReleaseDevice(deviceWithHighestComputeUnits);

    free(kernelSource);
    free(mandelbrotSpace);

    return 0;
}
