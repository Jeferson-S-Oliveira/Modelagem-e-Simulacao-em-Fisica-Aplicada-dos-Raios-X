#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int main(void) {

    /* numero aleatorio */
    float eps;
    /* variavel auxiliar */
    int i;
    int a, b, c;
    long int n;

    printf("Entre com o numero de sorteios: ");
    scanf("%ld", &n);

    /* a funcao srand gera semente para
    a geracao de numeros aleatorios */
    srand(time(NULL));
    /* funcao geradora de numeros
    aleatorios inteiros: rand() */
    /* a funcao rand() gera inteiro
    entre 0 e a constante RAND_MAX */
    //printf("RAND_MAX: %d\n",RAND_MAX);
    
    a = b = c = 0;

    for ( i=0; i<n; i++ ) {
        eps = (float)rand()/RAND_MAX;
        if (eps<0.1) {
            a++;
//            printf("\t(%d)\teps=%5.3f menor que 0.1\n",i, eps);
        } else if (eps<0.5) {
            b++;
//            printf("\t(%d)\teps=%5.3f menor que 0.5\n",i, eps);
        } else {
            c++;
//            printf("\t(%d)\teps=%5.3f\n",i, eps);
        }
    }
    printf("P(a)=%f\n",(float)a/n);
    printf("P(b)=%f\n",(float)b/n);
    printf("P(c)=%f\n",(float)c/n);
    
return 0;
}
