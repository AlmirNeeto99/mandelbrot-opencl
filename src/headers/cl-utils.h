#ifndef CL_TARGET_OPENCL_VERSION
#define CL_TARGET_OPENCL_VERSION 300
#endif

#include <CL/cl.h>

cl_uint getNumberOfPlatforms();

cl_uint getNumberOfGPUDevices(cl_platform_id platform);

void listAvailablePlatforms(cl_platform_id platforms[],
                            cl_uint numberOfPlatforms);

cl_device_id getDeviceWithHighestComputeUnits(cl_platform_id platforms[],
                                              cl_uint numberOfPlatforms);

void printDeviceInfo(cl_device_id device);