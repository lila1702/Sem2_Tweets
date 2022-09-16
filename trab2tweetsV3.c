// Nome: Lila Maria Salvador Frazão
// Matrícula: 510809
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void clrscr(){ // Limpar tela
    system("@cls||clear");
}

typedef struct hashtags{
    char *hashtag;
    int vezesEscrita;
    struct hashtags *prox;
}HASHTAG;

typedef struct bloco{
    int spcUsado; // (Usar tamBlocos - spcUsado para gerar spcDisponivel quando preciso.)
    struct bloco *prox;
    char *tweets[]; // Membro Flexível do Array
}BLOKO;

BLOKO *inicioBlokos = NULL;
BLOKO *fimBlokos = NULL;

HASHTAG *inicioHashtag = NULL;
HASHTAG *fimHashtag = NULL;

int tamBlocos; // Quantas linhas cada bloco deve ter
// Info de Admin
int qtdBlocos = 0; // Quantos blocos foram criados
int qtdTweets = 0; // Quantos tweets foram postados
int qtdHashtags = 0; // Quantas hashtags foram usadas

// ----- PARA RECONHECIMENTO AUTOMÁTICO DE TAG --------
void salvarTags(char hashtagR[]){
    HASHTAG *novaTag = malloc(sizeof(HASHTAG));
    novaTag->prox = NULL;

    int lenTag = strlen(hashtagR);
    char *tag = (char*)malloc(sizeof(char)*(lenTag+1));
    strcpy(tag, hashtagR);

    novaTag->hashtag = tag;

    if(inicioHashtag == NULL){
        novaTag->vezesEscrita = 1;
        inicioHashtag = novaTag;
        fimHashtag = novaTag;
    }
    else{
        int jaTavaLa = 0;

        for (HASHTAG *lendo = inicioHashtag; lendo != NULL; lendo = lendo->prox){
            // Se for verdade, aumenta a quantidade de vezesEscrita da hashtag postada primeiro
            if(strcmp(novaTag->hashtag, lendo->hashtag) == 0){
                lendo->vezesEscrita++;
                jaTavaLa = 1;
                break;
            }
        }
        if(jaTavaLa == 0){
            novaTag->vezesEscrita = 1;
            fimHashtag->prox = novaTag;
            fimHashtag = novaTag;
        }
        else if(jaTavaLa == 1){
            free(novaTag);
            free(tag);
        }
    }
    qtdHashtags++;
}

void reconhecerTags(char frase[]){
    int inicioTag = -1;
    char tagtemp[281] = "\0";

    for (int i = 0; frase[i] != '\0'; i++){
        // Indica o índice inicial de uma hashtag
        if(frase[i] == '#' && inicioTag == -1){
            inicioTag = i;
        }
        // Caso reconheça uma hashtag e ela terminar com espaço
        else if(inicioTag >= 0 && frase[i] == ' '){
            strncpy(tagtemp, frase+inicioTag, i-inicioTag);
            salvarTags(tagtemp);
            inicioTag = -1;
        }
        // Caso tenha reconhecido uma hashtag, mas chegar outra hashtag logo em seguida
        else if(inicioTag >= 0 && frase[i] == '#'){
            strncpy(tagtemp, frase+inicioTag, i-inicioTag);
            salvarTags(tagtemp);
            inicioTag = i;
        }
        // Caso tenha reconhecido uma hashtag, mas ela ir até o fim da frase.
        else if(inicioTag >= 0 && frase[i+1] == '\0'){
            strncpy(tagtemp, frase+inicioTag, strlen(frase));
            salvarTags(tagtemp);
            inicioTag = -1;
        }
    }
}

// ------------ PARA O SISTEMA DE BLOCOS --------------
void criarBloco(){
    BLOKO *novo = malloc(sizeof(BLOKO)+(sizeof(char*) * tamBlocos));
    novo->prox = NULL;
    novo->spcUsado = 0;
    
    if(qtdBlocos == 0){
        inicioBlokos = novo;
        fimBlokos = novo;
    }
    else{
        fimBlokos->prox = novo;
        fimBlokos = novo;
    }
    qtdBlocos++;
    printf("Bloco criado %d\n", qtdBlocos);
}

void addAoBloco(char frase[]){
    int tweetLen = strlen(frase);
    char *tweet = (char*)malloc(sizeof(char)*(tweetLen+1));

    strcpy(tweet, frase);
    
    reconhecerTags(frase);

    // Verifica se nenhum bloco foi criado ainda, ou se o bloco em uso não possui espaço suficiente.
    if(qtdBlocos == 0 || fimBlokos->spcUsado == tamBlocos){
        criarBloco();
        fimBlokos->tweets[fimBlokos->spcUsado] = tweet;
        fimBlokos->spcUsado++;
        qtdTweets++;
    }
    else if(fimBlokos->spcUsado < tamBlocos){
        fimBlokos->tweets[fimBlokos->spcUsado] = tweet;
        fimBlokos->spcUsado++;
        qtdTweets++;
    }
    else{
        free(tweet);
        printf("ERRO: Nenhuma operação realizada, tente novamente.\n");
    }
}

// ------------ LISTAR TWEETS ------------------------
void listarTweets(){    
    int blocoMostrado = 1;
    if(qtdTweets > 0){
        // Passa por todos os blocos
        for (BLOKO *leitura = inicioBlokos; leitura != NULL; leitura = leitura->prox){
            printf("Bloco %d:\n", blocoMostrado);
            // Passa por cada indice do bloco
            for (int i = 0; i < leitura->spcUsado; i++){
                printf("[%d] = %s\n", i, leitura->tweets[i]);
                if(leitura->spcUsado < tamBlocos-1)
                    break;
            }
            blocoMostrado++;
            printf("\n");
        }
    }
    else{
        printf("Nenhum tweet postado.\n");
    }
}

// -------------- PARA DEBUG --------------------------
void mostrarTag(){
    if(qtdHashtags != 0){
        for (HASHTAG *lendo = inicioHashtag; lendo != NULL; lendo = lendo->prox){
            printf("%s - %d vez(es)\n", lendo->hashtag, lendo->vezesEscrita);
        }
    }
    else{
        printf("Não há nenhuma hashtag para mostrar\n");
    }
}

// ----------- PARA O FILTRAR ------------------------
void compararTags(BLOKO *atualBloko, char buscando[]){
    for (int i = 0; i < atualBloko->spcUsado; i++){
        if((strstr(atualBloko->tweets[i], buscando)) != NULL){
            printf("%s\n", atualBloko->tweets[i]);
        }
        // Avisa quando tem que mudar de bloco
        if(atualBloko->spcUsado < tamBlocos-1)
            break;
    }
}

void filtrarTags(char buscando[]){
    if(qtdTweets != 0 && (strchr(buscando, '#')) != NULL){
        // Bloco
        for (BLOKO *atualBloko = inicioBlokos; atualBloko != NULL; atualBloko = atualBloko->prox){
            // Limite do bloco (muda para o próximo bloco)
            compararTags(atualBloko, buscando);
        }
    }
    else{
        if(qtdTweets == 0) printf("Não há nenhum tweet postado no momento\n");
        else{printf("Por favor, digite uma hashtag.\n");}
    }
}

// ---------- PARA O TRENDING TOPICS -----------------
int qtdHashtagsDiferente(){
    int qtdTagsDif = 0;
    for(HASHTAG *lendo = inicioHashtag; lendo != NULL; lendo = lendo->prox){
        qtdTagsDif++;
    }
    return qtdTagsDif;
}

int estaEmOrdem(HASHTAG* vetor[], int qtd){
    int falhou = 0;
    for (int i = 0; i < qtd; i++){
        if(i+1 == qtd){
            break;
        }
        else if(vetor[i]->vezesEscrita >= vetor[i+1]->vezesEscrita){
            continue;
        }
        else if(vetor[i]->vezesEscrita < vetor[i+1]->vezesEscrita){
            falhou = 1;
            break;
        }
    }
    if(falhou == 1){
        return -1;
    }
    else{
        return 1;
    }
}

void rearranjoTrending(HASHTAG* vetor[], int qtd){
    HASHTAG* aux;
    // Se o array já estiver organizado, pula esse laço
    if ((estaEmOrdem(vetor, qtd)) == -1){
        for (int i = 0; i < qtd; i++){
            for (int j = 0; j < qtd; j++){
                // Caso contrário... se o twt em j foi menos escrito
                // do que o do índice i, eles trocam os lugares.
                if(vetor[j]->vezesEscrita < vetor[i]->vezesEscrita){
                    aux = vetor[i];
                    vetor[i] = vetor[j];
                    vetor[j] = aux;
                }
            }
        }
    }
}

void trending_topics(){
    // qtdTagsDif = Quantidade de Tags Diferentes
    int qtdTagsDif = qtdHashtagsDiferente();
    HASHTAG* trending[qtdTagsDif]; // Cria um array de Hashtags* do tamanho da quantidade de hashtags
    if(qtdHashtags != 0){
        int i = 0;
        // Copia tudo da lista encadeada das tags no vetor
        for(HASHTAG *lendo = inicioHashtag; lendo != NULL; lendo = lendo->prox){
            if(i == qtdTagsDif){
                break;
            }
            trending[i] = lendo;
            i++;
        }
        // Arruma em ordem decrescente
        rearranjoTrending(trending, qtdTagsDif);
        // Printa
        for (int i = 0; i < qtdTagsDif; i++){
            printf("%s - %d vez(es)\n", trending[i]->hashtag, trending[i]->vezesEscrita);
        }
    }
    else{
        printf("Não há nenhuma hashtag para mostrar\n");
    }
    //free(trending);
}

// --------- PARA SALVAR E CARREGAR ARQUIVOS -----------
void salvarNoArquivo(char nomeArq[]){
    // Para gravar a data e a hora no Arquivo
    struct tm *data;
    time_t segs;
    time(&segs);
    data = localtime(&segs);
    
    FILE *arquivo = fopen(nomeArq, "a+");
    if(arquivo != NULL){
        fprintf(arquivo, "Salvo em: %d/%d/%d as %d:%d\n", data->tm_mday, data->tm_mon+1, data->tm_year+1900, data->tm_hour, data->tm_min);
        for (BLOKO *atual = inicioBlokos; atual != NULL; atual = atual->prox){
            for (int i = 0; i < atual->spcUsado; i++){
                fprintf(arquivo, "%s\n", atual->tweets[i]);
                if(atual->spcUsado < tamBlocos-1)
                    break;
            }
        }
    }
    else if(arquivo == NULL){
        printf("Erro na criação do arquivo.\n");
    }
    fclose(arquivo);
}

void carregarAppending(char *nomeArq){
    char tweetLoad[280];
    FILE *arquivo = fopen(nomeArq, "r");
    
    if(arquivo == NULL){
        printf("Erro ao abrir o arquivo.\n");
    }
    else if(arquivo != NULL){
        while (fgets(tweetLoad, 280, arquivo)){
            // Acha a substring que diz começa a dizer a data
            if((strstr(tweetLoad, "Salvo em: ")) == NULL){
                // strcspn mostra o len de tweetload até chegar no \n
                tweetLoad[strcspn(tweetLoad, "\n")] = 0;
                addAoBloco(tweetLoad);
            }
        }
    }
    fclose(arquivo);
}

void carregarClean(char *nomeArq){
    char tweetLoad[280];
    
    FILE *arquivo = fopen(nomeArq, "r");
    if(arquivo == NULL){
        printf("Erro ao abrir o arquivo.\n");
    }
    else if(arquivo != NULL){
        while (inicioBlokos != NULL){
            if(qtdBlocos == 1){
                // Libera os tweets alocados
                for (int i = inicioBlokos->spcUsado; i >= 0; i--){
                    free(inicioBlokos->tweets[i]);
                    inicioBlokos->spcUsado--;
                    qtdTweets--;
                }
                // Libera o bloco em si e suas variáveis
                free(inicioBlokos);
                inicioBlokos = NULL;
                fimBlokos = NULL;
                qtdBlocos = 0;
            }
            else if(qtdBlocos > 1){
                BLOKO *blokoAtual = inicioBlokos;
                // Chega no último bloco antes bloco final da lista
                for (blokoAtual; blokoAtual->prox != NULL; blokoAtual = blokoAtual->prox){
                    // Libera os tweets
                    for (int i = blokoAtual->spcUsado; i >= 0; i--){
                        free(blokoAtual->prox->tweets[i]);
                        blokoAtual->prox->spcUsado--;
                        qtdTweets--;
                    }
                    free(blokoAtual->prox);
                    blokoAtual->prox = NULL;
                    qtdBlocos--;
                }
            }
        }
        while (fgets(tweetLoad, 280, arquivo)){
            // Acha a substring que diz começa a dizer a data
            if((strstr(tweetLoad, "Salvo em: ")) == NULL){
                // strcspn mostra o len de tweetload até chegar no \n
                tweetLoad[strcspn(tweetLoad, "\n")] = 0;
                addAoBloco(tweetLoad);
            }
        }
    }
    fclose(arquivo);
}

int main(){
    // Menu
    int opcoes;
    char sair = 1;
    
    // Para criação de Tweets
    char tweet[281];

    // Para buscar uma Hashtag
    char tag[281];

    // Para criação/carregamento do Arquivo
    char input[141];
    char *nomeArq;

    // Para carregar Arquivo
    int load = 0;
    char *loadArq;

    clrscr();

    do{
        printf("Digite o tamanho n dos blocos para armazenar os tweets: ");
        fflush(stdin);
        scanf("%d", &tamBlocos);
    } while (tamBlocos <= 0);

    // Repetição de Menu
    do
    {
        printf("Digite o número correspondente a uma das opções abaixo:\n");
        printf("1 - Fazer um tweet.\n");
        printf("2 - Listar tweets.\n");
        printf("3 - Filtrar por hashtag.\n");
        printf("4 - Listar trending topics.\n");
        printf("5 - Informações de adm.\n");
        printf("6 - Salvar.\n");
        printf("7 - Carregar.\n");
        printf("0 - Sair.\n");
        
        printf("Num: ");
        fflush(stdin);
        scanf("%d", &opcoes);

        switch (opcoes)
        {
        // Criar Tweet
        case 1:
            printf("Tweetar (Até 280 Caracteres): ");
            fflush(stdin);
            scanf("%[^\n]s", tweet);

            addAoBloco(tweet);

            printf("%s\n", tweet);
            break;
        // Listar Tweets
        case 2:
            listarTweets();
            break;
        // Filtrar por Hashtags
        case 3:
            printf("Buscar hashtag (incluir o símbolo #, por favor): ");
            fflush(stdin);
            scanf("%[^\n]s", tag);
            
            filtrarTags(tag);
            break;
        // Listar Trending Topics
        case 4:
            trending_topics();
            break;
        // Listar Configurações de Admin
        case 5:
            printf("Bem Vindo ADM!\n");
            printf("Informações sobre o sistema:\n");
            printf("%d blocos\n", qtdBlocos);
            printf("%d tweet(s)\n", qtdTweets);
            printf("%d hashtag(s)\n", qtdHashtags);
            printf("O último bloco possui %d espaço(s) não usado(s)\n", (tamBlocos - fimBlokos->spcUsado));
            break;
        // Salvar no Arquivo
        case 6:
            printf("Nomeie o arquivo sendo criado (Sem espaços): ");
            fflush(stdin);
            scanf("%s", input);

            nomeArq = malloc(sizeof(strlen(input))+sizeof(char)*5);
            strcpy(nomeArq, input);
            sprintf(nomeArq, "%s.txt", input);
            printf("%s\n", nomeArq);

            salvarNoArquivo(nomeArq);
            
            break;
        // Carregar um Arquivo
        case 7:
            printf("Pressione [1] se quiser Carregar o Arquivo preservando o conteúdo atual\n");
            printf("Pressione [2] se quiser Carregar o Arquivo apagando todo o conteúdo atual\n");
            printf("Pressione qualquer outra tecla para cancelar\n");
            scanf("%d", &load);
            if(load == 1 || load == 2){
                printf("Digite o nome do arquivo a ser carregado (sem o .txt): ");
                fflush(stdin);
                scanf("%s", input);
                loadArq = malloc(sizeof(strlen(input))+sizeof(char)*5);
                strcpy(loadArq, input);
                sprintf(loadArq, "%s.txt", input);
                if(load == 1){
                    carregarAppending(loadArq);
                }
                else if(load == 2){
                    carregarClean(loadArq);
                }
            }
            break;
        /*Para debug
        case 8:
            mostrarTag();
            break;
        */
        // Sair
        case 0:
            sair = 0;
            break;

        default:
            printf("Opção inválida, tente novamente.\n");
            break;
        }
    
    } while (sair != 0);
    
    return 0;
}