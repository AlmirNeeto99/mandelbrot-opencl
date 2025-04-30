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

void listAvailablePlatforms(cl_platform_id platforms[],
                            cl_uint numberOfPlatforms) {
    char name[255], version[255], profile[255], vendor[255];

    for (int i = 0; i < numberOfPlatforms; i++) {
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(name), name,
                          NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(version),
                          version, NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_PROFILE, sizeof(profile),
                          profile, NULL);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(vendor),
                          vendor, NULL);
        printf("-> %s\n\t-> %s\n\t->%s\n\t->%s\n", name, version, profile,
               vendor);
        cl_uint numberOfDevices = getNumberOfGPUDevices(platforms[i]);
        printf("\t-> Number of GPU devices: %u\n", numberOfDevices);
    }
}

cl_device_id getDeviceWithHighestComputeUnits(cl_platform_id platforms[],
                                              cl_uint numberOfPlatforms) {
    cl_device_id deviceWithHighestComputeUnits = NULL;
    cl_uint maxComputeUnits = 0;

    for (int i = 0; i < numberOfPlatforms; i++) {
        cl_uint numberOfDevices = getNumberOfGPUDevices(platforms[i]);
        cl_device_id devices[numberOfDevices];
        clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, numberOfDevices,
                       devices, NULL);

        for (int j = 0; j < numberOfDevices; j++) {
            cl_uint computeUnits;
            clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS,
                            sizeof(computeUnits), &computeUnits, NULL);
            if (computeUnits > maxComputeUnits) {
                maxComputeUnits = computeUnits;
                deviceWithHighestComputeUnits = devices[j];
            }
        }
    }
    printf("-> Highest compute units: %u\n", maxComputeUnits);
    return deviceWithHighestComputeUnits;
}