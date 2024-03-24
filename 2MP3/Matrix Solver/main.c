#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "functions.h"

int main(int argc, char *argv[])
{

    // <Handle the inputs here>
    const char *filename = argv[1];

    CSRMatrix A;
    ReadMMtoCSR(filename, &A);
    

    // Initializing all the vector b (in Ax=b)
    double *b = (double *)malloc(A.num_rows * sizeof(double));
    // Set all elements of b to 1
    for (int i = 0; i < A.num_rows; ++i)
    {
        b[i] = 1.0;
    }

    // <The rest of your code goes here>
    double *y = (double *)malloc(A.num_rows * sizeof(double));
    double *x = (double *)malloc(A.num_rows * sizeof(double));
    double *r = (double *)malloc(A.num_rows * sizeof(double));

    //DisplayCSR(&A);
    
    clock_t start_time, end_time;
    double cpu_time_used;
    start_time = clock();

    jacobiSolver(&A, b, x);

    end_time = clock();
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    //printf("CPU time: %f seconds\n", cpu_time_used);

    //Prints out matrix X
    // printf("\nMatrix X:\n");
    // for (int i = 0; i < A.num_rows; ++i) {
    //     printf("x[%d]=%.6f\n",i,x[i]);
    // }
 
    spmv_csr(&A,x,y);

    //prints out matrix Y
    /*printf("\nMatrix A*X:\n");
    for (int i=0; i < A.num_rows; ++i){
        printf("y[%d]=%.6f\n",i,y[i]);
    }*/
    
    compute_residual(&A, b, y, r);
        
    printf("The matrix name: %s\n",filename);
    printf("The dimension of the matrix: %d by %d\n",A.num_rows,A.num_rows);
    printf("Number of non-zeros: %d\n", A.num_non_zeros);
    printf("CPU time taken to solve Ax=b: %f seconds\n", cpu_time_used);
    compute_norm(&A, r);
    
}