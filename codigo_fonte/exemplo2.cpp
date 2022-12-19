#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int main(void) {

/* numero aleatorio */
float eps;
/* variavel auxiliar */
int i, n;

printf("Entre com o numero de sorteios: ");
scanf("%d", &n);

/* a funcao srand gera semente para
a geracao de numeros aleatorios */
srand(time(NULL));
/* funcao geradora de numeros
aleatorios inteiros: rand() */
/* a funcao rand() gera inteiro
entre 0 e a constante RAND_MAX */
printf("RAND_MAX: %d\n",RAND_MAX);

    for ( i=0; i<n; i++ ) {
        eps = (float)rand()/RAND_MAX;
        if (eps<0.1) {
        printf("\t(%d)\teps=%5.3f menor que 0.1\n",i, eps);
        } else if (eps<0.5) {
        printf("\t(%d)\teps=%5.3f menor que 0.5\n",i, eps);
        } else {
        printf("\t(%d)\teps=%5.3f\n",i, eps);
        }
    }
return 0;
}
