#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int main(void) {
    float irand_max = 1.0/RAND_MAX;
    float **mat; /* ponteiro */
    int i, j, k;/* contadores */
    int l, c; /* dimensoes de mat */
    
    srand(time(NULL));
    
    for ( i=0; i<10; i++ ) {
        printf ("Rodada %d\n",i); /* 1<=dim<=100*/
        l = 1+rand()%100;
        c = 1+rand()%100;
        printf("\tLinhas : %d\n",l);
        printf("\tColunas: %d\n",c);
        mat = (float**)malloc(l*sizeof(float*));
        
        printf("\tEndereco de mat:%p\n",mat);
        
        for ( j=0; j<l; j++ ) {
            mat[j] = (float*)malloc(c*sizeof(float));
            for ( k=0; k<c; k++ ) {
                mat[j][k] = rand()*irand_max;
            }
        }
        for ( j=0; j<l; j++ )
            free(mat[j]);
        free(mat);
    }
return 0;
}
