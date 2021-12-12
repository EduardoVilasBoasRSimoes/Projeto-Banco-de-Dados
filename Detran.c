/*
Integrantes da equipe:

Eduardo Vilas Boas Ribas Simões - 509925
Erika Oliveira Quinto - 511769
Pedro Enzo da Silva Batista - 514784
Renan Barroso Sá - 515168
*/

//PROGRAMA ELABORADO EM WINDOWS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>
#include <stdbool.h>
#include <time.h>
#define ARQUIVO "CadastroDetran.txt"

struct detran {
    char nome[100];
    int idade;
    char sexo[13];
    char nascimento[11];
    char estado[20];
    char municipio[30];
    char endereco[100];
    char cpf[15];
    char rg[20];
    char cnh[14];
    bool suspensao;
    char categoria[5];
    char renach[14];
    char emissao[11];
    char validade[11];
    bool desvalidou;
    char telefone[15];
    int infracoes;
};

FILE *arq;
char B = ' ';

void Escolha(){ //PROCESSO INICIAL DE ESCOLHA DAS AÇÕES GERAIS//
    int o;
    char B = ' ', A;
    printf("%49cDigite 1 para Inserir\n%49cDigite 2 para Buscar\n%49cDigite 3 para Remover\n%44cDigite 4 para Inserir infracao\n%41cDigite 5 para Revelar Banco de Dados\n%40cDigite 6 para Remover Supensao da CNH\n%45cDigite 7 para Renovar a CNH\n\n", B, B, B, B, B, B, B);
    do{
        o = 1;
        printf("%58c", B);
        fflush(stdin);
        scanf("%c", &A);
        if(A == '1'){
            Inserir();
        } else if(A == '2'){
            Buscar();
        } else if(A == '3'){
            Remover();
        } else if(A == '4'){
            AdicionarInfracao();
        } else if(A == '5'){
            ImprimirDados();
        } else if(A == '6'){
            RemoverSuspensao();
        } else if(A == '7'){
            RenovarCNH();
        } else {
            printf("\n%51cFuncao invalida\n%42cInsira a opcao desejada novamente\n\n", B, B);
            o = 0;
        }
    } while(o == 0);
}

void Maiuscula(char a[100]){ //TRANSFORMANDO TODAS AS LETRAS PARA MAIUCULAS DEVIDO AO FATOR SENSITIVO DA C//
    int i;
    for(i = 0; i < strlen(a); i++){
        a[i] = toupper(a[i]);
    }
}

void FormatarCPF(char a[15]){ //RECEBE O CPF COM APENAS NÚMEROS E SALVA EM FORMATAÇÃO COMUM COM PONTOS E TRAÇOS //
    int i, cont = 0;
    char b[15];
    for(i = 0; i < 11; i ++){
        if((i+1) % 3 == 0 && i != 8){
            b[cont] = a[i];
            b[cont+1] = '.';
            cont += 2;
        } else if (i == 8){
            b[cont] = a[i];
            b[cont+1] = '-';
            cont += 2;
        } else {
            b[cont] = a[i];
            cont++;
        }
    }
    strcpy(a, b);
}

void FormatarData(char a[11]){ //RECEBE A DATA COM APENAS NÚMEROS E ADAPTA PARA A FORMATAÇÃO USUAL DE 00/00/0000//
   int i, cont = 0;
    char b[11];
    for(i = 0; i < 8; i ++){
        if((i+1) % 2 == 0 && i < 4){
            b[cont] = a[i];
            b[cont+1] = '/';
            cont += 2;
        } else {
            b[cont] = a[i];
            cont++;
        }
    }
    strcpy(a, b);
}

int VerificarNumerais(char N[15]){ //VERIFICA A EXISTEÊNCIA DE NÚMEROS E LETRAS JUNTOS EM LOCAIS NÃO PERMITIDOS//
    int i;
    for(i = 0; i < strlen(N) - 1; i++){
        if(N[i] < '0' || N[i] > '9'){
            return 0;
        }
    }
    return 1;
}

int TransformarEmNumero(char I[10]){ //TRANSFORMA DE FORMATO CHAR PRA FORMATO INT OS NÚMEROS ENVIADOS AO PROGRAMA//
    int i;
    if(strlen(I) == 2){
        i = I[0] - 48;
        return i;
    } else if(strlen(I) == 3){
        i = I[0] - 48;
        i *= 10;
        i += I[1] - 48;
        return i;
    }
}

int DataDia(char data[11]){ //CAPTA O DIA ATUAL PARA COMPARAÇÃO COM A VALIDADE E EMISSÃO + IDADE//
    int Dia;
    if(data[0] == '0'){
        Dia = data[1] - 48;
    } else {
        Dia = (data[0] - 48)*10 + (data[1] - 48);
    }
    return Dia;
}

int DataMes(char data[11]){ //CAPTA O MES ATUAL PARA COMPARAÇÃO COM A VALIDADE E EMISSÃO + IDADE//
    int Mes;
    if(data[2] == '0'){
        Mes = data[3] - 48;
    } else {
        Mes = (data[2] - 48)*10 + (data[3] - 48);
    }
    return Mes;
}

int DataAno(char data[11]){ //CAPTA O ANO ATUAL PARA COMPARAÇÃO COM A VALIDADE E EMISSÃO + IDADE//
    int Ano;
    Ano = (data[4] - 48)*1000 + (data[5] - 48)*100 +(data[6] - 48)*10 + (data[7] - 48);
    return Ano;
}

int CompararEmissaoValidade(char E[11], char V[11]){ //VERIFICA SE VALIDADE > EMISSÃO//
    if(DataAno(E) < DataAno(V)){
        return 1;
    } else if(DataAno(E) > DataAno(V)){
        return 0;
    } else {
        if(DataMes(E) < DataMes(V)){
            return 1;
        } else if(DataMes(E) > DataMes(V)){
            return 0;
        } else {
            if(DataDia(E) < DataDia(V)){
                return 1;
            } else if(DataDia(E) >= DataDia(V)){
                return 0;
            }
        }
    }
}

int CompararNascimentoIdade(char N[11], int I){ //VERIFICA SE A DATA DE NASCIMENTO CORRESPONDE À IDADE//
    time_t mytime;
    mytime = time(NULL);
    struct tm tm = *localtime(&mytime);

    if(DataMes(N) < tm.tm_mon + 1){
        if(I == tm.tm_year + 1900 - DataAno(N)){
            return 1;
        } else {
            return 0;
        }
    } else if(DataMes(N) > tm.tm_mon + 1){
        if(I == tm.tm_year + 1900 - DataAno(N) - 1){
            return 1;
        } else {
            return 0;
        }
    } else if(DataMes(N) == tm.tm_mon + 1){
        if(DataDia(N) < tm.tm_mday){
            if(I == tm.tm_year + 1900 - DataAno(N)){
                return 1;
            } else {
                return 0;
            }
        } else if(DataDia(N) > tm.tm_mday){
            if(I == tm.tm_year + 1900 - DataAno(N) - 1){
                return 1;
            } else {
                return 0;
            }
        } else if(DataDia(N) == tm.tm_mday){
            if(I == tm.tm_year + 1900 - DataAno(N)){
                return 1;
            } else {
                return 0;
            }
        }
    }
}

int CompararNascimentoEmissao(char N[11], char E[11]){ //VERIFICA A EXITEÊNCIA DOS 18 ANOS EM RELAÇÃO À EMISSÃO//

    if(DataMes(N) < DataMes(E)){
        if(DataAno(E) - DataAno(N) >= 18){
            return 1;
        } else {
            return 0;
        }
    } else if(DataMes(N) > DataMes(E)){
        if(DataAno(E) - DataAno(N) - 1 >= 18){
            return 1;
        } else {
            return 0;
        }
    } else if(DataMes(N) == DataMes(E)){
        if(DataDia(N) < DataDia(E)){
            if(DataAno(E) - DataAno(N) >= 18){
                return 1;
            } else {
                return 0;
            }
        } else if(DataDia(N) > DataDia(E)){
            if(DataAno(E) - DataAno(N) - 1 >= 18){
                return 1;
            } else {
                return 0;
            }
        } else if(DataDia(N) == DataDia(E)){
            if(DataAno(E) - DataAno(N) >= 18){
                return 1;
            } else {
                return 0;
            }
        }
    }
}

int VerificarEmissao(char E[11]){ //VERIFICA SE A EMISSÃO É MENOR OU IGUAL AO DIA DE HOJE//
    time_t mytime;
    mytime = time(NULL);
    struct tm tm = *localtime(&mytime);

    if(DataAno(E) < tm.tm_year + 1900){
        return 1;
    } else if(DataAno(E) > tm.tm_year + 1900){
        return 0;
    } else if(DataAno(E) == tm.tm_year + 1900){
        if(DataMes(E) < tm.tm_mon + 1){
            return 1;
        } else if(DataMes(E) > tm.tm_mon + 1){
            return 0;
        } else if(DataMes(E) == tm.tm_mon + 1){
             if(DataDia(E) < tm.tm_mday){
                return 1;
            } else if(DataDia(E) > tm.tm_mday){
                return 0;
            } else if(DataDia(E) == tm.tm_mday){
                return 1;
            }
        }
    }
}

int VerificarValidade(char V[11]){ //VERIFICA SE A VALIDADE ESTÁ MAIOR QUE A DATA DE EMISSÃO//
    time_t mytime;
    mytime = time(NULL);
    struct tm tm = *localtime(&mytime);

    if(DataAno(V) < tm.tm_year + 1900){
        return 0;
    } else if(DataAno(V) > tm.tm_year + 1900){
        return 1;
    } else if(DataAno(V) == tm.tm_year + 1900){
        if(DataMes(V) < tm.tm_mon + 1){
            return 0;
        } else if(DataMes(V) > tm.tm_mon + 1){
            return 1;
        } else if(DataMes(V) == tm.tm_mon + 1){
             if(DataDia(V) < tm.tm_mday){
                return 0;
            } else if(DataDia(V) > tm.tm_mday){
                return 1;
            } else if(DataDia(V) == tm.tm_mday){
                return 0;
            }
        }
    }
}

void Inserir(){ //INSERE UM NOVO CADASTRO NO BANCO DE DADOS//
    struct detran aux;
    int find, A = 0, repetir = 0, repetir2 = 0;
    char o, confirmacao, finish[100], emissao[9], nascimento[9], i;
    printf("\n%24cPara inserir esse novo cadastro coloque os dados solicitados abaixo:\n\n", B);

    do{
        printf("\n%24cNome: ", B);
        fflush(stdin);
        fgets(aux.nome, sizeof(aux.nome), stdin);

        do{
            printf("\n%51cConfirmar Nome\n%51c[S/N]: ", B, B);
            fflush(stdin);
            scanf("%c", &confirmacao);
            confirmacao = toupper(confirmacao);
            printf("\n");
            if(confirmacao == 'N'){
                repetir = 1;
                repetir2 = 0;
            } else if(confirmacao == 'S'){
                repetir = 0;
                repetir2 = 0;
            } else {
                printf("%51cDado invalido!\n", B);
                repetir2 = 1;
            }
        } while(repetir2 == 1);

    } while(repetir == 1);

    Maiuscula(aux.nome);

    do{
        printf("\n%24cIdade (Apenas Numeros): ", B);
        fflush(stdin);
        scanf("%d", &aux.idade);
        i = aux.idade;
        if(aux.idade >= 18){
            do{
                printf("\n%51cConfirmar Idade\n%51c[S/N]: ", B, B);
                fflush(stdin);
                scanf("%c", &confirmacao);
                confirmacao = toupper(confirmacao);
                printf("\n");
                if(confirmacao == 'N'){
                    repetir = 1;
                    repetir2 = 0;
                } else if(confirmacao == 'S'){
                    repetir = 0;
                    repetir2 = 0;
                } else {
                    printf("%51cDado invalido!\n", B);
                    repetir2 = 1;
                }
            } while(repetir2 == 1);
        } else {
            printf("%51cIdade invalida!\n\n%51cInserir nova idade", B, B);
            repetir = 1;
        }

    } while(repetir == 1);

    do{
        printf("\n%24cSexo: ", B);
        fflush(stdin);
        fgets(aux.sexo, sizeof(aux.sexo), stdin);

        do{
            printf("\n%51cConfirmar Sexo\n%51c[S/N]: ", B, B);
            fflush(stdin);
            scanf("%c", &confirmacao);
            confirmacao = toupper(confirmacao);
            printf("\n");
            if(confirmacao == 'N'){
                repetir = 1;
                repetir2 = 0;
            } else if(confirmacao == 'S'){
                repetir = 0;
                repetir2 = 0;
            } else {
                printf("%51cDado invalido!\n", B);
                repetir2 = 1;
            }
        } while(repetir2 == 1);

    } while(repetir == 1);

    Maiuscula(aux.sexo);

    do{
        printf("\n%24cNascimento (Sem Barras): ", B);
        fflush(stdin);
        fgets(aux.nascimento, sizeof(aux.nascimento), stdin);

        if(strlen(aux.nascimento) == 9 && VerificarNumerais(aux.nascimento) == 1 && CompararNascimentoIdade(aux.nascimento, aux.idade)){
            do{
                printf("\n%51cConfirmar Nascimento\n%51c[S/N]: ", B, B);
                fflush(stdin);
                scanf("%c", &confirmacao);
                confirmacao = toupper(confirmacao);
                printf("\n");
                if(confirmacao == 'N'){
                    repetir = 1;
                    repetir2 = 0;
                } else if(confirmacao == 'S'){
                    repetir = 0;
                    repetir2 = 0;
                } else {
                    printf("%51cDado invalido!\n", B);
                    repetir2 = 1;
                }
            } while(repetir2 == 1);
        } else {
            printf("%51cData invalida!\n\n%51cInserir nova data", B, B);
            repetir = 1;
        }

    } while(repetir == 1);
    strcpy(nascimento, aux.nascimento);
    FormatarData(aux.nascimento);

    do{
        printf("\n%24cEstado: ", B);
        fflush(stdin);
        fgets(aux.estado, sizeof(aux.estado), stdin);

        do{
            printf("\n%51cConfirmar Estado\n%51c[S/N]: ", B, B);
            fflush(stdin);
            scanf("%c", &confirmacao);
            confirmacao = toupper(confirmacao);
            printf("\n");
            if(confirmacao == 'N'){
                repetir = 1;
                repetir2 = 0;
            } else if(confirmacao == 'S'){
                repetir = 0;
                repetir2 = 0;
            } else {
                printf("%51cDado invalido!\n", B);
                repetir2 = 1;
            }
        } while(repetir2 == 1);

    } while(repetir == 1);
    Maiuscula(aux.estado);

    do{
        printf("\n%24cMunicipio: ", B);
        fflush(stdin);
        fgets(aux.municipio, sizeof(aux.municipio), stdin);

        do{
            printf("\n%51cConfirmar Municipio\n%51c[S/N]: ", B, B);
            fflush(stdin);
            scanf("%c", &confirmacao);
            confirmacao = toupper(confirmacao);
            printf("\n");
            if(confirmacao == 'N'){
                repetir = 1;
                repetir2 = 0;
            } else if(confirmacao == 'S'){
                repetir = 0;
                repetir2 = 0;
            } else {
                printf("%51cDado invalido!\n", B);
                repetir2 = 1;
            }
        } while(repetir2 == 1);

    } while(repetir == 1);
    Maiuscula(aux.municipio);

    do{
        printf("\n%24cEndereco: ", B);
        fflush(stdin);
        fgets(aux.endereco, sizeof(aux.endereco), stdin);

        do{
            printf("\n%51cConfirmar Endereco\n%51c[S/N]: ", B, B);
            fflush(stdin);
            scanf("%c", &confirmacao);
            confirmacao = toupper(confirmacao);
            printf("\n");
            if(confirmacao == 'N'){
                repetir = 1;
                repetir2 = 0;
            } else if(confirmacao == 'S'){
                repetir = 0;
                repetir2 = 0;
            } else {
                printf("%51cDado invalido!\n", B);
                repetir2 = 1;
            }
        } while(repetir2 == 1);

    } while(repetir == 1);
    Maiuscula(aux.endereco);

    do{
        printf("\n%24cCPF (sem pontos ou tracos): ", B);
        fflush(stdin);
        fgets(aux.cpf, sizeof(aux.cpf), stdin);

        if(strlen(aux.cpf) == 12 && VerificarNumerais(aux.cpf) == 1){
            do{
                printf("\n%51cConfirmar CPF\n%51c[S/N]: ", B, B);
                fflush(stdin);
                scanf("%c", &confirmacao);
                confirmacao = toupper(confirmacao);
                printf("\n");
                if(confirmacao == 'N'){
                    repetir = 1;
                    repetir2 = 0;
                } else if(confirmacao == 'S'){
                    repetir = 0;
                    repetir2 = 0;
                } else {
                    printf("%51cDado invalido!\n", B);
                    repetir2 = 1;
                }
            } while(repetir2 == 1);
        } else {
            printf("%51cCPF invalido!\n\n%51cInserir novo CPF", B, B);
            repetir = 1;
        }

    } while(repetir == 1);
    FormatarCPF(aux.cpf);

    arq = fopen(ARQUIVO,"r+t");
        if(arq == NULL){
            arq = fopen(ARQUIVO,"a+t");
            if(arq == NULL){
                fprintf(stderr,"Nao se pode criar %s\n", ARQUIVO);
                exit(1);
            }
        }

    rewind(arq);
    find = 0;
    char cpf[15];
    while(!feof(arq)){
        fflush(stdin);
        fgets(cpf, 15, arq);
        if(strncmp(cpf, aux.cpf, 15) == 0){
            printf("%48cPessoa ja cadastrada!\n", B);
            find = 1;
            fclose(arq);
            break;
        }
    }

    if(find == 0){
        do{
            printf("\n%24cRG: ", B);
            fflush(stdin);
            fgets(aux.rg, sizeof(aux.rg), stdin);

            do{
                printf("\n%51cConfirmar RG\n%51c[S/N]: ", B, B);
                fflush(stdin);
                scanf("%c", &confirmacao);
                confirmacao = toupper(confirmacao);
                printf("\n");
                if(confirmacao == 'N'){
                    repetir = 1;
                    repetir2 = 0;
                } else if(confirmacao == 'S'){
                    repetir = 0;
                    repetir2 = 0;
                } else {
                    printf("%51cDado invalido!\n", B);
                    repetir2 = 1;
                }
            } while(repetir2 == 1);

        } while(repetir == 1);

        do{
            printf("\n%24cCNH: ", B);
            fflush(stdin);
            fgets(aux.cnh, sizeof(aux.cnh), stdin);

            if(strlen(aux.cnh) == 12){

                do{
                    printf("\n%51cConfirmar CNH\n%51c[S/N]: ", B, B);
                    fflush(stdin);
                    scanf("%c", &confirmacao);
                    confirmacao = toupper(confirmacao);
                    printf("\n");
                    if(confirmacao == 'N'){
                        repetir = 1;
                        repetir2 = 0;
                    } else if(confirmacao == 'S'){
                        repetir = 0;
                        repetir2 = 0;
                    } else {
                        printf("%51cDado invalido!\n", B);
                        repetir2 = 1;
                    }
                } while(repetir2 == 1);

            } else {
                printf("%51cCNH invalido!\n\n%51cInserir novo CNH", B, B);
                repetir = 1;
            }

        } while(repetir == 1);

        do{
            printf("\n%24cCategoria: ", B);
            fflush(stdin);
            fgets(aux.categoria, sizeof(aux.categoria), stdin);

            if(strlen(aux.categoria) <= 3){

                do{
                    printf("\n%51cConfirmar Categoria\n%51c[S/N]: ", B, B);
                    fflush(stdin);
                    scanf("%c", &confirmacao);
                    confirmacao = toupper(confirmacao);
                    printf("\n");
                    if(confirmacao == 'N'){
                        repetir = 1;
                        repetir2 = 0;
                    } else if(confirmacao == 'S'){
                        repetir = 0;
                        repetir2 = 0;
                    } else {
                        printf("%51cDado invalido!\n", B);
                        repetir2 = 1;
                    }
                } while(repetir2 == 1);

            } else {
                printf("%51cCategoria invalida!\n\n%51cInserir nova categoria", B, B);
                repetir = 1;
            }

        } while(repetir == 1);
        Maiuscula(aux.categoria);

        do{
            printf("\n%24cRenach: ", B);
            fflush(stdin);
            fgets(aux.renach, sizeof(aux.renach), stdin);

            if(strlen(aux.renach) == 12){

                do{
                    printf("\n%51cConfirmar Renach\n%51c[S/N]: ", B, B);
                    fflush(stdin);
                    scanf("%c", &confirmacao);
                    confirmacao = toupper(confirmacao);
                    printf("\n");
                    if(confirmacao == 'N'){
                        repetir = 1;
                        repetir2 = 0;
                    } else if(confirmacao == 'S'){
                        repetir = 0;
                        repetir2 = 0;
                    } else {
                        printf("%51cDado invalido!\n", B);
                        repetir2 = 1;
                    }
                } while(repetir2 == 1);

            } else {
                printf("%51cRenach invalido!\n\n%51cInserir novo Renach", B, B);
                repetir = 1;
            }
        } while(repetir == 1);
        Maiuscula(aux.renach);

        do{
            printf("\n%24cEmissao (Sem Barras): ", B);
            fflush(stdin);
            fgets(aux.emissao, sizeof(aux.emissao), stdin);

            if(strlen(aux.emissao) == 9 && VerificarNumerais(aux.emissao) == 1 && CompararNascimentoEmissao(nascimento, aux.emissao) == 1 && VerificarEmissao(aux.emissao) == 1){
                do{
                    printf("\n%51cConfirmar Emissao\n%51c[S/N]: ", B, B);
                    fflush(stdin);
                    scanf("%c", &confirmacao);
                    confirmacao = toupper(confirmacao);
                    printf("\n");
                    if(confirmacao == 'N'){
                        repetir = 1;
                        repetir2 = 0;
                    } else if(confirmacao == 'S'){
                        repetir = 0;
                        repetir2 = 0;
                    } else {
                        printf("%51cDado invalido!\n", B);
                        repetir2 = 1;
                    }
                } while(repetir2 == 1);
            } else {
                printf("%51cData invalida!\n\n%51cInserir nova data", B, B);
                repetir = 1;
            }

        } while(repetir == 1);
        strcpy(emissao, aux.emissao);
        FormatarData(aux.emissao);

        do{
            printf("\n%24cValidade (Sem Barras): ", B);
            fflush(stdin);
            fgets(aux.validade, sizeof(aux.validade), stdin);

            if(strlen(aux.validade) == 9 && VerificarNumerais(aux.validade) == 1 && CompararEmissaoValidade(emissao, aux.validade) == 1){
                do{
                    printf("\n%51cConfirmar Validade\n%51c[S/N]: ", B, B);
                    fflush(stdin);
                    scanf("%c", &confirmacao);
                    confirmacao = toupper(confirmacao);
                    printf("\n");
                    if(confirmacao == 'N'){
                        repetir = 1;
                        repetir2 = 0;
                    } else if(confirmacao == 'S'){
                        repetir = 0;
                        repetir2 = 0;
                    } else {
                        printf("%51cDado invalido!\n", B);
                        repetir2 = 1;
                    }
                } while(repetir2 == 1);
            } else {
                printf("%51cData invalida!\n\n%51cInserir nova data", B, B);
                repetir = 1;
            }

        } while(repetir == 1);
        if(VerificarValidade(aux.validade) == 0){
            aux.desvalidou = true;
        } else {
            aux.desvalidou = false;
        }
        FormatarData(aux.validade);

        do{
            printf("\n%24cTelefone: ", B);
            fflush(stdin);
            fgets(aux.telefone, sizeof(aux.telefone), stdin);

            if(strlen(aux.telefone) == 12){

                do{
                    printf("\n%51cConfirmar Telefone\n%51c[S/N]: ", B, B);
                    fflush(stdin);
                    scanf("%c", &confirmacao);
                    confirmacao = toupper(confirmacao);
                    printf("\n");
                    if(confirmacao == 'N'){
                        repetir = 1;
                        repetir2 = 0;
                    } else if(confirmacao == 'S'){
                        repetir = 0;
                        repetir2 = 0;
                    } else {
                        printf("%51cDado invalido!\n", B);
                        repetir2 = 1;
                    }
                } while(repetir2 == 1);

            } else {
                printf("%51cTelefone invalido!\n\n%51cInserir novo Telefone", B, B);
                repetir = 1;
            }

        } while(repetir == 1);

        do{
            printf("\n%24cInfracoes: ", B);
            scanf("%d", &aux.infracoes);

            if(aux.infracoes >= 21){
                aux.suspensao = true;
            } else {
                aux.suspensao = false;
            }

            do{
                printf("\n%51cConfirmar Infracoes\n%51c[S/N]: ", B, B);
                fflush(stdin);
                scanf("%c", &confirmacao);
                confirmacao = toupper(confirmacao);
                if(confirmacao == 'N'){
                    repetir = 1;
                    repetir2 = 0;
                } else if(confirmacao == 'S'){
                    repetir = 0;
                    repetir2 = 0;
                } else {
                    printf("\n%51cDado invalido!\n", B);
                    repetir2 = 1;
                }
            } while(repetir2 == 1);

        } while(repetir == 1);
        printf("\n\n");

        arq = fopen(ARQUIVO,"r+t");
        if(arq == NULL){
            arq = fopen(ARQUIVO,"a+t");
            if(arq == NULL){
                fprintf(stderr,"Nao se pode criar %s\n", ARQUIVO);
                exit(1);
            }
        }

        fclose(arq);

        arq = fopen(ARQUIVO, "a");

        fprintf(arq,"%s", aux.cpf);
        fprintf(arq, "\n");
        fprintf(arq,"%s", aux.nome);
        fprintf(arq,"%d", aux.idade);
        fprintf(arq, "\n");
        fprintf(arq,"%s", aux.sexo);
        fprintf(arq,"%s", aux.nascimento);
        fprintf(arq, "\n");
        fprintf(arq,"%s", aux.estado);
        fprintf(arq,"%s", aux.municipio);
        fprintf(arq,"%s", aux.endereco);
        fprintf(arq,"%s", aux.rg);
        if(aux.suspensao == true){
            aux.cnh[strlen(aux.cnh) - 1] = ' ';
            fprintf(arq,"%s", aux.cnh);
            fprintf(arq, "(SUSPENSO)"); //CASO OS PONTOS DA CNH EXCEDAM 21, A MESMA FICARÁ SUSPENSA.
            fprintf(arq, "\n");
        } else {
            fprintf(arq,"%s", aux.cnh);
        }
        fprintf(arq,"%s", aux.categoria);
        fprintf(arq,"%s", aux.renach);
        fprintf(arq,"%s", aux.emissao);
        fprintf(arq, "\n");
        if(aux.desvalidou == true){
            aux.validade[strlen(aux.validade) - 1] = ' ';
            fprintf(arq,"%s", aux.validade);
            fprintf(arq, "(VENCIDA)"); //CASO A DATA DE VENCIMENTO SEJA ANTERIOR A DATA ATUAL, A CNH FICA VENCIDA.
            fprintf(arq, "\n");
        } else {
            fprintf(arq,"%s", aux.validade);
            fprintf(arq, "\n");
        }
        fprintf(arq,"%s", aux.telefone);
        fprintf(arq,"%d", aux.infracoes);
        fprintf(arq, "\n\n");
        printf("%45cDados inseridos com sucesso!\n\n", B);

        fclose(arq);
    }

    printf("%40cVoce deseja realizar mais alguma acao?\n\n%55c[S/N]: ", B, B);
    do{
        fflush(stdin);
        scanf("%c", &o);
        o = toupper(o);
        if(o == 'S'){
            printf("\n\n%56cCerto!\n\n", B);
            A = 1;
            Escolha();
        } else if(o == 'N'){
            printf("\n\n%45cMuito obrigado! Volte sempre!\n\n", B);
            A = 1;
        } else {
            printf("\n\n%40cDado invalido! Favor inserir novamente!\n\n%55c[S/N]: ", B, B);
        }
    } while(A == 0);
}

void Buscar(){ //BUSCA UM CADASTRO NO BANCO DE DADOS//
    char CPF[15], cpf[15], aux[100], o;
    int find = 0, i = 0, A = 0;

    printf("\n%34cDigite o CPF da pessoa que voce deseja encontrar:\n\n", B);
    fflush(stdin);
    printf("%53c", B);
    gets(cpf);
    FormatarCPF(cpf);

    arq = fopen(ARQUIVO,"r");
    if(arq == NULL){
        printf("\n\n%48cArquivo nao existe!\n%39cVoce precisa inserir algo nele antes!\n", B, B);
        exit(1);
    }

    while(!feof(arq)){
        fgets(CPF, 15, arq);
        if(strcmp(cpf, CPF) == 0 && find == 0){
            find = 1;
            printf("\n%30cPessoa encontrada! Seus dados serao encontrados abaixo:\n\n", B);
            printf("             ========================================================================================   \n\n");
            printf("            |%34c CPF: %s\n", B, CPF);
            break;
        }
    }

    if(find == 1){
        while(i != 16){
            fflush(stdin);
            fgets(aux, 100, arq);
            aux[strlen(aux) - 1] = ' ';
            i++;
            if(i == 2){
                printf("            |%34c Nome: %s\n", B, aux);
            } else if(i == 3){
                printf("            |%34c Idade: %s\n", B, aux);
            } else if(i == 4){
                printf("            |%34c Sexo: %s\n", B, aux);
            } else if(i == 5){
                printf("            |%34c Nascimento: %s\n", B, aux);
            } else if(i == 6){
                printf("            |%34c Estado: %s\n", B, aux);
            } else if(i == 7){
                printf("            |%34c Municipio: %s\n", B, aux);
            } else if(i == 8){
                printf("            |%34c Endereço: %s\n", B, aux);
            } else if(i == 9){
                printf("            |%34c RG: %s\n", B, aux);
            } else if(i == 10){
                printf("            |%34c CNH: %s\n", B, aux);
            } else if(i == 11){
                printf("            |%34c Categoria: %s\n", B, aux);
            } else if(i == 12){
                printf("            |%34c Renach: %s\n", B, aux);
            } else if(i == 13){
                printf("            |%34c Emissao: %s\n", B, aux);
            } else if(i == 14){
                printf("            |%34c Validade: %s\n", B, aux);
            } else if(i == 15){
                printf("            |%34c Telefone: %s\n", B, aux);
            } else if(i == 16){
                printf("            |%34c Infracoes: %s\n", B, aux);
            }
            if(i > 0 && i != 16){
                printf("\n");
            }
        }
        printf("             ========================================================================================   \n\n");
    } else {
        printf("\n%47cCadastro nao encontrado!\n\n", B);
    }

    fclose(arq);

    printf("%40cVoce deseja realizar mais alguma acao?\n\n%55c[S/N]: ", B, B);
    do{
        fflush(stdin);
        scanf("%c", &o);
        o = toupper(o);
        if(o == 'S'){
            printf("\n\n%56cCerto!\n\n", B);
            A = 1;
            Escolha();
        } else if(o == 'N'){
            printf("\n\n%45cMuito obrigado! Volte sempre!\n\n", B);
            A = 1;
        } else {
            printf("\n\n%40cDado invalido! Favor inserir novamente!\n\n%55c[S/N]: ", B, B);
        }
    } while(A == 0);
}

void Remover(){ //REMOVE UM CADASTTO NO BANCO DE DADOS//
    FILE *arqAux;
    char Aux[100], CPF[100], cpf[15], o;
    int find = 0, i = 0, j, k = 0, cont = 0, A = 0, linhas = 0, T;

    printf("\n%36cDigite o CPF da pessoa que voce deseja remover:\n\n", B);
    fflush(stdin);
    printf("%53c", B);
    gets(cpf);
    FormatarCPF(cpf);

    arq = fopen(ARQUIVO, "r");
    if(arq == NULL){
        printf("\n\n%43c%s esta vazio!\n%39cVoce precisa inserir algo nele antes!\n", B, ARQUIVO, B);
        exit(1);
    }

    arqAux = fopen("ArquivoAuxiliar.txt", "a");
    if(arq == NULL){
        printf("\n\nERRO!");
        exit(1);
    }

    while(!feof(arq)){
        fgets(Aux, 100, arq);
        linhas++;
    }
    T = linhas;

    rewind(arq);
    while(!feof(arq)){
        fgets(CPF, 100, arq);
        if(strncmp(CPF, cpf, strlen(cpf)) == 0){
            find = 1;
            break;
        }
        i++;
    }
    linhas = 0;
    if(find == 1){
        find = 0;
        rewind(arq);
        while(!feof(arq)){
            linhas++;
            fflush(stdin);
            fgets(Aux, 100, arq);
            if(k != i && find == 0){
                if(linhas < T){
                    fprintf(arqAux, "%s", Aux);
                }
            } else if(k == i && find == 0){
                find = 1;
                cont++;
            } else if(cont < 16){
                cont++;
            } else if(cont == 16){
                find = 0;
            }
            k++;
        }

        fclose(arq);
        fclose(arqAux);

        remove(ARQUIVO);
        rename("ArquivoAuxiliar.txt", ARQUIVO);

        printf("\n%44cCadastro Removido com sucesso!\n\n", B);
    } else {
        printf("\n%50cCPF nao cadastrado!\n\n", B);

        fclose(arq);
        fclose(arqAux);

        remove("ArquivoAuxiliar.txt");
    }

    printf("%40cVoce deseja realizar mais alguma acao?\n\n%55c[S/N]: ", B, B);
    do{
        fflush(stdin);
        scanf("%c", &o);
        o = toupper(o);
        if(o == 'S'){
            printf("\n\n%56cCerto!\n\n", B);
            A = 1;
            Escolha();
        } else if(o == 'N'){
            printf("\n\n%45cMuito obrigado! Volte sempre!\n\n", B);
            A = 1;
        } else {
            printf("\n\n%40cDado invalido! Favor inserir novamente!\n\n%55c[S/N]: ", B, B);
        }
    } while(A == 0);
}

void ImprimirDados(){ //IMPRIME TODOS OS DADOS EXISTENTES NO BANCO DE DADOS//
    int i = 0, A = 0, cont = 1, tamanho = 0, j;
    char aux[100], o;
    arq = fopen(ARQUIVO,"r");
    if(arq == NULL){
        printf("\n\n%43c%s esta vazio!\n%39cVoce precisa inserir algo nele antes!\n", B, ARQUIVO, B);
        exit(1);
    }

    while(!feof(arq)){
        if(i == 17){
            i = 0;
            tamanho++;
        }
        fflush(stdin);
        fgets(aux, 100, arq);
        i++;
    }

    printf("\n *********************************************** CADASTROS DO DETRAN *************************************************\n\n", B);

    printf("\n\n             ============================================ 1 ==========================================   \n\n");

    rewind(arq);
    i = 0;
    while(!feof(arq)){
        if(i == 17 && cont != tamanho){
            i = 0;
            cont++;
            printf("             ============================================ %d ==========================================   \n\n", cont);
        } else if(cont == tamanho && i == 17){
            printf("             *****************************************************************************************   \n\n");
            break;
        }
        fflush(stdin);
        fgets(aux, 100, arq);
        aux[strlen(aux) - 1] = ' ';
        i++;
        if(i == 1){
            printf("             |%33c CPF: %s\n", B, aux);
        } else if(i == 2){
            printf("             |%33c Nome: %s\n", B, aux);
        } else if(i == 3){
            printf("             |%33c Idade: %s\n", B, aux);
        } else if(i == 4){
            printf("             |%33c Sexo: %s\n", B, aux);
        } else if(i == 5){
            printf("             |%33c Nascimento: %s\n", B, aux);
        } else if(i == 6){
            printf("             |%33c Estado: %s\n", B, aux);
        } else if(i == 7){
            printf("             |%33c Municipio: %s\n", B, aux);
        } else if(i == 8){
            printf("             |%33c Endereco: %s\n", B, aux);
        } else if(i == 9){
            printf("             |%33c RG: %s\n", B, aux);
        } else if(i == 10){
            printf("             |%33c CNH: %s\n", B, aux);
        } else if(i == 11){
            printf("             |%33c Categoria: %s\n", B, aux);
        } else if(i == 12){
            printf("             |%33c Renach: %s\n", B, aux);
        } else if(i == 13){
            printf("             |%33c Emissao: %s\n", B, aux);
        } else if(i == 14){
            printf("             |%33c Validade: %s\n", B, aux);
        } else if(i == 15){
            printf("             |%33c Telefone: %s\n", B, aux);
        } else if(i == 16){
            printf("             |%33c Infracoes: %s\n\n", B, aux);
        }

    }

    fclose(arq);

    printf("%40cVoce deseja realizar mais alguma acao?\n\n%55c[S/N]: ", B, B);
    do{
        fflush(stdin);
        scanf("%c", &o);
        o = toupper(o);
        if(o == 'S'){
            printf("\n\n%56cCerto!\n\n", B);
            A = 1;
            Escolha();
        } else if(o == 'N'){
            printf("\n\n%45cMuito obrigado! Volte sempre!\n\n", B);
            A = 1;
        } else {
            printf("\n\n%40cDado invalido! Favor inserir novamente!\n\n%55c[S/N]: ", B, B);
        }
    } while(A == 0);
}

void AdicionarInfracao(){ //ADICIONA MAIS INFRAÇÕES AO CADASTRADO SOLICITADO//
    FILE *arqAux;
    int find = 0, infra, i = 1, N, cont = 0, A, c = 0, linhas = 0, T;
    char aux[100], cpf[15], CPF[15], I[10], o;
    printf("\n%35cDigite o CPF do usuario que cometeu a infracao:");
    printf("\n%52c", B);
    fflush(stdin);
    printf(" ");
    gets(cpf);
    FormatarCPF(cpf);

    arq = fopen(ARQUIVO,"r");
    if(arq == NULL){
        printf("\n\n%43c%s esta vazio!\n%39cVoce precisa inserir algo nele antes!\n", B, ARQUIVO, B);
        exit(1);
    }

    rewind(arq);

    while(!feof(arq)){
        fgets(aux, 100, arq);
        linhas++;
    }
    T = linhas;
    rewind(arq);

    while(!feof(arq)){
        fflush(stdin);
        fgets(CPF, 15, arq);
        cont++;
        if(strncmp(cpf,CPF, 15) == 0){
            find = 1;
            break;
        }
    }
    while(i != 16){
        fflush(stdin);
        fgets(aux, 100, arq);
        i++;
    }

    if(find == 1){
        printf("\n%41cIndique o tipo de infracao cometida:\n%31c[1] Leve\n%31c[2] Media\n%31c[3] Grave\n%31c[4] Gravissima\n", B, B, B, B, B);
        printf("\n%58c", B);
        scanf("%d", &infra);
        if(infra == 1){
            fgets(I, 10, arq);
            N = TransformarEmNumero(I) + 3;
        } else if(infra == 2){
            fgets(I, 10, arq);
            N = TransformarEmNumero(I) + 4;
        } else if(infra == 3){
            fgets(I, 10, arq);
            N = TransformarEmNumero(I) + 5;
        } else if(infra == 4){
            fgets(I, 10, arq);
            N = TransformarEmNumero(I) + 7;
        }

        arqAux = fopen("ArquivoAuxiliar.txt", "w");
        if(arq == NULL){
            printf("\n\nERRO!");
            exit(1);
        }
        i = 0;
        rewind(arq);
        while(!feof(arq)){
            if(i == 0){
                fflush(stdin);
                fgets(aux, 15, arq);
                fprintf(arqAux, "%s", aux);
                i++;
            }
            if(strncmp(cpf,aux, 15) == 0){
                while(c != 15){
                    if(N >= 21){
                        if(c != 9){
                            fgets(aux, 100, arq);
                            fprintf(arqAux, "%s", aux);
                        } else {
                            fgets(aux, 100, arq);
                            aux[strlen(aux) - 1] = ' ';
                            fprintf(arqAux, "%s", aux);
                            fprintf(arqAux, "(SUSPENSO)"); //CASO OS PONTOS NA CNH EXCEDAM 21, A MESMA SERÁ SUSPENSA
                            fprintf(arqAux, "\n");
                        }
                    } else {
                        fgets(aux, 100, arq);
                        fprintf(arqAux, "%s", aux);
                    }
                    c++;
                }
                fgets(aux, 100, arq);
                fprintf(arqAux, "%d", N);
                fprintf(arqAux, "\n");
            } else {
                fgets(aux, 15, arq);
                fprintf(arqAux, "%s", aux);
            }
        }

        fclose(arq);
        fclose(arqAux);

        arq = fopen(ARQUIVO, "w+t");
        arqAux = fopen("ArquivoAuxiliar.txt", "r+t");
        linhas = 0;
        while(!feof(arqAux) && linhas < T - 1){
                fflush(stdin);
                fgets(aux, 100, arqAux);
                fprintf(arq, "%s", aux);
                linhas++;
        }

        fclose(arq);
        fclose(arqAux);

        remove("ArquivoAuxiliar.txt");

        printf("\n%43cInfracao adicionada com sucesso!\n", B);

    } else {
        printf("\n%48cUsuario nao cadastrado!", B);
        fclose(arq);
    }
    printf("\n");
    printf("\n%40cVoce deseja realizar mais alguma acao?\n\n%55c[S/N]: ", B, B);
    do{
        fflush(stdin);
        scanf("%c", &o);
        o = toupper(o);
        if(o == 'S'){
            printf("\n\n%56cCerto!\n\n", B);
            A = 1;
            Escolha();
        } else if(o == 'N'){
            printf("\n\n%45cMuito obrigado! Volte sempre!\n\n", B);
            A = 1;
        } else {
            printf("\n\n%40cDado invalido! Favor inserir novamente!\n\n%55c[S/N]: ", B, B);
        }
    } while(A == 0);

}

void RemoverSuspensao(){ //REMOVE A SUSPENSÃO DA CNH//
    FILE *arqAux;
    int findcpf = 0, infra, i = 1, N, findcnh = 0, A, c = 0, j, k = 0, cont = 0, linhas = 0, T;
    char aux[100], cpf[15], CPF[15], I[10], o, cnh[15];
    printf("\n%27cDigite o CPF do usuario cuja suspensao da CNH deseja-se remover:");
    printf("\n%52c", B);
    fflush(stdin);
    printf(" ");
    gets(cpf);
    FormatarCPF(cpf);

    arq = fopen(ARQUIVO,"r");
    if(arq == NULL){
        printf("\n\n%43c%s esta vazio!\n%39cVoce precisa inserir algo nele antes!\n", B, ARQUIVO, B);
        exit(1);
    }

     rewind(arq);

    while(!feof(arq)){
        fgets(aux, 100, arq);
        linhas++;
    }
    T = linhas;
    rewind(arq);

    while(!feof(arq)){
        fgets(CPF, 15, arq);
        if(strncmp(cpf,CPF, 15) == 0){
            findcpf = 1;
            while(i != 11){
                fgets(aux, 100, arq);
                i++;
            }
            for(j = 0; j < strlen(aux) + 10; j++){
                if(aux[j] == ' ' && aux[j + 1] == '(' && aux[j + 2] == 'S'){
                    findcnh = 1;
                    break;
                }
            }
        }
    }

    if(findcnh == 1){

        arqAux = fopen("ArquivoAuxiliar.txt", "w");
        if(arq == NULL){
            printf("\n\nERRO!");
            exit(1);
        }

        i = 0;
        rewind(arq);
        while(!feof(arq)){
            if(i == 0){
                fflush(stdin);
                fgets(aux, 15, arq);
                fprintf(arqAux, "%s", aux);
                i++;
            }
            if(strncmp(cpf,aux, 15) == 0){
                while(c != 15){
                    fflush(stdin);
                    if(c != 9){
                        fgets(aux, 100, arq);
                        fprintf(arqAux, "%s", aux);
                    } else if(c == 9){
                        fgets(aux, 100, arq);
                        for(j = 0; j < strlen(aux); j++){
                            if(aux[j] == ' '){
                                break;
                            } else {
                                cnh[k] = aux[j];
                                k++;
                            }
                        }
                        fprintf(arqAux, "%s", cnh);
                        fprintf(arqAux, "\n");
                    } else {
                        fgets(aux, 100, arq);
                        fprintf(arqAux, "%s", aux);
                    }
                    c++;
                }
                fgets(aux, 100, arq);
                fprintf(arqAux, "0");
                fprintf(arqAux, "\n");
            } else {
                fgets(aux, 15, arq);
                cont++;
                fprintf(arqAux, "%s", aux);
            }
        }
        if(cont == 1){
            fprintf(arqAux, "\n");
        }

        fclose(arq);
        fclose(arqAux);

        arq = fopen(ARQUIVO, "w+t");
        arqAux = fopen("ArquivoAuxiliar.txt", "r+t");
        linhas = 0;
        while(!feof(arqAux) && linhas < T - 1){
                fflush(stdin);
                fgets(aux, 100, arqAux);
                fprintf(arq, "%s", aux);
                linhas++;
        }

        fclose(arq);
        fclose(arqAux);

        remove("ArquivoAuxiliar.txt");

        printf("\n%45cCNH modificada com sucesso!\n", B);

    } else if(findcpf == 1 && findcnh == 0){
        printf("\n%48cCNH nao esta suspensa!\n", B);
        fclose(arq);
    } else if(findcpf == 0){
        printf("\n%48cUsuario nao cadastrado!\n", B);
        fclose(arq);
    }

    printf("\n%40cVoce deseja realizar mais alguma acao?\n\n%55c[S/N]: ", B, B);
    do{
        fflush(stdin);
        scanf("%c", &o);
        o = toupper(o);
        if(o == 'S'){
            printf("\n\n%56cCerto!\n\n", B);
            A = 1;
            Escolha();
        } else if(o == 'N'){
            printf("\n\n%45cMuito obrigado! Volte sempre!\n\n", B);
            A = 1;
        } else {
            printf("\n\n%40cDado invalido! Favor inserir novamente!\n\n%55c[S/N]: ", B, B);
        }
    } while(A == 0);
}

void RenovarCNH(){ //REMOVA A CNH DE ACORDO COM O DIA ATUAL//
    time_t mytime;
    mytime = time(NULL);
    struct tm tm = *localtime(&mytime);

    FILE *arqAux;
    int findcpf = 0, infra, i = 1, N, findcnh = 0, A, c = 0, j, k = 0, cont = 0, linhas = 0, T;
    char aux[100], cpf[15], CPF[15], I[10], o, validade[11];
    printf("\n%33cDigite o CPF do usuario cuja CNH deseja-se renovar:");
    printf("\n%52c", B);
    fflush(stdin);
    printf(" ");
    gets(cpf);
    FormatarCPF(cpf);

    arq = fopen(ARQUIVO,"r");
    if(arq == NULL){
        printf("\n\n%43c%s esta vazio!\n%39cVoce precisa inserir algo nele antes!\n", B, ARQUIVO, B);
        exit(1);
    }

    rewind(arq);

    while(!feof(arq)){
        fgets(aux, 100, arq);
        linhas++;
    }
    T = linhas;
    rewind(arq);

    while(!feof(arq)){
        fgets(CPF, 15, arq);
        if(strncmp(cpf,CPF, 15) == 0){
            findcpf = 1;
            while(i != 15){
                fgets(aux, 100, arq);
                i++;
            }
            for(j = 0; j < strlen(aux) + 10; j++){
                if(aux[j] == ' ' && aux[j + 1] == '(' && aux[j + 2] == 'V'){
                    findcnh = 1;
                    break;
                }
            }
        }
    }

    if(findcnh == 1){

        arqAux = fopen("ArquivoAuxiliar.txt", "w");
        if(arq == NULL){
            printf("\n\nERRO!");
            exit(1);
        }

        i = 0;
        rewind(arq);
        while(!feof(arq)){
            if(i == 0){
                fflush(stdin);
                fgets(aux, 15, arq);
                fprintf(arqAux, "%s", aux);
                i++;
            }
            if(strncmp(cpf,aux, 15) == 0){
                while(c != 15){
                    fflush(stdin);
                    if(c != 13 && c != 12){
                        fgets(aux, 100, arq);
                        fprintf(arqAux, "%s", aux);
                    } else if(c == 13){
                        fgets(aux, 100, arq);

                        if(tm.tm_mday < 10 && tm.tm_mon + 1 < 10){
                            fprintf(arqAux, "0%d/0%d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900 + 5);
                            fprintf(arqAux, "\n");
                        } else if(tm.tm_mday < 10 && tm.tm_mon + 1 >= 10){
                            fprintf(arqAux, "0%d/%d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900 + 5);
                            fprintf(arqAux, "\n");
                        } else if(tm.tm_mday >= 10 && tm.tm_mon + 1 < 10){
                            fprintf(arqAux, "%d/0%d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900 + 5);
                            fprintf(arqAux, "\n");
                        } else if(tm.tm_mday >= 10 && tm.tm_mon + 1 >= 10){
                            fprintf(arqAux, "%d/%d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900 + 5);
                            fprintf(arqAux, "\n");
                        }

                    } else if(c == 12){
                        fgets(aux, 100, arq);

                        if(tm.tm_mday < 10 && tm.tm_mon + 1 < 10){
                            fprintf(arqAux, "0%d/0%d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
                            fprintf(arqAux, "\n");
                        } else if(tm.tm_mday < 10 && tm.tm_mon + 1 >= 10){
                            fprintf(arqAux, "0%d/%d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
                            fprintf(arqAux, "\n");
                        } else if(tm.tm_mday >= 10 && tm.tm_mon + 1 < 10){
                            fprintf(arqAux, "%d/0%d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
                            fprintf(arqAux, "\n");
                        } else if(tm.tm_mday >= 10 && tm.tm_mon + 1 >= 10){
                            fprintf(arqAux, "%d/%d/%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
                            fprintf(arqAux, "\n");
                        }

                    } else {
                        fgets(aux, 100, arq);
                        fprintf(arqAux, "%s", aux);
                    }
                    c++;
                }
                fgets(aux, 100, arq);
                fprintf(arqAux, "%s", aux);
            } else {
                fgets(aux, 15, arq);
                cont++;
                fprintf(arqAux, "%s", aux);
            }
        }
        if(cont == 1){
            fprintf(arqAux, "\n");
        }

        fclose(arq);
        fclose(arqAux);

        arq = fopen(ARQUIVO, "w+t");
        arqAux = fopen("ArquivoAuxiliar.txt", "r+t");
        linhas = 0;
        while(!feof(arqAux) && linhas < T - 1){
                fflush(stdin);
                fgets(aux, 100, arqAux);
                fprintf(arq, "%s", aux);
                linhas++;
        }

        fclose(arq);
        fclose(arqAux);

        remove("ArquivoAuxiliar.txt");

        printf("\n%46cCNH renovada com sucesso!\n", B);

    } else if(findcpf == 1 && findcnh == 0){
        printf("\n%48cCNH nao esta vencida!\n", B);
        fclose(arq);
    } else if(findcpf == 0){
        printf("\n%48cUsuario nao cadastrado!\n", B);
        fclose(arq);
    }

    printf("\n%40cVoce deseja realizar mais alguma acao?\n\n%55c[S/N]: ", B, B);
    do{
        fflush(stdin);
        scanf("%c", &o);
        o = toupper(o);
        if(o == 'S'){
            printf("\n\n%56cCerto!\n\n", B);
            A = 1;
            Escolha();
        } else if(o == 'N'){
            printf("\n\n%45cMuito obrigado! Volte sempre!\n\n", B);
            A = 1;
        } else {
            printf("\n\n%40cDado invalido! Favor inserir novamente!\n\n%55c[S/N]: ", B, B);
        }
    } while(A == 0);
}

int main(){
    printf("\n ********************************** Seja bem vindo ao sistema de cadastro do Detran **********************************\n\n%46cO que voce deseja realizar:\n\n", B, B);
    Escolha(); //CHAMA A FUNÇÃO QUE MOSTRA AS ALTERNATIVAS DO PROGRAMA
    return 0;
}
