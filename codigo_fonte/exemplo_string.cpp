#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char nome[50];
//    strcpy(nome,"Jubileu");
//    scanf("%s", nome);
    fgets(nome, 50/* numero maximo de arquivos que quero ler*/, stdin /*entrada padrao (teclado); já o stdout o padrao e a tela*/);
    nome[strlen(nome)-1]='\0'; // pegando o último caracter e trocando por zero
    printf("Meu nome eh: %s\n",nome); // Temos enter e \0 e quando fazemos isso tiramos o enter
    // lembrando que o caracter com acento conta como dois caracteres
    printf("Tamanho declarado eh: %ld\n",sizeof(nome));
    printf("Tamanho usado eh : %ld\n",strlen(nome));
return 0;
}
