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
        printf("-> %s\n\t-> %s\n\t-> %s\n\t-> %s\n", name, version, profile,
               vendor);
        cl_uint numberOfDevices = getNumberOfGPUDevices(platforms[i]);
        printf("\t-> Number of GPU devices: %u\n", numberOfDevices);
        printf("===========================================================\n");
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

    char name[255];

    clGetDeviceInfo(deviceWithHighestComputeUnits, CL_DEVICE_NAME, sizeof(name),
                    name, NULL);

    printf("-> Device with highest compute units: %s\n", name);
    printf("-> Highest compute units: %u\n", maxComputeUnits);
    printf("===========================================================\n");
    return deviceWithHighestComputeUnits;
}

void printDeviceInfo(cl_device_id device) {
    char name[255], version[255], vendor[255];
    size_t maxWorkGroupSize, maxWorkItemSizes[3], preferredWorkGroupSize;
    cl_ulong globalMem, localMem;
    cl_uint maxComputeUnits, maxWorkItemDimensions;

    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(name), name, NULL);
    clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(version), version, NULL);
    clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(vendor), vendor, NULL);
    clGetDeviceInfo(device, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(globalMem),
                    &globalMem, NULL);
    clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(localMem),
                    &localMem, NULL);
    clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS,
                    sizeof(maxComputeUnits), &maxComputeUnits, NULL);
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
                    sizeof(maxWorkItemDimensions), &maxWorkItemDimensions,
                    NULL);
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE,
                    sizeof(maxWorkGroupSize), &maxWorkGroupSize, NULL);
    clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES,
                    sizeof(maxWorkItemSizes), maxWorkItemSizes, NULL);
    clGetDeviceInfo(device, CL_DEVICE_PREFERRED_WORK_GROUP_SIZE_MULTIPLE,
                    sizeof(preferredWorkGroupSize), &preferredWorkGroupSize,
                    NULL);

    printf("-> Device name: %s\n", name);
    printf("-> Device version: %s\n", version);
    printf("-> Device vendor: %s\n", vendor);
    printf("-> Global memory size: %lu GB\n", globalMem / 1024 / 1024 / 1024);
    printf("-> Local memory size: %lu KB\n", localMem / 1024);
    printf("-> Max compute units: %u\n", maxComputeUnits);
    printf("-> Max work item dimensions: %u\n", maxWorkItemDimensions);
    printf("-> Max work group size: %zu\n", maxWorkGroupSize);
    printf("-> Max work item sizes: %zu x %zu x %zu\n", maxWorkItemSizes[0],
           maxWorkItemSizes[1], maxWorkItemSizes[2]);
    printf("-> Preferred work group size multiple: %zu\n",
           preferredWorkGroupSize);
    printf("===========================================================\n");
}