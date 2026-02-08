#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>
#include <time.h>

typedef struct {
    double **a;
    double *data;
    int m,n;
} matrix;


matrix* allocate_matrix (int m, int n){
    matrix *mat = (matrix*) malloc(sizeof(matrix));
    mat->m = m;
    mat->n = n;
    mat->a = (double**)malloc(m*sizeof(double*));
    if (mat->a == NULL)return NULL;
    mat->data = (double*)malloc(m*n*sizeof(double));
    if (mat->data == NULL)return NULL;
    for (int i=0;i<m;i++) mat->a[i]=mat->data+i*n;
    return mat;
}


void free_matrix (matrix *mat){
    if (mat == NULL)return;
    free (mat->a);
    free (mat->data);
    free(mat);
}


int mult_matrix (matrix *A, matrix *B, matrix *C){

    // Q3
    // Nombre d'itérations : mn * (p + p - 1)

    if (A == NULL || B == NULL || C == NULL){
        return -1;
    }

    if (A->m != C->m || A->n != B->m || B->n != C->n){
        return -1;
    }

    double **a = A->a;
    double **b = B->a;
    double **c = C->a;

    for (int i = 0; i < A->m; i++){
        for (int j = 0; j < B->n; j++){
            double sum = 0;
            for (int k = 0; k < A->n; k++){
                sum += a[i][k]*b[k][j];
            }
            c[i][j] = sum;
        }
    }

    return 0;
}


// Q4
// 16 coeurs
// 8 flop par cycle et par coeur
// frequence typique : 2.7 GhZ
// FLOPS = 345.6 GFLOPS

// Q5
// wall clock time : 35s
// temps théorique : FLOP / FLOPS = 1000*3000*2*2000 / 345.6 * 10^9 = 0.0347 s

// Q6
// pas opti dans l'acces memoire
// d'autres choses tournent sur le pc
// n'utilise pas tous les coeurs


int main(void){

    matrix *A = allocate_matrix(1000,2000);
    if (A == NULL) return -1;
    matrix *B = allocate_matrix(2000,3000);
    if (B == NULL) return -1;
    matrix *C = allocate_matrix(1000,3000);
    if (C == NULL) return -1;

    double **a = A->a;
    for (int i=0;i<1000;i++){
        for (int j=0;j<2000;j++){
            a[i][j] = (double) rand() / RAND_MAX;;
        }
    }

    double **b = B->a;
    for (int i=0;i<2000;i++){
        for (int j=0;j<3000;j++){
            b[i][j] = (double) rand() / RAND_MAX;;
        }
    }

    struct timespec t1, t2;

    clock_gettime(CLOCK_MONOTONIC, &t1);
    int result = mult_matrix(A,B,C);
    clock_gettime(CLOCK_MONOTONIC, &t2);

    double t_naif = (t2.tv_sec - t1.tv_sec) + 1e-9 * (t2.tv_nsec - t1.tv_nsec);
    printf("Temps naive : %.6f s\n", t_naif);

    if (result == -1) {
        return -1;
    }

    free_matrix(C);
    C = allocate_matrix(1000,3000);
    if (C == NULL) return -1;

    clock_gettime(CLOCK_MONOTONIC, &t1);
    cblas_dgemm(
        CblasRowMajor,
        CblasNoTrans,
        CblasNoTrans,
        A->m,        // M
        B->n,        // N
        A->n,        // K
        1.0,         // alpha
        A->data,     // A
        A->n,        // lda
        B->data,     // B
        B->n,        // ldb
        0.0,         // beta
        C->data,     // C
        C->n         // ldc
    );
    clock_gettime(CLOCK_MONOTONIC, &t2);

    double t_blas = (t2.tv_sec - t1.tv_sec) + 1e-9 * (t2.tv_nsec - t1.tv_nsec);
    printf("Temps BLAS  : %.6f s\n", t_blas);

    printf("Speedup : x%.1f\n", t_naif / t_blas);

    free_matrix(A);
    free_matrix(B);
    free_matrix(C);

    A = NULL;
    B = NULL;
    C = NULL;

    return 0;
}