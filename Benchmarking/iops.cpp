#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ctime>
 
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.hpp>
#endif
 
#define MAX_SOURCE_SIZE (0x100000)
 
int main( int argc, char *argv[] ) {
    int i, wgs;
    long N;
    const int LIST_SIZE = 1024;
    int *A = (int*)malloc(sizeof(int)*LIST_SIZE);
    int *B = (int*)malloc(sizeof(int)*LIST_SIZE);
    int *C = (int*)malloc(sizeof(int)*LIST_SIZE);

    for(i = 0; i < LIST_SIZE; i++) {
        A[i] = 5;
        B[i] = 7;
    }

    N = atoi(argv[1]);
    wgs = atoi(argv[2]);
 
    // Load the kernel source code into the array source_str
    FILE *fp;
    char *source_str;
    size_t source_size;
 
    fp = fopen("normaladd_kernels.cl", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );
 
    cl_uint ret_num_devices = 0;
    cl_uint ret_num_platforms = 0;

    // Use this to check the output of each API call
    cl_int ret;

    // Retrieve the number of platforms
    ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);
    if (ret < 0) {
        printf("Error Platform = %d\n", ret);
        return -1;
    }

    // Allocate enough space for each platform
    cl_platform_id *platforms = NULL;
    platforms = (cl_platform_id*)malloc(ret_num_platforms*sizeof(cl_platform_id));

    // Fill in the platforms
    ret = clGetPlatformIDs(ret_num_platforms, platforms, NULL);
    if (ret < 0) {
        printf("Error Platform = %d\n", ret);
        return -1;
    }

    // Retrieve the number of devices
    ret = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, NULL, &ret_num_devices);
    if (ret < 0) {
        printf("Error Device = %d\n", ret);
        return -1;
    }

    // Allocate enough space for each device
    cl_device_id *devices;
    devices = (cl_device_id*)malloc(ret_num_devices*sizeof(cl_device_id));

    // Fill in the devices
    ret = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, ret_num_devices, devices,  NULL);
    if (ret < 0) {
        printf("Error Device = %d\n", ret);
        return -1;
    }
 
    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, ret_num_devices, devices, NULL, NULL, &ret);
    if (ret < 0) {
        printf("Error Context = %d\n", ret);
        return -1;
    }
 
    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, devices[0], CL_QUEUE_PROFILING_ENABLE, &ret);
    if (ret < 0) {
        printf("Error Command Queue= %d\n", ret);
        return -1;
    }

    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, LIST_SIZE * sizeof(int), NULL, &ret);
    cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, LIST_SIZE * sizeof(int), NULL, &ret);
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, LIST_SIZE * sizeof(int), NULL, &ret);

    ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), A, 0, NULL, NULL);
    if (ret < 0) {
        printf("Error Buffer= %d\n", ret);
        return -1;
    }
    ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), B, 0, NULL, NULL);
    if (ret < 0) {
        printf("Error Buffer= %d\n", ret);
        return -1;
    }
    ret = clEnqueueWriteBuffer(command_queue, c_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), C, 0, NULL, NULL);
    if (ret < 0) {
        printf("Error Buffer= %d\n", ret);
        return -1;
    }
 
    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
    if (ret < 0) {
        printf("Error Program = %d\n", ret);
        return -1;
    }
 
    // Build the program
    ret = clBuildProgram(program, ret_num_devices, devices, NULL, NULL, NULL);
    if (ret < 0) {
        printf("Error Source = %d\n", ret);
        return -1;
    }
 
    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "intadd", &ret);
    if (ret < 0) {
        printf("Error Kernel = %d\n", ret);
        return -1;
    }
 
    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(int), &N);
    if (ret < 0) {
        printf("Error Arguments = %d\n", ret);
        return -1;
    }
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&a_mem_obj);
    if (ret < 0) {
        printf("Error Arguments = %d\n", ret);
        return -1;
    }
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&b_mem_obj);
    if (ret < 0) {
        printf("Error Arguments = %d\n", ret);
        return -1;
    }
    ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&c_mem_obj);
    if (ret < 0) {
        printf("Error Arguments = %d\n", ret);
        return -1;
    }
 
    // Execute the OpenCL kernel on the list
    size_t global_item_size = LIST_SIZE; // Process the entire lists
    size_t global_work_offset = wgs;

    cl_event event;
    double elapsed = 0;
    cl_ulong time_start, time_end;

    for (long k = 0; k < N; k++){
        ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &global_work_offset, 0, NULL, &event);
        if (ret < 0) {
            printf("Error NDRange = %d\n", ret);
            return -1;
        }
    }

    ret = clWaitForEvents(1, &event);
    if (ret < 0) {
        printf("Error Events = %d\n", ret);
        return -1;
    }


    ret = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
    if (ret < 0) {
        printf("Error Start = %d\n", ret);
        return -1;
    }

    clFinish(command_queue);

    ret = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
    if (ret < 0) {
        printf("Error Stop = %d\n", ret);
        return -1;
    }
    elapsed += (time_end - time_start);

    //printf("Each iteration 0.3*%ld = %.1f\n", N, C[0]);
    printf("Elaplsed time = %.3f ms\n", elapsed/1000000);
    printf("FLOP = %ld\n", 1024*N);
    printf("GFLOPS = %.6f\n", (double)(1024*N)/(elapsed));
 
    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(a_mem_obj);
    ret = clReleaseMemObject(b_mem_obj);
    ret = clReleaseMemObject(c_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(A);
    free(B);
    free(C);
    return 0;
}