__kernel void memorycopy(__global char *source,__global char *copy){

    	memcpy (copy, source, strlen(source)+1);
	}

}