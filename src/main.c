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
    }
}

int main(int argc, char const *argv[]) {
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

    return 0;
}
