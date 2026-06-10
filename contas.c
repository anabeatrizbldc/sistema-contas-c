#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REGISTROS  100
#define ARQUIVO        "contas.dat"
#define NOME_MAX       50

typedef struct {
    int    conta;
    char   titular[NOME_MAX];
    double saldo;
} Registro;

static void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

static int gravar(FILE *fp, int pos, const Registro *r) {
    if (fseek(fp, (long)pos * (long)sizeof(Registro), SEEK_SET) != 0)
        return -1;
    return fwrite(r, sizeof(Registro), 1, fp) == 1 ? 0 : -1;
}

static int ler(FILE *fp, int pos, Registro *r) {
    if (fseek(fp, (long)pos * (long)sizeof(Registro), SEEK_SET) != 0)
        return -1;
    return fread(r, sizeof(Registro), 1, fp) == 1 ? 0 : -1;
}

static int num_registros(FILE *fp) {
    if (fseek(fp, 0, SEEK_END) != 0) return 0;
    return (int)(ftell(fp) / (long)sizeof(Registro));
}

static int buscar_conta(FILE *fp, int conta) {
    int n = num_registros(fp);
    for (int i = 0; i < n; i++) {
        Registro r;
        if (ler(fp, i, &r) == 0 && r.conta == conta)
            return i;
    }
    return -1;
}

static void op_cadastrar(FILE *fp) {
    int pos;
    printf("Posicao [0-%d]: ", MAX_REGISTROS - 1);
    if (scanf("%d", &pos) != 1) { flush_stdin(); return; }
    flush_stdin();

    if (pos < 0 || pos >= MAX_REGISTROS) {
        fprintf(stderr, "Posicao fora do intervalo permitido.\n");
        return;
    }

    Registro existente = {0};
    if (ler(fp, pos, &existente) == 0 && existente.conta != 0) {
        fprintf(stderr, "Posicao %d ja esta ocupada (conta %d).\n",
                pos, existente.conta);
        return;
    }

    Registro r = {0};
    printf("Numero da conta : "); scanf("%d",  &r.conta);   flush_stdin();
    printf("Titular         : "); fgets(r.titular, NOME_MAX, stdin);
    r.titular[strcspn(r.titular, "\n")] = '\0';
    printf("Saldo inicial   : "); scanf("%lf", &r.saldo);   flush_stdin();

    if (gravar(fp, pos, &r) != 0) {
        fprintf(stderr, "Falha ao gravar registro.\n");
        return;
    }
    printf("Conta %d registrada na posicao %d.\n", r.conta, pos);
}

static void op_consultar(FILE *fp) {
    int conta;
    printf("Numero da conta: ");
    if (scanf("%d", &conta) != 1) { flush_stdin(); return; }
    flush_stdin();

    int pos = buscar_conta(fp, conta);
    if (pos < 0) { fprintf(stderr, "Conta %d nao encontrada.\n", conta); return; }

    Registro r;
    ler(fp, pos, &r);
    printf("Posicao  : %d\n",      pos);
    printf("Conta    : %d\n",      r.conta);
    printf("Titular  : %s\n",      r.titular);
    printf("Saldo    : R$ %.2f\n", r.saldo);
}

static void op_atualizar(FILE *fp) {
    int conta;
    printf("Numero da conta: ");
    if (scanf("%d", &conta) != 1) { flush_stdin(); return; }
    flush_stdin();

    int pos = buscar_conta(fp, conta);
    if (pos < 0) { fprintf(stderr, "Conta %d nao encontrada.\n", conta); return; }

    Registro r;
    ler(fp, pos, &r);
    printf("Saldo atual : R$ %.2f\n", r.saldo);
    printf("Novo saldo  : ");
    if (scanf("%lf", &r.saldo) != 1) { flush_stdin(); return; }
    flush_stdin();

    if (gravar(fp, pos, &r) != 0) { fprintf(stderr, "Falha ao atualizar saldo.\n"); return; }
    printf("Saldo atualizado.\n");
}

static void op_encerrar(FILE *fp) {
    int conta;
    printf("Numero da conta: ");
    if (scanf("%d", &conta) != 1) { flush_stdin(); return; }
    flush_stdin();

    int pos = buscar_conta(fp, conta);
    if (pos < 0) { fprintf(stderr, "Conta %d nao encontrada.\n", conta); return; }

    Registro vazio = {0};
    if (gravar(fp, pos, &vazio) != 0) { fprintf(stderr, "Falha ao encerrar conta.\n"); return; }
    printf("Conta %d encerrada. Posicao %d liberada.\n", conta, pos);
}

static void imprimir_cabecalho(void) {
    printf("\n%-6s %-8s %-30s %s\n",
           "Pos", "Conta", "Titular", "Saldo");
    printf("%-6s %-8s %-30s %s\n",
           "------", "--------", "------------------------------", "----------");
}

static void op_listar(FILE *fp) {
    int n = num_registros(fp);
    int imprimiu = 0;

    imprimir_cabecalho();
    for (int i = 0; i < n; i++) {
        Registro r;
        if (ler(fp, i, &r) == 0 && r.conta != 0) {
            printf("%-6d %-8d %-30s R$ %.2f\n", i, r.conta, r.titular, r.saldo);
            imprimiu = 1;
        }
    }
    if (!imprimiu) printf("Nenhum registro ativo.\n");
}

static void op_rewind(FILE *fp) {
    rewind(fp);

    Registro r;
    int pos = 0, imprimiu = 0;

    imprimir_cabecalho();
    while (fread(&r, sizeof(Registro), 1, fp) == 1) {
        if (r.conta != 0) {
            printf("%-6d %-8d %-30s R$ %.2f\n", pos, r.conta, r.titular, r.saldo);
            imprimiu = 1;
        }
        pos++;
    }
    if (!imprimiu) printf("Nenhum registro ativo.\n");
}

int main(void) {
    FILE *fp = fopen(ARQUIVO, "r+b");
    if (!fp) fp = fopen(ARQUIVO, "w+b");
    if (!fp) { perror(ARQUIVO); return EXIT_FAILURE; }

    int opcao;
    do {
        printf("\n=== MANUTENCAO DE CONTAS ===\n"
               "1. Cadastrar\n"
               "2. Consultar\n"
               "3. Atualizar saldo\n"
               "4. Encerrar conta\n"
               "5. Listar\n"
               "6. Relistar (rewind)\n"
               "7. Sair\n"
               "Opcao: ");

        if (scanf("%d", &opcao) != 1) { flush_stdin(); continue; }
        flush_stdin();

        switch (opcao) {
            case 1: op_cadastrar(fp); break;
            case 2: op_consultar(fp); break;
            case 3: op_atualizar(fp); break;
            case 4: op_encerrar(fp);  break;
            case 5: op_listar(fp);    break;
            case 6: op_rewind(fp);    break;
            case 7: break;
            default: fprintf(stderr, "Opcao invalida.\n");
        }
    } while (opcao != 7);

    fclose(fp);
    return EXIT_SUCCESS;
}
