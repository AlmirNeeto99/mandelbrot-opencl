#include <png.h>
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

    double yMin = -1.5, yMax = 1.5, xMin = -2.0, xMax = 1.0;
    int width = 7680, height = 4320;
    int maxIterations = 10000;
    int* mandelbrotSpace = create2DSpace(width, height);

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

    cl_mem mandelbrotBuffer = clCreateBuffer(
        context, CL_MEM_READ_WRITE, width * height * sizeof(int), NULL, &err);
    printError(err, "Creating buffer for mandelbrotSpace");

    err = clSetKernelArg(kernel, 0, sizeof(int), &xMin);
    printError(err, "Setting kernel argument 0");
    err = clSetKernelArg(kernel, 1, sizeof(int), &xMax);
    printError(err, "Setting kernel argument 1");
    err = clSetKernelArg(kernel, 2, sizeof(int), &yMin);
    printError(err, "Setting kernel argument 2");
    err = clSetKernelArg(kernel, 3, sizeof(int), &yMax);
    printError(err, "Setting kernel argument 3");
    err = clSetKernelArg(kernel, 4, sizeof(int), &width);
    printError(err, "Setting kernel argument 4");
    err = clSetKernelArg(kernel, 5, sizeof(int), &height);
    printError(err, "Setting kernel argument 5");
    err = clSetKernelArg(kernel, 6, sizeof(int), &maxIterations);
    printError(err, "Setting kernel argument 6");
    err = clSetKernelArg(kernel, 7, sizeof(cl_mem), &mandelbrotBuffer);
    printError(err, "Setting kernel argument 7");

    size_t globalSize[2] = {width, height};
    size_t localSize[2] = {32, 16};

    printf("============================================\n");

    printf("-> Global size: %zu x %zu\n", globalSize[0], globalSize[1]);
    printf("-> Local size: %zu x %zu\n", localSize[0], localSize[1]);

    size_t globalSizeX = globalSize[0] / localSize[0];
    size_t globalSizeY = globalSize[1] / localSize[1];
    printf("-> Number of groups: %zu x %zu\n", globalSizeX, globalSizeY);
    printf("-> Total number of groups: %zu\n", globalSizeX * globalSizeY);
    printf("-> Number of work items: %zu\n", globalSize[0] * globalSize[1]);
    printf("-> Number of work items per group: %zu\n",
           localSize[0] * localSize[1]);

    printf("============================================\n");

    err = clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, globalSize,
                                 localSize, 0, NULL, NULL);
    printError(err, "Enqueueing kernel");

    err = clFinish(commandQueue);
    printError(err, "Finishing command queue");

    clEnqueueReadBuffer(commandQueue, mandelbrotBuffer, CL_TRUE, 0,
                        width * height * sizeof(int), mandelbrotSpace, 0, NULL,
                        NULL);
    printError(err, "Reading buffer");

    clReleaseMemObject(mandelbrotBuffer);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(commandQueue);
    clReleaseContext(context);
    clReleaseDevice(deviceWithHighestComputeUnits);

    const char* outputName = "mandelbrots/mandelbrot.png";

    FILE* mandelbrot = fopen(outputName, "wb");

    if (!mandelbrot) {
        printf("Unable to open %s file\n", outputName);
        return -1;
    }

    png_structp png =
        png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fclose(mandelbrot);
        return -1;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, NULL);
        fclose(mandelbrot);
        return -1;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(mandelbrot);
        return -1;
    }

    png_set_compression_level(png, 0);
    png_init_io(png, mandelbrot);
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    png_bytep row = (png_bytep)malloc(3 * width * sizeof(png_byte));

    int color = 0;
    double proportion = 0;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            proportion =
                (double)mandelbrotSpace[x * height + y] / maxIterations;
            color = (int)(proportion * 255);
            row[x * 3] = color;      // Red
            row[x * 3 + 1] = color;  // Green
            row[x * 3 + 2] = color;  // Blue
        }
        png_write_row(png, row);
    }
    free(row);

    png_write_end(png, NULL);
    png_destroy_write_struct(&png, &info);

    free(kernelSource);
    free(mandelbrotSpace);

    return 0;
}
