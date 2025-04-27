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

    int numberOfGpusDevices =
        getNumberOfAllGPUDevices(platforms, numberOfPlatforms);
    cl_device_id devices[numberOfGpusDevices];

    getAllDevices(platforms, numberOfPlatforms, devices);

    if (numberOfGpusDevices > 0) {
        printf("-> Number of GPU devices found: %d\n", numberOfGpusDevices);

        if (!devices) {
            printf("-> Unable to read devices ID's\n");
            return 2;
        }
    }

    return 0;
}
