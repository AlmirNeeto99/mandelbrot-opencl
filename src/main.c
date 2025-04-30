#include <stdio.h>
#include <stdlib.h>

#include "headers/cl-utils.h"

int main(int argc, char const* argv[]) {
    cl_uint numberOfPlatforms = getNumberOfPlatforms();
    if (numberOfPlatforms > 0) {
        printf("-> Number of available OpenCL platforms: %d\n",
               numberOfPlatforms);
    } else {
        printf("-> No OpenCL platform found!\n");
        return 1;
    }

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

    clReleaseContext(context);
    clReleaseCommandQueue(commandQueue);
    clReleaseDevice(deviceWithHighestComputeUnits);

    return 0;
}
