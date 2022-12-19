#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <iostream>

int main(void) {/* inicio */

/* numero aleatorio */
int eps;

/* variavel auxiliar */
int i, n;


printf("Entre com o numero de sorteios: ");
scanf("%d", &n);

srand(time(NULL));

/* funcao geradora de numeros
aleatorios inteiros: rand() */

/* a funcao rand() gera inteiro
entre 0 e a constante RAND_MAX */

printf("RAND_MAX: %d\n",RAND_MAX);

    for (/*ini*/ i=0;/*cond*/ i<n;/**/ i++ ) {

        eps = rand();

        if (eps<RAND_MAX/10) {

            printf("\t(%d) eps=%5d menor que RAND_MAX/10\n",i,eps);

        } else if (eps<RAND_MAX/2) {

            printf("\t(%d) eps=%5d menor que RAND_MAX/2\n",i,eps);

        } else {

            printf("\t(%d) eps=%5d\n",i,eps);
        }
    }
    
    printf("\n%ld",time(NULL));
    std::cout << "\n" << time(NULL);

return 0;
} /* fim */
