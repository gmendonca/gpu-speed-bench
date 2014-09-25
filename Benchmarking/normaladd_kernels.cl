__kernel void floatadd(const int N, __global float *A, __global float *B, __global float *C){

    // Get the work-item's unique ID
    int i = get_global_id(0);
    
    int x = 0;
    for(long j = 0; j < N;j++){
        C[i] = A[i] + B[i];
    }
}

__kernel void intadd(const int N, __global int *A, __global int *B, __global int *C){

    // Get the work-item's unique ID
    int i = get_global_id(0);
    
    int x = 0;
    for(long j = 0; j < N;j++){
        for(long k = 0; k < N;k++){
            x += 5;
        }
    }

    C[i] = x;
}