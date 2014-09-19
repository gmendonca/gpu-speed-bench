#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ctime>
 
#define MAX_SOURCE_SIZE (0x100000)

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
    int *A = (int*)malloc(sizeof(int)*LIST_SIZE);
    int *B = (int*)malloc(sizeof(int)*LIST_SIZE);
    int *C = (int*)malloc(sizeof(int)*LIST_SIZE);
    
    for(i = 0; i < LIST_SIZE; i++) {
        A[i] = 5;
        B[i] = 7;
        C[i] = 0;
    }

    //Ask to the user, how many interactions he wants to see
    //printf("How many interactions(*1024):\n");
    //scanf("%d",&N);
    N = strtol(argv[1], NULL, 10);

     //Get initial time
    starttime = GetTimeMs();

    for(i = 0; i < LIST_SIZE; i++) {
        for(int j = 0; j < N;j++){
            C[i] += A[i] + B[i];
        }
    }
 
    // Display the result to the screen
    //for(i = 0; i < LIST_SIZE; i++)
        printf("(%d + %d)*%d = %d\n", A[0], B[0], N, C[0]);

    //Get stop time
    stoptime = GetTimeMs();

    printf("Duration= %d ms\n", stoptime - starttime);
    free(A);
    free(B);
    free(C);
    return 0;
}