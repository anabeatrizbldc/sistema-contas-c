#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define ARQ "contas.dat"

typedef struct {
    int conta;
    char nome[50];
    float saldo;
} Cliente;

FILE *fp;

void cadastrar() {
    int pos;
    Cliente c, aux;

    printf("Informe a posicao (0 a %d): ", MAX - 1);
    scanf("%d", &pos);
    fflush(stdin);

    if (pos < 0 || pos >= MAX) {
        printf("Posicao invalida!\n");
        return;
    }

    fseek(fp, pos * sizeof(Cliente), SEEK_SET);
    fread(&aux, sizeof(Cliente), 1, fp);

    if (aux.conta != 0) {
        printf("Posicao ja cadastrada!\n");
        return;
    }

    printf("Numero da conta: ");
    scanf("%d", &c.conta);
    fflush(stdin);

    printf("Nome do cliente: ");
    fgets(c.nome, 50, stdin);
    c.nome[strlen(c.nome) - 1] = '\0';

    printf("Saldo: ");
    scanf("%f", &c.saldo);
    fflush(stdin);

    fseek(fp, pos * sizeof(Cliente), SEEK_SET);
    fwrite(&c, sizeof(Cliente), 1, fp);

    printf("Cliente cadastrado com sucesso!\n");
}

void consultar() {
    int num, i, n;
    Cliente c;

    printf("Numero da conta: ");
    scanf("%d", &num);
    fflush(stdin);

    fseek(fp, 0, SEEK_END);
    n = ftell(fp) / sizeof(Cliente);

    for (i = 0; i < n; i++) {
        fseek(fp, i * sizeof(Cliente), SEEK_SET);
        fread(&c, sizeof(Cliente), 1, fp);

        if (c.conta == num) {
            printf("\nConta  : %d\n", c.conta);
            printf("Nome   : %s\n", c.nome);
            printf("Saldo  : R$ %.2f\n", c.saldo);
            return;
        }
    }
    printf("Conta nao encontrada!\n");
}

void atualizar() {
    int num, i, n;
    Cliente c;

    printf("Numero da conta: ");
    scanf("%d", &num);
    fflush(stdin);

    fseek(fp, 0, SEEK_END);
    n = ftell(fp) / sizeof(Cliente);

    for (i = 0; i < n; i++) {
        fseek(fp, i * sizeof(Cliente), SEEK_SET);
        fread(&c, sizeof(Cliente), 1, fp);

        if (c.conta == num) {
            printf("Saldo atual: R$ %.2f\n", c.saldo);
            printf("Novo saldo : ");
            scanf("%f", &c.saldo);
            fflush(stdin);

            fseek(fp, i * sizeof(Cliente), SEEK_SET);
            fwrite(&c, sizeof(Cliente), 1, fp);

            printf("Saldo atualizado!\n");
            return;
        }
    }
    printf("Conta nao encontrada!\n");
}

void encerrar() {
    int num, i, n;
    Cliente c, vazio;

    printf("Numero da conta: ");
    scanf("%d", &num);
    fflush(stdin);

    memset(&vazio, 0, sizeof(Cliente));

    fseek(fp, 0, SEEK_END);
    n = ftell(fp) / sizeof(Cliente);

    for (i = 0; i < n; i++) {
        fseek(fp, i * sizeof(Cliente), SEEK_SET);
        fread(&c, sizeof(Cliente), 1, fp);

        if (c.conta == num) {
            fseek(fp, i * sizeof(Cliente), SEEK_SET);
            fwrite(&vazio, sizeof(Cliente), 1, fp);
            printf("Conta encerrada!\n");
            return;
        }
    }
    printf("Conta nao encontrada!\n");
}

void listar() {
    int i, n, achou = 0;
    Cliente c;

    fseek(fp, 0, SEEK_END);
    n = ftell(fp) / sizeof(Cliente);

    printf("\n%-5s %-8s %-30s %s\n", "Pos", "Conta", "Nome", "Saldo");
    printf("-----------------------------------------------------\n");

    for (i = 0; i < n; i++) {
        fseek(fp, i * sizeof(Cliente), SEEK_SET);
        fread(&c, sizeof(Cliente), 1, fp);

        if (c.conta != 0) {
            printf("%-5d %-8d %-30s R$ %.2f\n", i, c.conta, c.nome, c.saldo);
            achou = 1;
        }
    }

    if (!achou)
        printf("Nenhum cliente cadastrado.\n");
}

void relistar() {
    int pos = 0, achou = 0;
    Cliente c;

    rewind(fp);

    printf("\n%-5s %-8s %-30s %s\n", "Pos", "Conta", "Nome", "Saldo");
    printf("-----------------------------------------------------\n");

    while (fread(&c, sizeof(Cliente), 1, fp) == 1) {
        if (c.conta != 0) {
            printf("%-5d %-8d %-30s R$ %.2f\n", pos, c.conta, c.nome, c.saldo);
            achou = 1;
        }
        pos++;
    }

    if (!achou)
        printf("Nenhum cliente cadastrado.\n");
}

int main() {
    int op;

    fp = fopen(ARQ, "r+b");
    if (fp == NULL)
        fp = fopen(ARQ, "w+b");

    if (fp == NULL) {
        printf("Erro ao abrir arquivo!\n");
        return 1;
    }

    do {
        printf("\n===== SISTEMA DE CONTAS =====\n");
        printf("1 - Cadastrar cliente\n");
        printf("2 - Consultar cliente\n");
        printf("3 - Atualizar saldo\n");
        printf("4 - Encerrar conta\n");
        printf("5 - Listar clientes\n");
        printf("6 - Relistar (rewind)\n");
        printf("7 - Sair\n");
        printf("Opcao: ");
        scanf("%d", &op);
        fflush(stdin);

        switch (op) {
            case 1: cadastrar(); break;
            case 2: consultar(); break;
            case 3: atualizar(); break;
            case 4: encerrar();  break;
            case 5: listar();    break;
            case 6: relistar();  break;
            case 7: printf("Encerrando...\n"); break;
            default: printf("Opcao invalida!\n");
        }
    } while (op != 7);

    fclose(fp);
    return 0;
}
