#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAMANHOhash 17035841;
char arquivo1 [] = "201803_BolsaFamiliaFolhaPagamento.csv";
char arquivoHash [] = "201803_BolsaFamiliaHash.dat";
char arquivoHashIntersecao [] = "201803and04BolsaFamiliaHash.dat";
char arquivo2 [] = "201804_BolsaFamiliaFolhaPagamento.csv";
long ultimoIndex = TAMANHOhash;

typedef struct _TabelaHash TabelaHash;

struct _TabelaHash {
    long chave;
    long endereco;
    long proximo;
};

long hash (long chave) {
    return (((chave*23)*19)*29)%TAMANHOhash;
}

void lerPosicao (long posicao, char nomeArquivo []) {
    char linha[2048];
    FILE *f = fopen(nomeArquivo, "r");
    if (f == NULL) {
        printf("Não foi possivel abrir o aquivo para leitura na função: LerPosicao");
    }else {
        fseek(f,posicao,SEEK_SET);
        fgets(linha, 2048, f);
        printf("%s\n",linha);
    }
    fclose(f);
}

void gerarTabelaHash (char nomeArquivo []) {
    TabelaHash tabelaHash;
    char linha[2048], nis[15], *campo;
    long posicao, chave, index, proximo;
    int cont = 0, coluna = 0, fim=0;
    FILE *f = fopen(nomeArquivo, "r");
    fgets(linha, 2048, f);
    fgets(linha, 2048, f);

    //criar tabela hash vazia
    FILE *fHash = fopen(arquivoHash, "w");
    fseek(fHash, 0, SEEK_SET);
    tabelaHash.chave=0;
    tabelaHash.endereco=0;
    tabelaHash.proximo=0;
    for (cont = 0; cont < TAMANHOhash cont++) {
        fwrite(&tabelaHash, sizeof(tabelaHash),1, fHash);
    }
    fclose(fHash);
    //

    fHash = fopen(arquivoHash, "r+");
    cont = 0;
    fseek(fHash, 0, SEEK_END);
    fim = ftell(fHash);
    fseek(fHash, 0, SEEK_SET);

    while(!feof(f)) {
        coluna = 0;
        campo = strtok(linha,"\t");
        while(campo) {
            if (coluna == 7) {
                strcpy(nis, campo);
                chave = atol(nis);
                index = hash(chave);
                posicao = ftell(f);
                fseek(fHash,index*sizeof(tabelaHash),SEEK_SET);
                TabelaHash tabelaLida, tabelaInserir;
                fread(&tabelaLida, sizeof(tabelaHash),1,fHash);
                if (tabelaLida.chave == 0) {
                    fseek(fHash,index*sizeof(tabelaHash),SEEK_SET);
                    tabelaInserir.chave = chave;
                    tabelaInserir.endereco = posicao;
                    tabelaInserir.proximo = 0;
                    fwrite(&tabelaInserir, sizeof(tabelaInserir),1, fHash);
                }else {
                    proximo = tabelaLida.proximo;
                    fseek(fHash,index*sizeof(tabelaLida),SEEK_SET);
                    tabelaLida.proximo = fim/sizeof(tabelaLida);
                    fwrite(&tabelaLida, sizeof(tabelaLida),1, fHash);
                    fseek(fHash,fim,SEEK_SET);
                    tabelaInserir.chave = chave;
                    tabelaInserir.endereco = posicao;
                    tabelaInserir.proximo = proximo;
                    fwrite(&tabelaInserir, sizeof(tabelaInserir),1, fHash);
                    fim = ftell(fHash);
                    ultimoIndex++;
                }
            }
            coluna++;
            campo = strtok(NULL,"\t");
        }
        fgets(linha, 2048, f);
        cont++;
    }
    printf("Hash criada!\n");
    printf("Numero de registros: %d\n\n", cont);
    fclose(f);
    fclose(fHash);
}

void lerIndexTabelaHash (long index, char nomeArquivo []) {
    FILE *fHash = fopen(nomeArquivo, "r");
    TabelaHash tabelaHash;
    fseek(fHash,index*sizeof(tabelaHash),SEEK_SET);
    fread(&tabelaHash, sizeof(tabelaHash),1,fHash);
    printf("%ld - %ld - %ld - %ld\n", index,tabelaHash.chave, tabelaHash.endereco, tabelaHash.proximo);
    fclose(fHash);
}

void imprimirHash (char nomeArquivo []) {
    FILE *fHash = fopen(nomeArquivo, "r");
    fseek(fHash, 0,SEEK_SET);
    TabelaHash tabelaHash;
    for (int i = 0; i < ultimoIndex; i++) {
        fread(&tabelaHash, sizeof(tabelaHash),1,fHash);
        printf("\n%d - %ld - %ld - %ld\n", i,tabelaHash.chave, tabelaHash.endereco, tabelaHash.proximo);
    }
}

void intersecaoTabelasHash() {
    TabelaHash tabelaHash;
    char linha[2048], nis[15], *campo;
    long posicao=0, chave=0, index=0, proximo=0;
    int cont = 0, coluna = 0, fim=0;
    FILE *f = fopen(arquivo2, "r");
    fgets(linha, 2048, f);
    fgets(linha, 2048, f);
    FILE *fHash = fopen(arquivoHash, "r");

    TabelaHash tabelaHashIntersecao;
    //criar tabela hash vazia
    FILE *fHashIntersecao = fopen(arquivoHashIntersecao, "w");
    fseek(fHashIntersecao, 0, SEEK_SET);
    tabelaHashIntersecao.chave=0;
    tabelaHashIntersecao.endereco=0;
    tabelaHashIntersecao.proximo=0;
    for (cont = 0; cont < TAMANHOhash cont++) {
        fwrite(&tabelaHashIntersecao, sizeof(tabelaHashIntersecao),1, fHashIntersecao);
    }
    fclose(fHashIntersecao);
    cont = 0;
    fHashIntersecao = fopen(arquivoHashIntersecao, "r+");
    while(!feof(f)) {
        coluna = 0;
        campo = strtok(linha,"\t");
        while(campo) {
            if (coluna == 7) {
                strcpy(nis, campo);
                chave = atol(nis);
                index = hash(chave);
                posicao = ftell(f);
                fseek(fHash, index* sizeof(tabelaHash), SEEK_SET);
                fread(&tabelaHash, sizeof(tabelaHash),1,fHash);
                if (tabelaHash.chave == chave) {
                    fseek(fHashIntersecao,index*sizeof(tabelaHashIntersecao),SEEK_SET);
                    TabelaHash tabelaLida, tabelaInserir;
                    fread(&tabelaLida, sizeof(tabelaHashIntersecao),1,fHashIntersecao);
                    if (tabelaLida.chave == 0) {
                        fseek(fHashIntersecao,index*sizeof(tabelaHashIntersecao),SEEK_SET);
                        tabelaInserir.chave = chave;
                        tabelaInserir.endereco = posicao;
                        tabelaInserir.proximo = 0;
                        fwrite(&tabelaInserir, sizeof(tabelaInserir),1, fHashIntersecao);
                    }else {
                        proximo = tabelaLida.proximo;
                        fim = fseek(fHashIntersecao,0,SEEK_END);
                        fseek(fHashIntersecao,index*sizeof(tabelaLida),SEEK_SET);
                        tabelaLida.proximo = fim/sizeof(tabelaLida);
                        fwrite(&tabelaLida, sizeof(tabelaLida),1, fHashIntersecao);
                        fseek(fHashIntersecao,0,SEEK_END);
                        tabelaInserir.chave = chave;
                        tabelaInserir.endereco = posicao;
                        tabelaInserir.proximo = proximo;
                        fwrite(&tabelaInserir, sizeof(tabelaInserir),1, fHashIntersecao);
                        fim = ftell(fHashIntersecao);
                        //ultimoIndex++;
                    }
                    break;
                }else {
                    index = tabelaHash.proximo;
                    while (1) {
                        if (index == 0) {
                            break;
                        }else {
                            fseek(fHash, index* sizeof(tabelaHash), SEEK_SET);
                            fread(&tabelaHash, sizeof(tabelaHash),1,fHash);
                            if (tabelaHash.chave == chave) {
                                fseek(fHashIntersecao,index*sizeof(tabelaHashIntersecao),SEEK_SET);
                                TabelaHash tabelaLida, tabelaInserir;
                                fread(&tabelaLida, sizeof(tabelaHashIntersecao),1,fHashIntersecao);
                                if (tabelaLida.chave == 0) {
                                    fseek(fHashIntersecao,index*sizeof(tabelaHashIntersecao),SEEK_SET);
                                    tabelaInserir.chave = chave;
                                    tabelaInserir.endereco = posicao;
                                    tabelaInserir.proximo = 0;
                                    fwrite(&tabelaInserir, sizeof(tabelaInserir),1, fHashIntersecao);
                                }else {
                                    proximo = tabelaLida.proximo;
                                    fim = fseek(fHashIntersecao,0,SEEK_END);
                                    fseek(fHashIntersecao,index*sizeof(tabelaLida),SEEK_SET);
                                    tabelaLida.proximo = fim/sizeof(tabelaLida);
                                    fwrite(&tabelaLida, sizeof(tabelaLida),1, fHashIntersecao);
                                    fseek(fHashIntersecao,0,SEEK_END);
                                    tabelaInserir.chave = chave;
                                    tabelaInserir.endereco = posicao;
                                    tabelaInserir.proximo = proximo;
                                    fwrite(&tabelaInserir, sizeof(tabelaInserir),1, fHashIntersecao);
                                    fim = ftell(fHashIntersecao);
                                    //ultimoIndex++;
                                }
                                break;
                            }else {
                                index = tabelaHash.proximo;
                            }
                        }
                    }
                }
            }
            coluna++;
            campo = strtok(NULL,"\t");
        }
        cont++;
        fgets(linha, 2048, f);
    }
    printf("Hash da Intersecao criada!\n");
    printf("Numero de registros: %d\n\n", cont);
    fclose(f);
    fclose(fHash);
}

int main(int argc, char** argv) {
    gerarTabelaHash(arquivo1);
    //imprimirHash(arquivoHash);
    //lerIndexTabelaHash(10, arquivoHash);
    printf("\nIntersecao:\n");
    intersecaoTabelasHash();
    //imprimirHash(arquivoHashIntersecao);
    //lerIndexTabelaHash(10, arquivoHashIntersecao);
    return 0;
}