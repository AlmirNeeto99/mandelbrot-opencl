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

void printError(cl_int error, char *message) {
    switch (error) {
        case CL_SUCCESS:
            printf("-> Success | %s!\n", message);
            return;
        case CL_DEVICE_NOT_FOUND:
            printf("-> Device not found!\n");
            break;
        case CL_DEVICE_NOT_AVAILABLE:
            printf("-> Device not available!\n");
            break;
        case CL_COMPILER_NOT_AVAILABLE:
            printf("-> Compiler not available!\n");
            break;
        case CL_MEM_OBJECT_ALLOCATION_FAILURE:
            printf("-> Memory object allocation failure!\n");
            break;
        case CL_OUT_OF_RESOURCES:
            printf("-> Out of resources!\n");
            break;
        case CL_OUT_OF_HOST_MEMORY:
            printf("-> Out of host memory!\n");
            break;
        case CL_PROFILING_INFO_NOT_AVAILABLE:
            printf("-> Profiling information not available!\n");
            break;
        case CL_MEM_COPY_OVERLAP:
            printf("-> Memory copy overlap!\n");
            break;
        case CL_IMAGE_FORMAT_MISMATCH:
            printf("-> Image format mismatch!\n");
            break;
        case CL_INVALID_VALUE:
            printf("-> Invalid value!\n");
            break;
        case CL_INVALID_DEVICE_TYPE:
            printf("-> Invalid device type!\n");
            break;
        case CL_INVALID_PLATFORM:
            printf("-> Invalid platform!\n");
            break;
        case CL_INVALID_DEVICE:
            printf("-> Invalid device!\n");
            break;
        case CL_INVALID_CONTEXT:
            printf("-> Invalid context!\n");
            break;
        case CL_INVALID_QUEUE_PROPERTIES:
            printf("-> Invalid queue properties!\n");
            break;
        case CL_INVALID_COMMAND_QUEUE:
            printf("-> Invalid command queue!\n");
            break;
        case CL_INVALID_HOST_PTR:
            printf("-> Invalid host pointer!\n");
            break;
        case CL_INVALID_MEM_OBJECT:
            printf("-> Invalid memory object!\n");
            break;
        case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
            printf("-> Invalid image format descriptor!\n");
            break;
        case CL_INVALID_IMAGE_SIZE:
            printf("-> Invalid image size!\n");
            break;
        case CL_INVALID_SAMPLER:
            printf("-> Invalid sampler!\n");
            break;
        case CL_INVALID_BINARY:
            printf("-> Invalid binary!\n");
            break;
        case CL_INVALID_BUILD_OPTIONS:
            printf("-> Invalid build options!\n");
            break;
        case CL_INVALID_PROGRAM:
            printf("-> Invalid program!\n");
            break;
        case CL_INVALID_PROGRAM_EXECUTABLE:
            printf("-> Invalid program executable!\n");
            break;
        case CL_INVALID_KERNEL_NAME:
            printf("-> Invalid kernel name!\n");
            break;
        case CL_INVALID_KERNEL_DEFINITION:
            printf("-> Invalid kernel definition!\n");
            break;
        case CL_INVALID_KERNEL:
            printf("-> Invalid kernel!\n");
            break;
        case CL_INVALID_ARG_INDEX:
            printf("-> Invalid argument index!\n");
            break;
        case CL_INVALID_ARG_VALUE:
            printf("-> Invalid argument value!\n");
            break;
        case CL_INVALID_ARG_SIZE:
            printf("-> Invalid argument size!\n");
            break;
        case CL_INVALID_KERNEL_ARGS:
            printf("-> Invalid kernel arguments!\n");
            break;
        case CL_INVALID_WORK_DIMENSION:
            printf("-> Invalid work dimension!\n");
            break;
        case CL_INVALID_WORK_GROUP_SIZE:
            printf("-> Invalid work group size!\n");
            break;
        case CL_INVALID_WORK_ITEM_SIZE:
            printf("-> Invalid work item size!\n");
            break;
        case CL_INVALID_GLOBAL_OFFSET:
            printf("-> Invalid global offset!\n");
            break;
        case CL_INVALID_EVENT_WAIT_LIST:
            printf("-> Invalid event wait list!\n");
            break;
        case CL_INVALID_EVENT:
            printf("-> Invalid event!\n");
            break;
        case CL_INVALID_OPERATION:
            printf("-> Invalid operation!\n");
            break;
        case CL_INVALID_GL_OBJECT:
            printf("-> Invalid GL object!\n");
            break;
        case CL_INVALID_BUFFER_SIZE:
            printf("-> Invalid buffer size!\n");
            break;
        case CL_INVALID_MIP_LEVEL:
            printf("-> Invalid mip level!\n");
            break;
        case CL_INVALID_GLOBAL_WORK_SIZE:
            printf("-> Invalid global work size!\n");
            break;
    }

    printf("-> %s\n", message);
    printf("===========================================================\n");
}
void printWorkGroupInfo(size_t globalSize[2], size_t localSize[2]) {
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
}