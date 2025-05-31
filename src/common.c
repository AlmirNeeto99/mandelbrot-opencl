#include "headers/common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <png.h>

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

int save_result_to_png(const char *fileName, int *iterationsSpace, int width,
                       int height, int maxIterations) {
    FILE *mandelbrot = fopen(fileName, "wb");
    if (!mandelbrot) {
        printf("-> Unable to open file %s for writing.\n", fileName);
        return -1;
    }

    png_structp png =
        png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fclose(mandelbrot);
        return -1;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, NULL);
        fclose(mandelbrot);
        return -1;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(mandelbrot);
        return -1;
    }

    png_set_compression_level(png, 0);
    png_init_io(png, mandelbrot);
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    png_bytep row = (png_bytep)malloc(3 * width * sizeof(png_byte));
    int color = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            color = (int)(((double)iterationsSpace[x * height + y] /
                           maxIterations) *
                          255);
            row[x * 3] = color;      // Red
            row[x * 3 + 1] = color;  // Green
            row[x * 3 + 2] = color;  // Blue
        }
        png_write_row(png, row);
    }
    free(row);

    png_write_end(png, NULL);
    png_destroy_write_struct(&png, &info);
    fclose(mandelbrot);

    return 0;
}