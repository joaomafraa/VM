#include <stdio.h>
#include <string.h>

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

    printf("ok %s %s",argv[2],argv[3]);

    fclose(arq_end);
    return 0;
}