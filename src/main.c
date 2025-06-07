#include <png.h>
#include <stdio.h>
#include <stdlib.h>

#include "headers/cl-utils.h"
#include "headers/common.h"

#define WIDTH 7680
#define HEIGHT 4320
#define Y_MIN -1.5f
#define Y_MAX 1.5f
#define X_MIN -2.0f
#define X_MAX 1.0f

int main(int argc, char const* argv[]) {
    cl_uint numberOfPlatforms = getNumberOfPlatforms();
    if (numberOfPlatforms > 0) {
        printf("-> Number of available OpenCL platforms: %d\n",
               numberOfPlatforms);
    } else {
        printf("-> No OpenCL platform found!\n");
        return 1;
    }

    int maxIterations = 0;

    scanf("%d", &maxIterations);
    if (maxIterations <= 0) {
        printf("-> Invalid number of iterations!\n");
        return 1;
    }

    cl_int err;

    float yMin = Y_MIN, yMax = Y_MAX, xMin = X_MIN, xMax = X_MAX;
    int width = WIDTH, height = HEIGHT;
    int* mandelbrotSpace = create2DSpace(width, height);

    cl_context context;
    cl_program program;
    cl_kernel kernel;
    cl_command_queue commandQueue;
    cl_mem mandelbrotBuffer;

    cl_platform_id platforms[numberOfPlatforms];
    clGetPlatformIDs(numberOfPlatforms, platforms, NULL);

    listAvailablePlatforms(platforms, numberOfPlatforms);

    cl_device_id deviceWithHighestComputeUnits =
        getDeviceWithHighestComputeUnits(platforms, numberOfPlatforms);

    printDeviceInfo(deviceWithHighestComputeUnits);

    context = clCreateContext(NULL, 1, &deviceWithHighestComputeUnits, NULL,
                              NULL, NULL);

    cl_queue_properties queueProperties[] = {CL_QUEUE_PROPERTIES,
                                             CL_QUEUE_PROFILING_ENABLE, 0};

    commandQueue = clCreateCommandQueueWithProperties(
        context, deviceWithHighestComputeUnits, queueProperties, &err);
    printError(err, "Creating command queue");

    char* kernelSource = NULL;
    readKernelSource("mandelbrot", &kernelSource);

    if (kernelSource == NULL) {
        clCleanUp(deviceWithHighestComputeUnits, context, commandQueue, kernel,
                  program, mandelbrotBuffer);
        return 1;
    }
    printf("============================================\n");

    program = clCreateProgramWithSource(context, 1, (const char**)&kernelSource,
                                        NULL, &err);
    printError(err, "Creating program");

    err = clBuildProgram(program, 1, &deviceWithHighestComputeUnits, NULL, NULL,
                         NULL);
    printError(err, "Building program");

    kernel = clCreateKernel(program, "mandelbrot", &err);
    printError(err, "Creating kernel");

    mandelbrotBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE,
                                      width * height * sizeof(int), NULL, &err);
    printError(err, "Creating buffer for mandelbrotSpace");

    err = clSetKernelArg(kernel, 0, sizeof(float), &xMin);
    printError(err, "Setting kernel argument 0 - xMin");
    err = clSetKernelArg(kernel, 1, sizeof(float), &xMax);
    printError(err, "Setting kernel argument 1 - xMax");
    err = clSetKernelArg(kernel, 2, sizeof(float), &yMin);
    printError(err, "Setting kernel argument 2 - yMin");
    err = clSetKernelArg(kernel, 3, sizeof(float), &yMax);
    printError(err, "Setting kernel argument 3 - yMax");
    err = clSetKernelArg(kernel, 4, sizeof(int), &width);
    printError(err, "Setting kernel argument 4 - width");
    err = clSetKernelArg(kernel, 5, sizeof(int), &height);
    printError(err, "Setting kernel argument 5 - height");
    err = clSetKernelArg(kernel, 6, sizeof(int), &maxIterations);
    printError(err, "Setting kernel argument 6 - maxIterations");
    err = clSetKernelArg(kernel, 7, sizeof(cl_mem), &mandelbrotBuffer);
    printError(err, "Setting kernel argument 7 - mandelbrotBuffer");

    printf("============================================\n");

    size_t globalSize[2] = {width, height};
    size_t localSize[2] = {32, 16};

    printWorkGroupInfo(globalSize, localSize);

    cl_event kernelEvent;

    err = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, globalSize,
                                 localSize, 0, NULL, &kernelEvent);
    printError(err, "Enqueueing kernel");

    err = clFinish(commandQueue);
    printError(err, "Finishing command queue");

    cl_ulong timeStart, timeEnd;
    err = clGetEventProfilingInfo(kernelEvent, CL_PROFILING_COMMAND_START,
                                  sizeof(cl_ulong), &timeStart, NULL);
    printError(err, "Getting kernel profiling info - start time");

    err = clGetEventProfilingInfo(kernelEvent, CL_PROFILING_COMMAND_END,
                                  sizeof(cl_ulong), &timeEnd, NULL);
    printError(err, "Getting kernel profiling info - end time");

    double kernelExecutionTime = (timeEnd - timeStart) / 1e6;
    printf("-> Kernel execution time: %.6f ms\n", kernelExecutionTime);

    int maxComputeUnits = 0;
    err = clGetDeviceInfo(deviceWithHighestComputeUnits,
                          CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(int),
                          &maxComputeUnits, NULL);
    printError(err, "Getting device info - max compute units");

    logExperiment(maxComputeUnits, globalSize, localSize, maxIterations,
                  kernelExecutionTime);
    printf("============================================\n");

    clReleaseEvent(kernelEvent);

    clEnqueueReadBuffer(commandQueue, mandelbrotBuffer, CL_TRUE, 0,
                        width * height * sizeof(int), mandelbrotSpace, 0, NULL,
                        NULL);
    printError(err, "Reading buffer");

    clCleanUp(deviceWithHighestComputeUnits, context, commandQueue, kernel,
              program, mandelbrotBuffer);

    const char* outputName = "mandelbrots/mandelbrot.png";

    save_result_to_png(outputName, mandelbrotSpace, width, height,
                       maxIterations);
    printf("-> Mandelbrot set saved to %s\n", outputName);

    free(kernelSource);
    free(mandelbrotSpace);

    return 0;
}
