#ifndef CL_TARGET_OPENCL_VERSION
#define CL_TARGET_OPENCL_VERSION 300
#endif

#include <CL/cl.h>

cl_uint getNumberOfPlatforms();

cl_uint getNumberOfGPUDevices(cl_platform_id platform);

int getNumberOfAllGPUDevices(cl_platform_id platforms[], int numberOfPlatforms);

void listAvailablePlatforms(cl_platform_id platforms[],
                            cl_uint numberOfPlatforms);

void getAllDevices(cl_platform_id platforms[], cl_uint numberOfPlatforms,
                   cl_device_id devices[]);