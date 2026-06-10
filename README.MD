# Sistema de Manutenção de Contas

Sistema de gerenciamento de contas bancárias em C, usando arquivo binário com registros de tamanho fixo.

## Estrutura do registro

```c
typedef struct {
    int    numero;
    char   nome[50];
    double saldo;
} Cliente;
```

Cada registro ocupa exatamente `sizeof(Cliente)` bytes no arquivo `contas.dat`.

## Funcionalidades

| Opção | Descrição | Função C usada |
|-------|-----------|----------------|
| 1 | Cadastrar cliente em posição específica | `fseek` + `fwrite` |
| 2 | Consultar pelo número da conta | `fseek` + `fread` |
| 3 | Atualizar saldo | `fseek` + `fread`/`fwrite` |
| 4 | Encerrar conta (zera o slot) | `fseek` + `fwrite` |
| 5 | Listar todos os clientes | `fread` sequencial |
| 6 | Repetir listagem do início | `rewind` + `fread` |
| 7 | Encerrar o programa | — |

## Como compilar e executar

```bash
gcc -o contas contas.c
./contas        # Linux/macOS
contas.exe      # Windows
```

## Detalhes de implementação

- **Arquivo binário** (`contas.dat`): criado automaticamente na primeira execução.
- **Acesso direto**: `fseek(fp, pos * sizeof(Cliente), SEEK_SET)` posiciona o ponteiro no slot desejado, permitindo leitura/escrita O(1) por posição.
- **Slot vazio**: `numero == 0` indica posição livre; encerrar conta zera o registro com `memset`.
- **`rewind()`**: a opção 6 reposiciona o ponteiro para o início do arquivo e relê todos os registros sequencialmente.
