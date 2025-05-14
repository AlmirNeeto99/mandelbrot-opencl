#include "headers/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void readKernelSource(const char *filename, char **source) {
    char fullPath[256] = "src/kernels/";
    strcat(fullPath, filename);
    strcat(fullPath, ".cl");
    printf("-> Reading kernel from path: %s\n", fullPath);
    FILE *file = fopen(fullPath, "r");
    if (file == NULL) {
        fprintf(stderr, "\t-> Error: Unable to open kernel source file %s\n",
                filename);
        return;
    }
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    *source = (char *)malloc((fileSize + 1) * sizeof(char));
    if (*source == NULL) {
        fprintf(stderr,
                "\t ->Error: Unable to allocate memory for kernel source\n");
        fclose(file);
        return;
    }

    fread(*source, sizeof(char), fileSize, file);
    (*source)[fileSize] = '\0';

    fclose(file);
}

int *create2DSpace(int width, int height) {
    int *space = (int *)malloc(width * height * sizeof(int));
    if (space == NULL) {
        printf("-> Error allocating memory for 2D space!\n");
        return NULL;
    }

    return space;
}

void logExperiment(int numberComputeUnits, size_t globalGroupSize[2],
                   size_t localGroupSize[2], int maxNumberOfIterations,
                   double elapsedTime) {
    printf("-> Elapsed time: %lf\n", elapsedTime);

    FILE *experimentFile = fopen("experiments.csv", "a");

    if (!experimentFile) {
        printf("-> Unable to open experiment file.\n");
        return;
    }

    fprintf(experimentFile, "%d,\"[%zu,%zu]\",\"[%zu,%zu]\",%d,%lf\n",
            numberComputeUnits, globalGroupSize[0], globalGroupSize[1],
            localGroupSize[0], localGroupSize[1], maxNumberOfIterations,
            elapsedTime);
    fclose(experimentFile);
}