#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ctime>
 
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
 ret /= 1000;

 /* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
 ret += (tv.tv_sec * 1000);

 return ret;
}
 
int main( int argc, char *argv[] ) {
    // Create the variables for the time measure
    int starttime, stoptime;
    // Create the two input vectors and instance the output vector
    int i, N;
    const int LIST_SIZE = 1024;
 
    // Load the kernel source code into the array source_str
    FILE *fp;
    char *source_str;
    char *get_str;
    size_t source_size;
 
    fp = fopen("txt/1b.txt", "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

     //Get initial time
    starttime = GetTimeMs();
 
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
    cl_mem str_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, 
            source_size * sizeof(int), NULL, &ret);
    cl_mem read_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, 
            source_size * sizeof(int), NULL, &ret);

    //write file on the memory
    ret = clEnqueueWriteBuffer(command_queue, str_mem_obj, CL_TRUE, 0,
            source_size * sizeof(int), source_str, 0, NULL, NULL);

    //read file on the memory
    ret = clEnqueueReadBuffer(command_queue, read_mem_obj, CL_TRUE, 0, 
            source_size * sizeof(int), get_str, 0, NULL, NULL);
    
    //Get stop time
    stoptime = GetTimeMs();

    printf("Duration= %d ms\n", stoptime - starttime);
 
    // Clean up
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseMemObject(str_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(source_str);
    free(get_str);
    return 0;
}