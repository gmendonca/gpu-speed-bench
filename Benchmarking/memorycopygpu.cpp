#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ctime>
#include <string.h>
 
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.hpp>
#endif
 
#define MAX_SOURCE_SIZE (0x1000000000)

int GetTimeMs()
{
 /* Linux */
 struct timeval tv;

 gettimeofday(&tv, NULL);

 unsigned int ret = tv.tv_usec;
 /* Convert from micro seconds (10^-6) to milliseconds (10^-3) */
 //ret /= 1000;

 /* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
 ret += (tv.tv_sec * 1000);

 return ret;
}
 
int main( int argc, char *argv[] ) {
    // Create the variables for the time measure
    int starttime, stoptime;
    // Create the two input vectors and instance the output vector
    int i, N, lSize;
    const int LIST_SIZE = 1024;
 
    // Load the kernel source code into the array source_str
    FILE *fp;
    char *source_str;
    char *source;
    char *copy;
    size_t source_size, sourcesize;

    if(strcmp(argv[1], "1b") == 0)fp = fopen("txt/1b.txt", "rb");
    else if(strcmp(argv[1], "1kb") == 0)fp = fopen("txt/1kb.txt", "rb");
    else if(strcmp(argv[1], "1mb") == 0)fp = fopen("txt/1mb.txt", "rb");
    else if(strcmp(argv[1], "10mb") == 0)fp = fopen("txt/1mb.txt", "rb");
    else fp = fopen("txt/alice.txt", "rb");
    //printf("%s\n", argv[1]);
    if (!fp) {
        fprintf(stderr, "Failed to load buffer.\n");
        exit(1);
    }
    fseek( fp , 0L , SEEK_END);
    lSize = ftell( fp );
    rewind( fp );

    /* allocate memory for entire content */
    source = (char *)calloc( 1, lSize+1 );
    if( !source ){
        fclose(fp);
        printf("Couldn't allocate memory\n");
        return -1;
    }

    /* copy the file into the buffer */
    if( fread( source , lSize, 1 , fp) != true){
        fclose(fp);
        free(source);
        printf("Couldn't copy in buffer\n");
        return -1;
    }

    fclose(fp);
    //printf("source = %s\n", source);

    copy = (char *)calloc( 1, lSize+1 );
 
    cl_uint ret_num_devices = 0;
    cl_uint ret_num_platforms = 0;

    // Use this to check the output of each API call
    cl_int ret;

    // Retrieve the number of platforms
    ret = clGetPlatformIDs(0, NULL, &ret_num_platforms);

    // Allocate enough space for each platform
    cl_platform_id *platforms = NULL;
    platforms = (cl_platform_id*)malloc(ret_num_platforms*sizeof(cl_platform_id));

    // Fill in the platforms
    ret = clGetPlatformIDs(ret_num_platforms, platforms, NULL);

    // Retrieve the number of devices
    ret = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 0, NULL, &ret_num_devices);

    // Allocate enough space for each device
    cl_device_id *devices;
    devices = (cl_device_id*)malloc(ret_num_devices*sizeof(cl_device_id));

    // Fill in the devices
    ret = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, ret_num_devices, devices,  NULL);
 
    // Create an OpenCL context
    cl_context context = clCreateContext( NULL, ret_num_devices, devices, NULL, NULL, &ret);
 
    // Create a command queue
    cl_command_queue command_queue = clCreateCommandQueue(context, devices[0], 0, &ret);
 
    // Create memory buffers on the device for each vector 
    cl_mem str_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, lSize * sizeof(char), NULL, &ret);
    cl_mem read_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, lSize * sizeof(char), NULL, &ret);

    starttime = GetTimeMs();
    //write file on the memory
    ret = clEnqueueWriteBuffer(command_queue, str_mem_obj, CL_TRUE, 0, lSize * sizeof(char), source, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, read_mem_obj, CL_TRUE, 0, lSize * sizeof(char), copy, 0, NULL, NULL);
    stoptime = GetTimeMs();

    printf("Write Speed= %d us\n", stoptime - starttime);

    char src[8192];
    FILE *fil=fopen("copystring_kernels.cl","r");
    source_size=fread(src, sizeof src, 1, fil);
    fclose(fil);

    const char *srcptr[]={src};
    /* Submit the source code of the kernel to OpenCL, and create a program object with it */
    cl_program program=clCreateProgramWithSource(context, 1, srcptr, &source_size, &ret);
 
    // Build the program
    ret = clBuildProgram(program, ret_num_devices, devices, NULL, NULL, NULL);


    cl_kernel kernel = clCreateKernel(program, "memorybuffercopy", &ret);
    // Set the arguments of the kernel
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&str_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&read_mem_obj);

    // Execute the OpenCL kernel on the list
    size_t global_item_size = lSize; // Process the entire lists
    //size_t global_work_offset = 64; //Divide work-groups
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, NULL, 0, NULL, NULL);

    starttime = GetTimeMs();
    //read file on the memory
    ret = clEnqueueReadBuffer(command_queue, read_mem_obj, CL_TRUE, 0, lSize * sizeof(char), copy, 0, NULL, NULL);

    //printf("copy = %s\n", copy);
    
    //Get stop time
    stoptime = GetTimeMs();

    printf("Read speed= %d us\n", stoptime - starttime);
 
    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(str_mem_obj);
    ret = clReleaseMemObject(read_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(source);
    free(copy);
    return 0;
}