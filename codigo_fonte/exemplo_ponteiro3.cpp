#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main(void) {
    char nome[5], i;
    
    strcpy(nome,"ola"); // nome = "ola";
    // imprimir na tela o endereço armazenado em nome
    printf("Endereço armazenado em nome: %p\n", nome);
    printf("\n");
    
    // imprimir os endereços de cada elemento de nome
    for (i=0;i<5;i++) {
        printf("Endereço de nome[%d]: %p\n", i, &nome[i]);
    }
    printf("\n");
    
    printf("O endereço de nome[0] é %p\n",  nome+0 );
    printf("O conteúdo de nome[0] é %c\n",*(nome+0));
    printf("\n");

    printf("O endereço de nome[1] é %p\n",  nome+1 );
    printf("O conteúdo de nome[1] é %c\n",*(nome+1));
    printf("\n");

    printf("O endereço de nome[2] é %p\n",  nome+2 );
    printf("O conteúdo de nome[2] é %c\n",*(nome+2));
    printf("\n");
	}
