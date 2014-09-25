__kernel void simplefloatadd(__global float *sum, const int N){

	// Get the index of the current element to be processed
    int i = get_global_id(0);

    // Add the corresponding locations of   
    // 'A' and 'B', and store the result in 'C'.
    for(int j = 0; j < N; j++){
    	sum[i] += 0.3;
    }

}