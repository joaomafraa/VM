#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#define NUM_PAGINAS 256
#define NUM_FRAMES 128

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

int ler_enderecos(FILE* arquivo,int *tabela_paginas){
    char linha[256];
    int n_linha = 0;
    int proximo_frame=0;
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
        if(tabela_paginas[numero_pagina] == -1){
            if(proximo_frame >= NUM_FRAMES){
                fprintf(stderr,"Memoria cheia ");
                return 0;
            }
            tabela_paginas[numero_pagina] = proximo_frame;
            printf("pagina %ld carregada no frame %d\n", numero_pagina,proximo_frame);
            proximo_frame++;
        }else{
            printf("pagina %ld encontrada no frame %d \n", numero_pagina, tabela_paginas[numero_pagina]);
        }

        printf("Endereco: %ld | Pagina: %ld | Deslocamento: %ld\n",endereco, numero_pagina, deslocamento);
    }
    if(ferror(arquivo)){
        fprintf(stderr,"erro durante a leitura do arq\n");
        return 0;
    }
    
    return 1;
}
void inicializar_tabela(int *tabela_paginas){
    for(int i=0;i<NUM_PAGINAS;i++){
        tabela_paginas[i]=-1;
    }
}
int main(int argc, char** argv){
    algoritimo algoritimo_paginas;  
    algoritimo algoritimo_tlb;
    FILE * arq_end;
    int tabela_paginas[NUM_PAGINAS];
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
    inicializar_tabela(tabela_paginas);
    if(!ler_enderecos(arq_end,tabela_paginas)){
        fclose(arq_end);
        return 1;
    }


    fclose(arq_end);
    printf("ok");
    return 0;
}