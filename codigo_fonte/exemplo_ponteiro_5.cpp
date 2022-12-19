#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int main(void) {
   float irand_max = 1.0/RAND_MAX;
   float *lista; /* ponteiro */
   int i, j; /* contadores */
   int dim; /* dimensao de lista */

   srand(time(NULL));

   for ( i=0; i<10; i++ ) {
      printf ("Rodada %d\n",i);
      /* 1<=dim<=100*/
      dim = 1+rand()%100;
      printf("\tComprimento sorteado: %d\n",dim);
      //lista = (float*)malloc(dim*sizeof(float));
      lista = (float*) new float[dim];

      printf("\tEndereco de lista:%p\n",lista);
      for ( j=0; j<dim; j++ ) {
         lista[j] = rand()*irand_max;
         printf("\t\t%7.4f",lista[j]);
      }
      printf("\n");
      delete [] lista;
      //free(lista);
   }
   return 0;
}
