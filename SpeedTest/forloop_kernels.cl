__kernel void vector_sum(__global const int *A, __global const int *B, __global int *C) {
 
    // Get the index of the current element to be processed
    int i = get_global_id(0);
 	
 	int sum = A[i]+B[i];
    for(int j = 0;j < 1000; j++){
    	for(int k = 0;k < 1000000; k++){
    		sum += 1;
    	}
    }
    C[i] = sum;
}