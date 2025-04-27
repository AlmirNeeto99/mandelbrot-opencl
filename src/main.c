#include <stdio.h>
#include <stdlib.h>

#ifndef CL_TARGET_OPENCL_VERSION
#define CL_TARGET_OPENCL_VERSION 300
#endif

#include <CL/cl.h>

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
