#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

typedef enum{
    FIFO,
    LRU
}algoritimo;

int ler_algoritimo(char* texto, algoritimo *algoritimo){
    if(strcmp(texto,"fifo")==0){
        *algoritimo = FIFO;
        return 1;
    }
    if(strcmp(texto,"lru")==0){
        *algoritimo = LRU;
        return 1;
    }
    return 0;
}

int ler_enderecos(FILE* arquivo){
    char linha[256];
    int n_linha = 0;

    while (fgets(linha,sizeof(linha), arquivo)!= NULL){
        char *fim;
        long endereco;

        n_linha++;
        errno=0;
        endereco = strtol(linha, &fim,10);

        if(fim == linha || errno == ERANGE){
            fprintf(stderr,"erro endereco invalido na linha %d\n", n_linha);
            return 0;
        }
        while (isspace((unsigned char)*fim)){
            fim++;//permite espacos ou quebra de linah
        }
        if(*fim!='\0' || endereco <0 || endereco>65535){
            fprintf(stderr,"Erro no endereço da linha %d\n",n_linha);
            return 0;
        }
        long numero_pagina = endereco / 256;
        long deslocamento = endereco % 256;

        printf("Endereco: %ld | Pagina: %ld | Deslocamento: %ld\n",endereco, numero_pagina, deslocamento);
    }
    if(ferror(arquivo)){
        fprintf(stderr,"erro durante a leitura do arq\n");
        return 0;
    }
    
    return 1;
}
int main(int argc, char** argv){
    algoritimo algoritimo_paginas;  
    algoritimo algoritimo_tlb;
    FILE * arq_end;

    if(argc !=4){
        fprintf(stderr,"Parametros insuficientes\n");
        return 1;
    }
    
    if(!ler_algoritimo(argv[2],&algoritimo_paginas)){
        fprintf(stderr,"use fifo ou lru nas paginas \n");
        return 1;
    }
    if(!ler_algoritimo(argv[3],&algoritimo_tlb)){
        fprintf(stderr,"use fifo ou lru no tlb\n");
        return 1;
    }

    arq_end = fopen(argv[1],"r");

    if(arq_end==NULL){
        perror("erro ao abrir arquivo de endereços\n");
        return 1;
    }

    if(!ler_enderecos(arq_end)){
        fclose(arq_end);
        return 1;
    }

    fclose(arq_end);
    printf("ok");
    return 0;
}