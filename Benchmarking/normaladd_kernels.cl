__kernel void floatadd(__global float *C, const int N){

    // Get the work-item's unique ID
    int i = get_global_id(0);
    
    for(long j = 0; j < N;j++){
    C[i] += 0.7;
    }

}

__kernel void intadd(__global int *C, const int N){

    // Get the work-item's unique ID
    int i = get_global_id(0);
    
    for(long j = 0; j < N;j++){
    C[i] += 5;
    }

}