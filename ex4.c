/*
Ex4: Agenda
Igor Correa Domingues de Almeida
*/

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_NOME 30
#define TAM_END 40
#define TAM_FONE 13

typedef struct magenda {
  char key[TAM_NOME];
  unsigned pos;
  short erased;
} Tdado;

// Definicao do registro de dados
typedef struct registro {
  char nome[TAM_NOME];
  char endereco[TAM_END];
  char telefone[TAM_FONE];
  short apagado;
} registro;

//------------------------------------
// Definição da estrutura nó de árvore:
//------------------------------------
typedef struct TreeNode {
  Tdado info;             // dado armazenado no nó
  struct TreeNode *left;  // ponteiro para o ramo esquerdo
  struct TreeNode *right; // ponteiro para o nramo direito
} TreeNode;

FILE *arquivo;
int sizer = sizeof(registro);
TreeNode *arv = NULL; // Global

// Usado para depurar o codigo
void printBinTree(const TreeNode *bt, int level) {
  int i;

  if (!bt) {
    return;
  } else {
    printBinTree(bt->right, level + 1);
    for (i = 0; i < level; i++) {
      printf("----|");
    }
    printf("%s %d\n", bt->info.key, bt->info.pos);
    printBinTree(bt->left, level + 1);
  }
}

// Funcao para inserir um elemento na arvore
TreeNode *insertBinTree(char pal[], TreeNode *bt, int n) {
  TreeNode *aux = bt;
  int i;
  // verificar se o ponteiro é nulo
  if (!bt) {
    // insere o novo nó
    aux = (TreeNode *)malloc(sizeof(TreeNode));
    if (!aux) {
      printf("Erro: Memória insuficiente!\n");
      return bt;
    } else { // Set aux->info
      strcpy(aux->info.key, pal);
      aux->info.pos = n;
      aux->info.erased = 0;
      aux->left = NULL;
      aux->right = NULL;
      return aux;
    }
  } else {
    if ((i = strcmp(aux->info.key, pal)) >= 0) {
      aux->left = insertBinTree(pal, aux->left, n); // Ir para a esquerda
      return bt;
    } else {
      aux->right = insertBinTree(pal, aux->right, n); // Ir para a direita
      return bt;
    }
  }
}

// Funcao para contar o numero de nós na arvore
int numNodes(TreeNode *bt) {
  if (!bt)
    return 0;
  else {
    return 1 + numNodes(bt->right) + numNodes(bt->left);
  }
}

// Funcao para inserir os elementos da arvore no vetor
void emOrdem(TreeNode *bt, Tdado v[]) {
  static int i = 0;
  if (bt == NULL)
    return;

  emOrdem(bt->left, v);
  v[i] = bt->info;
  i++;
  emOrdem(bt->right, v);
}

// Funcao para construir arvore balanceada a partir do vetor
TreeNode *buildBalancedTree(Tdado v[], int low, int high) {
  TreeNode *aux = NULL;
  if (low > high) // Parar quando houver sobreposicao
    return aux;
  int mid = (low + high) / 2; // Metade entre os parametros

  // Similar à funcao InsertBinTree
  aux = (TreeNode *)malloc(sizeof(TreeNode));
  if (!aux) {
    printf("Erro: Memória insuficiente!\n");
    exit(1);
  } else {
    aux->info = v[mid]; // Inserir o elemento que esta no meio
    // Balanceamento das subárvores
    // Para a esquerda, usar a metade menor do vetor
    aux->left = buildBalancedTree(v, low, mid - 1);
    // Para a direita usar a metade maior do vetor
    aux->right = buildBalancedTree(v, mid + 1, high);
    return aux;
  }
}
/***************************************************
 * freeBinTree: Apaga toda a arvore com raiz em bt *
 ***************************************************/
void freeBinTree(TreeNode *bt) {
  if (!bt) {
    return;
  }
  freeBinTree(bt->left);
  freeBinTree(bt->right);
  free(bt);
  bt = NULL;
}

// Deleta um elemento da arvore por merge
// Deletar por copia causou muitos problemas
TreeNode *delTree(TreeNode *bt, registro x) {
  static int erase = 0;
  TreeNode *p, *p2;
  if (bt == NULL) { // árvore vazia ou chegou ao fim da árvore.
    return NULL;
  } else if ((!strcmp(bt->info.key, x.nome) ||
              strcmp(bt->info.key, x.nome) == 10) &&
             !erase) {
    //'\n' = 10, no fgets vem o '\n' junto, causando erros no strcmp
    erase = 1; // Nao apagar todos os nomes iguais
    // apaga a raiz da subárvore
    if (bt->left == bt->right && !erase) {
      // subárvore com apenas um elemento, a própria raiz.
      free(bt);
      return NULL;
    } else if (bt->left == NULL) {
      p = bt->right;
      free(bt);
      return p;
    } else if (bt->right == NULL) {
      p = bt->left;
      free(bt);
      return p;
    } else {
      p2 = bt->left;
      p = p2;
      while (p->right != NULL) {
        p = p->right;
      }
      p->right = bt->right;
      free(bt);
      return p2;
    }
  } else {
    if (strcmp(bt->info.key, x.nome) > 0) {
      bt->left = delTree(bt->left, x);
    } else {
      bt->right = delTree(bt->right, x);
    }
  }
  return bt;
}

// Funcao juntando as 2 necessarias para o balanceamento
TreeNode *balancedTree(TreeNode *bt, int low, int high) {
  Tdado v[numNodes(bt)]; // Vetor criado localmente
  TreeNode *aux = NULL;
  emOrdem(bt, v); // Preencher o vetor e limpar arvore
  // Criar arvore balanceada e retorna-la
  aux = buildBalancedTree(v, low, high);
  freeBinTree(bt);
  return aux;
}

// Funcao para criar o indice na memoria em forma de arvore binaria
void getR() {
  int i = 1;
  registro a;

  fseek(arquivo, 0, SEEK_END);
  if (ftell(arquivo) == 0)
    return;
  rewind(arquivo);
  // Ler todos os registros do arquivo e inseri-los na arv, que é um TreeNode
  // global
  while (fread(&a, sizer, 1, arquivo) == 1) {
    arv = insertBinTree(a.nome, arv, i);
    i++;
    if (feof(arquivo))
      return;
  }
}

//------------------------------------------------------------------------------
// Funcao para inserir um registro de dados no arquivo
//------------------------------------------------------------------------------
int insereRegistro() {
  registro a;
  a.apagado = 0;
  fseek(arquivo, 0, SEEK_END);
  int n = ftell(arquivo) / sizer;
  system("clear");
  printf("\tInserção de registro\n\n");
  printf("Nome: ");
  getchar();

  // Nao ser possivel inserir um registro sem nome
  while (fgets(a.nome, TAM_NOME, stdin) && !strcmp(a.nome, "\n"))
    ;
  printf("Endereço: ");
  fgets(a.endereco, TAM_END, stdin);
  printf("Telefone: ");
  fgets(a.telefone, TAM_FONE, stdin);
  fwrite(&a, sizeof(registro), 1, arquivo);
  // Inserir na arvore, com  posicao = tamanho + 1
  arv = insertBinTree(a.nome, arv, n + 1);

  printf("\n\tRegistro inserido! Tecle [ENTER]\n");
  getchar();
  return 0;
}

//--------------------------------------------------------------
// Funcao para listar os registros no arquivo
//--------------------------------------------------------------
void listaRegistros(TreeNode *bt) {
  static int first = 0;
  registro a;
  int n;

  if (bt == NULL)
    return;
  if (!first) {
    getchar();
    first = 1;
    system("clear");
  }
  fseek(arquivo, 0, SEEK_END);
  n = ftell(arquivo) / sizeof(registro); // calcula o número de registros
  // Lista os registros em Ordem
  listaRegistros(bt->left);
  fseek(arquivo, sizer * (bt->info.pos - 1), 0);
  fread(&a, sizeof(registro), 1, arquivo);
  if (a.apagado == 0) {
    printf("Registro: %lu/%d\n", ftell(arquivo) / sizeof(registro), n);
    printf("Nome: %s\nEndereço: %s\nTelefone: %s\n", a.nome, a.endereco,
           a.telefone);
    printf("\nTecle [ENTER] para o próximo registro\n");
    getchar(); // captura a tecla [ENTER]
  }
  listaRegistros(bt->right);
}

//--------------------------------------------------------------------
// Funcao para buscar um registro no arquivo
//--------------------------------------------------------------------
int buscaRegistro() {
  registro a;
  char chave[TAM_NOME];
  TreeNode *aux = arv;

  getchar(); // limpa o buffer de teclado
  system("clear");
  printf("\tBusca por nome\n");
  printf("Buscar por: ");
  scanf("%s[a-z, A-Z, "
        "]",
        chave);
  getchar(); // limpa o buffer de teclado
  // Busca uma correspondecia na arvore e entao opera no arquivo
  while (aux) { //strcmp = 10 quando a diferenca entre as strings é um \n
    if (!strcmp(aux->info.key, chave) || strcmp(aux->info.key, chave) == 10) {
      fseek(arquivo, sizer * (aux->info.pos - 1), 0);
      fread(&a, sizeof(registro), 1, arquivo);
      printf("Endereço: %s\n", a.endereco);
      printf("Telefone: %s\n", a.telefone);
      printf("Tecle [ENTER]");
      getchar(); // captura a tecla [ENTER]
      return 0;
    } else {
      if (strcmp(aux->info.key, chave) > 0)
        aux = aux->left;
      else
        aux = aux->right;
    }
  }
  printf("Nome não encontrado.\n"); // não achou
  printf("Tecle [ENTER]");
  getchar(); // captura a tecla [ENTER]
  return 1;
}

//----------------------------------------
// Altera um registro de dados
//----------------------------------------
int alteraRegistro() {
  registro a;
  char chave[TAM_NOME];
  int n;
  TreeNode *aux = arv;
  getchar(); // limpa o buffer de teclado
  rewind(arquivo);
  system("clear");
  printf("\tAlteração de dados\n");
  printf("Buscar por: ");
  scanf("%s[a-z, A-Z, "
        "]",
        chave);
  getchar(); // limpa o buffer de teclado
  // Busca uma correspondecia na arvore e entao opera no arquivo
  while (aux) {
    if (!strcmp(aux->info.key, chave) ||
        strcmp(aux->info.key, chave) == 10) // achou
    {
      fseek(arquivo, sizer * (aux->info.pos - 1), 0); //Procura no arquivo na posicao correspondente
      fread(&a, sizeof(registro), 1, arquivo);
      printf("Endereço: %s\n", a.endereco);
      printf("Telefone: %s\n", a.telefone);
      n = ftell(arquivo) / sizeof(registro);
      arv = delTree(arv, a); // Deleta da arvore
      printf("\nAlterar dados:\n");
      printf("Novo Nome: ");
      while (fgets(a.nome, TAM_NOME, stdin) && !strcmp(a.nome, "\n"))
        ;
      printf("Novo Endereço: ");
      scanf("%s[A-Z, a-z, 0-9, "
            "]",
            a.endereco);
      printf("Novo Telefone: ");
      scanf("%s[0-9]", a.telefone);
      fseek(arquivo, -sizeof(registro), 1); //substituir no arquivo
      fwrite(&a, sizeof(registro), 1, arquivo);
      // Reinsere na arvore no lugar certo
      arv = insertBinTree(a.nome, arv, n);
      printf("Tecle [ENTER]");
      getchar(); // captura a tecla [ENTER]
      return 0;
    } else {
      if (strcmp(aux->info.key, chave) > 0)
        aux = aux->left;
      else
        aux = aux->right;
    }
  }
  printf("Nome não encontrado.\n"); // não achou
  printf("Tecle [ENTER]");
  getchar(); // captura a tecla [ENTER]
  return 1;
}

//---------------------------------
// Função para apagar um registro
//---------------------------------
int apagarRegistro() {
  registro a;
  char chave[TAM_NOME];
  TreeNode *aux = NULL;
  aux = arv;

  getchar(); // limpa o buffer de teclado
  rewind(arquivo);
  system("clear");
  printf("\t Apagar dados\n");
  printf("Buscar por: ");
  scanf("%s[a-z, A-Z, "
        "]",
        chave);
  getchar(); // limpa o buffer de teclado
  // Busca uma correspondecia na arvore e entao opera no arquivo
  while (aux) {
    if (!strcmp(aux->info.key, chave) || strcmp(aux->info.key, chave) == 10) {
      fseek(arquivo, sizer * (aux->info.pos - 1), 0);
      fread(&a, sizeof(registro), 1, arquivo);
      a.apagado = 1; // Diz que o elemento foi apagado
      fseek(arquivo, -sizer, 1);
      fwrite(&a, sizer, 1, arquivo);
      printf("Apagado com sucesso\n");
      printf("Tecle [ENTER]");
      getchar(); // captura a tecla [ENTER]
      return 0;
    } else {
      if (strcmp(aux->info.key, chave) > 0)
        aux = aux->left;
      else
        aux = aux->right;
    }
  }
  printf("Nome não encontrado.\n"); // não achou
  printf("Tecle [ENTER]");
  getchar(); // captura a tecla [ENTER]
  return 1;
}

//Funcao para alterar o campo apagado do registro
int recuperarRegistro() {
  registro a;
  char chave[TAM_NOME];
  TreeNode *aux = NULL;
  aux = arv;

  getchar(); // limpa o buffer de teclado
  rewind(arquivo);
  system("clear");
  printf("\t Recuperar dados\n");
  printf("Buscar por: ");
  scanf("%s[a-z, A-Z, "
        "]",
        chave);
  getchar(); // limpa o buffer de teclado
  // Busca uma correspondecia na arvore e entao opera no arquivo
  while (aux) {
    if (!strcmp(aux->info.key, chave) || strcmp(aux->info.key, chave) == 10) {
      fseek(arquivo, sizer * (aux->info.pos - 1), 0);
      fread(&a, sizeof(registro), 1, arquivo);
      a.apagado = 0; // Tira a flag de apagado
      fseek(arquivo, -sizer, 1);
      fwrite(&a, sizer, 1, arquivo);
      printf("Recuperado com sucesso\n");
      printf("Tecle [ENTER]");
      getchar(); // captura a tecla [ENTER]
      return 0;
    } else {
      if (strcmp(aux->info.key, chave) > 0)
        aux = aux->left;
      else
        aux = aux->right;
    }
  }
  printf("Nome não encontrado.\n"); // não achou
  printf("Tecle [ENTER]");
  getchar(); // captura a tecla [ENTER]
  return 1;
}

//Funcao para listar registros apagados
void listaLixeira(TreeNode *bt) {
  static int first = 0;
  registro a;
  int n;

  if (bt == NULL)
    return;
  if (!first) {
    getchar();
    first = 1;
    system("clear");
  }
  fseek(arquivo, 0, SEEK_END);
  n = ftell(arquivo) / sizeof(registro); // calcula o número de registros

  // Lista a lixeira em ordem
  listaLixeira(bt->left);
  fseek(arquivo, sizer * (bt->info.pos - 1), 0);
  fread(&a, sizeof(registro), 1, arquivo);
  if (a.apagado == 1) {
    printf("Registro: %lu/%d\n", ftell(arquivo) / sizeof(registro), n);
    printf("Nome: %s\nEndereço: %s\nTelefone: %s\n", a.nome, a.endereco,
           a.telefone);
    printf("\nTecle [ENTER] para o próximo registro\n");
    getchar(); // captura a tecla [ENTER]
  }
  listaLixeira(bt->right);
}

//Colocar em ordem no novo arquivo
void delBin(FILE *n, TreeNode *bt) {
  registro a;
  if (bt == NULL)
    return;

  delBin(n, bt->left);
  fseek(arquivo, sizer * (bt->info.pos - 1), 0);
  fread(&a, sizer, 1, arquivo);
  if (a.apagado == 0) {
    fwrite(&a, sizer, 1, n);
  }
  delBin(n, bt->right);
}
// Cria um novo arquivo, copiando todos que nao foram apagados em um novo
// arquivo
int apagarLixeira() {
  TreeNode *aux = NULL;
  FILE *novo;
  registro a;
  novo = fopen("txt.dat", "wb+");
  if (!novo) {
    printf("Erro ao abrir arquivo\n");
    return 1;
  }
  rewind(arquivo);
  delBin(novo, arv);
  //Fecha os 2 arquivos e renomeia
  fclose(arquivo);
  remove("dados.dat");
  fclose(novo);
  rename("txt.dat", "dados.dat");

  if (!(arquivo = fopen("dados.dat", "rb+"))) {
    printf("\nErro: não foi possível abrir o arquivo de dados!\n");
    printf("\nCriando novo arquivo de dados...");
    arquivo = fopen("dados.dat", "wb+");
  }
  printf("Lixeira limpa com sucesso\nTecle [ENTER]\n");
  getchar();
  getchar();
  return 0;
}

// Funcao para percorrer a arvore em ordem, apos terminar o programa e deixar a agenda em ordem alfabetica a partir de um novo arquivo
void endArchive(TreeNode *bt, FILE *novo) {
  registro a;
  if (bt == NULL)
    return;

  endArchive(bt->left, novo);
  fseek(arquivo, sizer * (bt->info.pos - 1), 0);
  fread(&a, sizer, 1, arquivo);
  fwrite(&a, sizer, 1, novo);
  endArchive(bt->right, novo);
}

int fecharArquivo() {
  FILE *novo;
  registro a;
  novo = fopen("txt.dat", "wb+");

  if (!(novo = fopen("txt.dat", "wb+"))) {
    printf("Erro ao abrir arquivo\n");
    return 1;
  }
  endArchive(arv, novo); // Preenche o novo arquivo
  // Fecha ambos os arquivos
  fclose(arquivo);
  remove("dados.dat");
  fclose(novo);
  rename("txt.dat", "dados.dat");

  if (!(arquivo = fopen("dados.dat", "rb+"))) {
    printf("\nErro: não foi possível abrir o arquivo de dados!\n");
    printf("\nCriando novo arquivo de dados...");
    arquivo = fopen("dados.dat", "wb+");
  }
  return 0;
}
//-------------------------
// Funcao principal
//-------------------------

int main() {
  int opcao = -1, n;
  int (*pf[6])();

  pf[0] = insereRegistro;
  pf[2] = buscaRegistro;
  pf[3] = alteraRegistro;
  pf[4] = apagarRegistro;
  pf[5] = recuperarRegistro;

  if (!(arquivo = fopen("dados.dat", "rb+"))) {
    printf("\nErro: não foi possível abrir o arquivo de dados!\n");
    printf("\nCriando novo arquivo de dados...");
    arquivo = fopen("dados.dat", "wb+");
  }
  getR(); //Criar a arvore a partir do arquivo
  arv = balancedTree(arv, 0, numNodes(arv) - 1); //balancear a arvore

  while (opcao) {
    system("clear");
    printf("\tAgenda\n\n");
    printf("Menu principal\n\n");
    printf("\t[0] Sair\n");
    printf("\t[1] Inserir registro\n");
    printf("\t[2] Listar registros\n");
    printf("\t[3] Buscar registro\n");
    printf("\t[4] Alterar registro\n");
    printf("\t[5] Apagar Registro\n");
    printf("\t[6] Recuperar Registro\n");
    printf("\t[7] Listar lixeira\n");
    printf("\t[8] Esvaziar lixeira\n");
    printf("\nQual a sua opção? ");
    scanf("%d", &opcao);
    if ((opcao > 0) && (opcao <= 6) && opcao != 2) {
      (*pf[opcao - 1])();
      getchar();
    } else if (opcao == 2) {
      getchar();
      listaRegistros(arv);
    } else if (opcao == 7) {
      getchar();
      listaLixeira(arv);
    } else if (opcao == 8) {
      getchar();
      apagarLixeira();
      freeBinTree(arv); // Arrumar info.pos, que causa erros depois
      arv = NULL;
      getR(); //refazer a arvore
    }
  }
  fecharArquivo(); //Arrumar o registro em ordem alfabetica
  return 0;
}

