#ifndef CL_TARGET_OPENCL_VERSION
#define CL_TARGET_OPENCL_VERSION 300
#endif

#include <CL/cl.h>

void printDeviceInfo(cl_device_id device);
void printError(cl_int error, char *message);
void printWorkGroupInfo(size_t groupSize[2], size_t localSize[2]);
cl_uint getNumberOfPlatforms();
cl_uint getNumberOfGPUDevices(cl_platform_id platform);

void listAvailablePlatforms(cl_platform_id platforms[],
                            cl_uint numberOfPlatforms);

cl_device_id getDeviceWithHighestComputeUnits(cl_platform_id platforms[],
                                              cl_uint numberOfPlatforms);

void clCleanUp(cl_device_id device, cl_context context,
             cl_command_queue commandQueue, cl_kernel kernel,
             cl_program program, cl_mem mandelbrotBuffer);