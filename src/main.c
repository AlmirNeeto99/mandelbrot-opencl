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

    float yMin = -1.5f, yMax = 1.5f, xMin = -2.0f, xMax = 1.0f;
    int width = 7680, height = 4320;
    int maxIterations = 100;
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
            // if (mandelbrotSpace[y * width + x] > 5000) {
            //     printf("Iteration: %d\n", mandelbrotSpace[y * width + x]);
            // }
            proportion = (double)mandelbrotSpace[y * width + x] / maxIterations;
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
