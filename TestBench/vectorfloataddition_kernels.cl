__kernel void floatadd(__global float *A, __global float *B, __global float *C, const int N){

    // Get the work-item's unique ID
    int idx = get_global_id(0);

    // Add the corresponding locations of   
    // 'A' and 'B', and store the result in 'C'.
    for(int j = 0; j < N;j++){
    C[idx] += A[idx] + B[idx];
    }

}