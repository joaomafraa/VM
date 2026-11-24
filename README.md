# Simulador de memória virtual

O `vm` traduz endereços virtuais em endereços físicos. A memória física tem 128
frames de 256 bytes, e a TLB guarda até 16 traduções de página para frame.
O programa usa uma thread por entrada para consultar a TLB. Se a página ainda
não estiver na memória, ela é carregada de `BACKING_STORE.bin`.

## Compilar

No Linux, no macOS ou no WSL, execute `make` na pasta do projeto. É preciso ter
um compilador C com suporte a POSIX threads. O Makefile já inclui a opção
`-pthread` e gera o executável `vm`.

```bash
make
```

## Executar

Coloque `BACKING_STORE.bin` na pasta de onde você vai rodar o programa. O arquivo
de entrada deve conter um endereço decimal por linha, de 0 a 65535.

```bash
./vm address.txt lru fifo
```

O primeiro argumento é o arquivo de endereços. O segundo escolhe o algoritmo
de substituição de páginas na memória física, e o terceiro escolhe o da TLB.
Cada um aceita `fifo` ou `lru`. FIFO substitui a página ou entrada que está
ali há mais tempo. LRU substitui a que ficou mais tempo sem ser acessada.

## Conferir a saída

O programa grava `correct.txt` na pasta de execução. Cada linha de tradução
mostra o endereço virtual, o índice da entrada na TLB, o endereço físico e o
valor do byte. No final, o arquivo mostra o total de endereços traduzidos e a
contagem e a taxa de faltas de página e de acertos na TLB.

Para conferir com uma entrada pequena, use o mesmo endereço duas vezes.

```bash
printf '0\n0\n' > teste_repetido.txt
./vm teste_repetido.txt lru fifo
cat correct.txt
```

O primeiro acesso carrega a página e o segundo encontra a tradução na TLB.
O resumo deve mostrar 2 traduções, 1 falta de página e 1 acerto na TLB.
Uma nova execução sobrescreve `correct.txt`.

Para remover o executável, rode `make clean`.
