#include <stdio.h>
#include <math.h>

// Petit programme qui montre que le langage C utilise la convention “row-major”

int main(void){
    double A[2][3] = {{1,2,3},{4,5,6}};
    double *a = (double*)A;
    printf("A[3] = %12.5E\n", a[3]);
    return 0;
}
