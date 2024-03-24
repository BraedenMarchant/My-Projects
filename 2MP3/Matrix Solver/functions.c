#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"
#include <math.h>
#define BUFFER 300

struct CSRMatrix {
    double *csr_data;   // Array of non-zero values
    int *col_ind;       // Array of column indices
    int *row_ptr;       // Array of row pointers
    int num_non_zeros;  // Number of non-zero elements
    int num_rows;       // Number of rows in matrix
    int num_cols;       // Number of columns in matrix    
};

struct SolverMatrix {
    double *csr_data;   // Array of non-zero values
    int *col_ind;       // Array of column indices
    int *row_ptr;       // Array of row pointers
    int num_non_zeros;  // Number of non-zero elements
    int num_rows;       // Number of rows in matrix
    int num_cols;       // Number of columns in matrix    
};

struct MarketMatrix {
    int row;
    int column;
    double value;
};


void sortMarketMatrix(int size, struct MarketMatrix mm[size], int totalRows){
    struct MarketMatrix *tempMM = malloc(size * sizeof(struct MarketMatrix));
    int index = 0;

    //Sort mm by mm.row value and put into temp array
    //Start from first row (represented by i) and group struct values with the same row into a temp struct
    
    for(int i = 1; i <= totalRows; i++){
        int j = 0; 

        while(j < size){ 
            if(mm[j].row == i){
                tempMM[index] = mm[j];
                index++;
            }
            j++;
        }
    }  

    //Replace unsorted market matrix struct with sorted temporary struct
    for(int i = 0; i < size; i++){
        mm[i] = tempMM[i];
    } 

    free(tempMM); 
    
}


void ReadMMtoCSR(const char *filename, CSRMatrix *matrix){
    struct MarketMatrix *marketMatrix;
    
    FILE * file = fopen (filename, "r");
    if ( file == NULL ) {
        printf (" Failed to open file %s: \n", filename);
        return;
    } 

    char line[BUFFER];
    int mmRowCount = 0;
    int mmCount = 0;
    while (fgets(line, BUFFER, file) != NULL)
    {   
        //FOR DEBUG
        //printf("line: %s\n", line);
        
        //Ignore comment lines
        if(line[0] != '%'){
            
            mmRowCount++;
            
            //If first row, set applicable matrix info in CSR
            if(mmRowCount == 1){
                sscanf(line, "%d %d %d", &matrix->num_rows, &matrix->num_cols, &matrix->num_non_zeros);

                //printf("rows: %d, columns: %d, non-zeroes: %d", matrix->num_rows, matrix->num_cols, matrix->num_non_zeros);

                //allocate memory for arrays
                matrix->csr_data = (double*)malloc(matrix->num_non_zeros * sizeof(double));
                matrix->col_ind = (int*)malloc(matrix->num_non_zeros * sizeof(int));
                matrix->row_ptr = (int*)malloc((matrix->num_rows + 1) * sizeof(int));
                
                marketMatrix = malloc(matrix->num_non_zeros * sizeof(struct MarketMatrix));

                continue;
            }
                        
            sscanf(line, "%d %d %lf", &marketMatrix[mmCount].row, &marketMatrix[mmCount].column, &marketMatrix[mmCount].value);
            mmCount++;
        }
    }

    // printf("\nUNSORTED: \n");
    // for(int i = 0; i < matrix->num_non_zeros; i++){
    //     printf("%d %d %lf\n", mm[i].row, mm[i].column, mm[i].value);
    // }
    
    //Sort Market Matrix in row ascending order to correctly retrieve values to set in CSR
    sortMarketMatrix(matrix->num_non_zeros, marketMatrix, matrix->num_rows); 

    // printf("\nSORTED: \n");
    // for(int i = 0; i < matrix->num_non_zeros; i++){
        
    //     printf("%d %d %lf\n", mm[i].row, mm[i].column, mm[i].value);
    // }

    int nzTotalInRow = 1; //counts the number of non-zero values in a row
    int nzRowIndex = 0; //
    int *nzRowSet = malloc(matrix->num_rows * sizeof(int)); //stores the total number of non-zero values per row
    
    //Set CSR values and do initial calculation for row pointer values
    for(int i = 0; i < matrix->num_non_zeros; i++){
        matrix->col_ind[i] = marketMatrix[i].column-1;
        matrix->csr_data[i] = marketMatrix[i].value;
   
        if(marketMatrix[i].row == marketMatrix[i+1].row){
            nzTotalInRow++; //count number of non-zero values in row
        } else {
            nzRowSet[nzRowIndex] = nzTotalInRow;
            nzTotalInRow = 1;

            if(nzRowIndex < matrix->num_rows - 1){
                nzRowIndex++;
            }
        }
    }

    //Calculate row pointer values
    for(int i = 0; i <= nzRowIndex + 1; i++){ 
        //printf("i: %d, nzRowSet: %d\n", i, nzRowSet[i]);    
        if(i == 0){
            matrix->row_ptr[i] = 0;  
        } else {
            matrix->row_ptr[i] = matrix->row_ptr[i-1] + nzRowSet[i-1];
        }
    }

    //free memmory
    free(nzRowSet);
    free(marketMatrix);
    fclose(file);
}


void spmv_csr(const CSRMatrix *A, const double *x, double *y){
    for (int i=0; i < A->num_rows; ++i){
        y[i]=0.0;
        // goes through each row using the row ptr and multiplies the value of A by each of the x columns 
        for (int j = A->row_ptr[i]; j < A->row_ptr[i+1]; j++){
            y[i] += (A->csr_data[j] * x[A->col_ind[j]]);
        }
    }
}


void DisplayCSR(const CSRMatrix *A){
    // printf("matrix num row: %d, matrix num col: %d, matrix num non zeros: %d\n", matrix->num_rows, matrix->num_cols, matrix->num_non_zeros);
    printf("\nNumber of non-zeros: %d\n", A->num_non_zeros);
    printf("Row Pointer: ");
    for(int i = 0; i < (A->num_rows + 1); i++){
        printf("%d ", A->row_ptr[i]);
    }

    printf("\nColumn Index: ");
    for(int i = 0; i < A->num_non_zeros; i++){
        printf("%d ", A->col_ind[i]);
    }

    printf("\nValues: ");
    for(int i = 0; i < A->num_non_zeros; i++){
        printf("%lf ", A->csr_data[i]);
    }

}


#define MAX_ITER 1000
#define EPSILON 1e-6

//Function to perform Jacobi iteration
void jacobiSolver(const CSRMatrix *A,const double *b, double *x) {
    int n=A->num_rows;
    double *x_new = (double *)malloc(n * sizeof(double));
    int k;

    for (k = 0; k < MAX_ITER; ++k) {
        for (int i = 0; i < n; ++i) {
            double sigma = 0.0;
            for (int j = A->row_ptr[i]; j < A->row_ptr[i + 1]; ++j) {
                if (A->col_ind[j] != i) {
                    sigma += A->csr_data[j] * x[A->col_ind[j]];
                }
            }
            x_new[i] = (b[i] - sigma) / A->csr_data[A->row_ptr[i + 1] - 1];
        }

        // Check for convergence
        double max_diff = 0.0;
        for (int i = 0; i < n; ++i) {
            double diff = fabs(x_new[i] - x[i]);
            if (diff > max_diff) {
                max_diff = diff;
            }
        }

        //prints num iterations after convergence is reached
        if (max_diff < EPSILON) {
            printf("Convergence reached after %d iterations.\n", k + 1);
            break;
        }

        // Update x for the next iteration
        for (int i = 0; i < n; ++i) {
            x[i] = x_new[i];
        }
    }
    free(x_new);
}




void compute_residual(const CSRMatrix *A, double *b, double *y, double *r){
    //double *y = (double *)malloc(A->num_rows * sizeof(double));
    //spmv_csr(A,x,y);
    
    for (int i = 0; i < A->num_rows; ++i){
        r[i]=y[i]-b[i];
        //printf("r[%d]=%f\n",i,r[i]);
    }
    //free(y);
}


void compute_norm(const CSRMatrix *A, double *r){
    double mag = 0.0;
    double sum = 0.0;
    for (int i = 0; i < A->num_rows; ++i){
        sum += r[i]*r[i];
        //printf("sum:%f",sum);
        mag = sqrt(sum);
    }
    printf("Residual Norm: %f\n", mag);
}





