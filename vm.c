#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#define NUM_PAGINAS 256
#define NUM_FRAMES 128
#define TAM_PAGINA 256

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

int ler_enderecos(FILE* arquivo,int *tabela_paginas,algoritimo algoritimo_paginas,FILE *backing_store,
   unsigned char memoria_fisica[NUM_FRAMES][TAM_PAGINA]){
    
    char linha[256];
    int n_linha = 0;
    int proximo_frame=0;
    int pagina_no_frame[NUM_FRAMES];
    int proxima_vitima = 0;
    long long ultimo_acesso[NUM_FRAMES] = {0};
    long long contador_acessos = 0;

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
            int frame;

            if(proximo_frame<NUM_FRAMES){
                frame=proximo_frame;
                proximo_frame++;
            }
            else{
                if(algoritimo_paginas==FIFO){
                    frame = proxima_vitima;
                    proxima_vitima= (proxima_vitima+1) %NUM_FRAMES;
                }else{
                    frame = 0;
                    for(int i =1;i<NUM_FRAMES;i++){
                        if(ultimo_acesso[i]<ultimo_acesso[frame]){
                            frame=i;
                        }
                    }
                }
                int pagina_removida = pagina_no_frame[frame];
                tabela_paginas[pagina_removida] = -1;
                printf("pagina %d removida do frame %d\n",pagina_removida, frame);
            }
            if(fseek(backing_store, numero_pagina * TAM_PAGINA, SEEK_SET) != 0){
            fprintf(stderr, "Erro ao buscar a pagina %ld no BACKING_STORE.bin\n",numero_pagina);
            return 0;
            }

            if(fread(memoria_fisica[frame], 1, TAM_PAGINA, backing_store) != TAM_PAGINA) {
                fprintf(stderr, "Erro ao ler a pagina %ld do BACKING_STORE.bin\n",numero_pagina);
                return 0;
            }
            tabela_paginas[numero_pagina] = frame;
            pagina_no_frame[frame] = numero_pagina;

            printf("Endereco: %ld | Pagina: %ld | Deslocamento: %ld\n",endereco, numero_pagina, deslocamento);
            printf("Pagina %ld carregada no frame %d\n",numero_pagina, frame);
        }else{
            printf("Pagina %ld encontrada no frame %d\n",numero_pagina, tabela_paginas[numero_pagina]);
        }
        contador_acessos++;
        ultimo_acesso[tabela_paginas[numero_pagina]] = contador_acessos;
        int frame_atual = tabela_paginas[numero_pagina];
        long endereco_fisico = frame_atual * TAM_PAGINA + deslocamento;
        int valor = memoria_fisica[frame_atual][deslocamento];
        if(valor >= 128){
            valor -= 256;
        }
        printf("Endereco fisico: %ld | Valor: %d\n",endereco_fisico, valor);
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
    unsigned char memoria_fisica[NUM_FRAMES][TAM_PAGINA];
    FILE *backing_store;

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
    backing_store = fopen("BACKING_STORE.bin", "rb");

    if(backing_store == NULL){
        perror("Erro ao abrir BACKING_STORE.bin");
        fclose(arq_end);
        return 1;
    }
    inicializar_tabela(tabela_paginas);
    if(!ler_enderecos(arq_end,tabela_paginas,algoritimo_paginas,backing_store,memoria_fisica)){
        fclose(arq_end);
        fclose(backing_store);
        return 1;
    }

    fclose(backing_store);
    fclose(arq_end);
    printf("ok");
    return 0;
}
