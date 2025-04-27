#include "headers/cl-utils.h"

#include <stdio.h>

cl_uint getNumberOfPlatforms() {
    cl_int numberOfPlatforms = 0;
    clGetPlatformIDs(0, NULL, &numberOfPlatforms);
    return numberOfPlatforms;
}

cl_uint getNumberOfGPUDevices(cl_platform_id platform) {
    cl_uint numberOfDevices = 0;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numberOfDevices);
    return numberOfDevices;
}

int getNumberOfAllGPUDevices(cl_platform_id platforms[],
                             int numberOfPlatforms) {
    int totalNumberOfGpus = 0;
    for (int i = 0; i < numberOfPlatforms; i++) {
        totalNumberOfGpus += getNumberOfGPUDevices(platforms[i]);
    }
    return totalNumberOfGpus;
}

void listAvailablePlatforms(cl_platform_id platforms[],
                            cl_uint numberOfPlatforms) {
    char name[255], version[255], profile[255], vendor[255];

    for (int i = 0; i < numberOfPlatforms; i++) {
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, -1, name, NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, -1, version, NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, -1, profile, NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, -1, vendor, NULL);
        printf("-> %s\n\t-> %s\n\t->%s\n\t->%s\n", name, version, profile,
               vendor);
        cl_uint numberOfDevices = getNumberOfGPUDevices(platforms[i]);
        printf("\t-> Number of GPU devices: %u\n", numberOfDevices);
    }
}

void getAllDevices(cl_platform_id platforms[], cl_uint numberOfPlatforms,
                   cl_device_id devices[]) {
    int currentCount = 0, numDevicesInPlatform = 0;

    for (int i = 0; i < numberOfPlatforms; i++) {
        numDevicesInPlatform = 0;

        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 0, NULL,
                       &numDevicesInPlatform);
        if (numDevicesInPlatform > 0) {
            clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU,
                           numDevicesInPlatform, devices + currentCount, NULL);
            currentCount += numDevicesInPlatform;
        }
    }
}