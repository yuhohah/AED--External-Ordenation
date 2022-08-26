// Aluno: Luan de Paula Mota
// Matrícula: 202003588

// Funcionalidades:
// Usuário: Criação, alteração, remoção, visualização da Notes.
// Administrador: Visualizar todas as Notes em ordem sequencial, Ordenar as
// Notes na memória externa

// Considerações:
// Para visualizar todas as Notes digite um nome de usuário qualquer que não
// possua Note, e então digite 88 Para Ordenar as Notes na memória externa um
// nome de usuário qualquer que não possua Note, e então digite 999

/////////////////////////////////////////////////////
// Bibliotecas
/////////////////////////////////////////////////////
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/////////////////////////////////////////////////////
// Tamanhos estaticos das Variaveis
/////////////////////////////////////////////////////
#define MAX_USER_LENGTH 11
#define MAX_NOTE_TITLE_LENGTH 100
#define MAX_NOTE_BODY_LENGTH 5000
#define MAX_titleS_PER_NOTE 5
#define MAX_title_BODY_LENGTH 50

// Quantidade máxima de notas que cabem na memória, pode ser alterado
#define AMOUNT_NOTE 8

/////////////////////////////////////////////////////
// Define para limpar a tela no Linux e no Windows
/////////////////////////////////////////////////////
#ifdef __WIN32
#define limpar_tela "cls"
#else
#define limpar_tela "clear"
#endif

/////////////////////////////////////////////////////
// Estrutura da Note
/////////////////////////////////////////////////////
typedef struct {
  char user[MAX_USER_LENGTH];
  char title[MAX_NOTE_TITLE_LENGTH];
  bool checkbox;
  time_t due_date;
  time_t creation_date;
  time_t exclusion_date;
  int priority;
  char body[MAX_NOTE_BODY_LENGTH];
} note;

// Estrutura para link interno entre arquivos complementar da ordenação externa
// Multi-Way merging
typedef struct {
  FILE *f;
  int pos, max;
  note *notes_buffer;
} sort_notes;

/////////////////////////////////////////////////////
// Variaveis Globais declaradas
/////////////////////////////////////////////////////
char *priority_names[3] = {"Baixa", "Média", "Alta"};
FILE *data_file;
char user[11], data_file_name[30] = "Banco de Dados Note.txt";

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Declaração das Funções
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Função onde a maioria das funcionalidades podem ser acessadas e onde imprimi
// as Notes do usuário
int interface();

// Função Create para criar uma Note e guardar na memoria externa/HDD
void create();

// Função Update que atualiza uma Note (Concluída/Alterar/Excluir) e
// sobreescreve a antiga na memoria externa
int update(note *note_update, const int *pos);

// Função para imprimir uma Note na tela/Modo de apresentar uma Note
void note_print(note *n, int controler);

// Função secreta para o Administrador ver todas as Notes/Confirmar se estão em
// ordem
void print_all_notes();

// Função para ordenação externa com base na string do usuário
void bubble_sort(note *notes, int length);

// Função recursiva de ordenação interna com base na data de criação
void quick_sort(note *note, int right, int left);

// Função complementar para o quick sort
int partition(note *note, int right, int left);

// Função para troca de lugar das Notes na memoria interna - Utilizada no quick
// sort e no bubble
void swap_note(note *note_initial, note *note_final);

// Ciar arquivos ordenados apartir da data_file
int create_arq_sort();

// Salvar os arquivos
void save_arq(note *blocos, char *arq_name, int amount);

// Funcao Principal para Ordenar
void merging();

// Le os dados nos arquivos temporarios e escreve no data_file
void merge(int arq_count, int K);

// Enche o buffer na struct linkada de cada arquivo
void slot_buffer(sort_notes *arq, int K);

// Procurar a menor Note dentro da struct dos arquivos em seus buffers
int search_small(sort_notes *arq, int arq_count, int K, note *small);

// Função MAIN para receber o usuário que vai utilizar o NoteBlocks e guardar na
// memoria interna
int main() {

  data_file = fopen(data_file_name, "ab");
  data_file = fopen(data_file_name, "r+");

  system(limpar_tela);

  printf(
      "\n******************************************************************\n");
  printf(
      "**************** Bem Vindo(a) ao Note Block! *********************\n");
  printf(
      "******************************************************************\n");

  printf(
      "\nEntre com seu user de usuário(máximo 10 caracteres e sem espaços):\n");
  scanf("%s", user);

  system(limpar_tela);

  interface();

  fclose(data_file);

  return (printf("Obrigado por usar o NOTEBLOCKS!\n"));
}

void create() {
  note new_note;
  int choice;

  strcpy(new_note.user, user);

  getchar();
  printf("Atribua um Titulo(max: 30 caracteres): ");
  scanf("%[^\n]", new_note.title);

  new_note.checkbox = false;

  time_t now;
  time(&now);
  new_note.creation_date = now;
  new_note.due_date = now;
  new_note.exclusion_date = -1;

  printf("Escolha um nivel de prioridade para a "
         "Note:\n1-Baixa\n2-Media\n3-Alta\n");
  scanf("%d", &choice);

  switch (choice) {
  case 1:
    new_note.priority = 1;
    break;
  case 2:
    new_note.priority = 2;
    break;
  case 3:
    new_note.priority = 3;
    break;
  default:
    printf("Não há essa alternativa, a prioridade foi definida para Baixa.");
    new_note.priority = 1;
    break;
  }

  printf("Insira sua Note:\n");
  getchar();
  scanf("%[^\n]", new_note.body);

  fseek(data_file, 0, SEEK_END);
  fwrite(&new_note, sizeof(note), 1, data_file);

  system(limpar_tela);

  interface();
}

int update(note *note_update, const int *pos) {
  char null_note[5] = "NULL";
  int choice_input, choice;
  time_t now;

  choice_input = 0;
  do {
    printf("O que deseja fazer com a Note?\n1- Marcar como concluída.\n2- "
           "Editar\n3- Remover\n");
    scanf("%d", &choice_input);
  } while (!choice_input || choice_input < 1 || choice_input > 3);

  switch (choice_input) {
  case 1:
    note_update->checkbox = 1;

    time(&now);
    note_update->due_date = now;

    fseek(data_file, *pos, SEEK_SET);
    fwrite(&(note_update), sizeof(data_file), 1, data_file);
    break;
  case 2:
    note_update->checkbox = false;

    printf("Digite o Titulo(max: 30 caracteres): ");
    scanf("%[^\n]", note_update->title);

    time(&now);
    note_update->due_date = now;
    note_update->exclusion_date = -1;

    printf("Escolha um nivel de prioridade para a "
           "Note:\n1-Baixa\n2-Media\n3-Alta");
    scanf("%d", &choice);

    switch (choice) {
    case 1:
      note_update->priority = 1;
      break;
    case 2:
      note_update->priority = 2;
      break;
    case 3:
      note_update->priority = 3;
      break;
    default:
      printf("Não há essa alternativa a prioridade foi definida para Baixa.");
      note_update->priority = 1;
      break;
    }

    printf("Insira sua Note(max: 5000 caracteres):\n");
    getchar();
    scanf("%[^\n]", note_update->body);

    fseek(data_file, *pos, SEEK_SET);
    fwrite(&(note_update), sizeof(data_file), 1, data_file);
    break;

  case 3:
    strcpy(note_update->user, null_note);

    time(&now);
    note_update->exclusion_date = now;

    fseek(data_file, *pos, SEEK_SET);
    fwrite(&(note_update), sizeof(data_file), 1, data_file);
    break;
  default:
    break;
  }

  return 0;
}

int interface() {
  int count = 0;
  int position[30], input_choice;

  note n[30];

  // Ler o arquivo e encontrar as Notes do Usuário, como há uma função de
  // ordenação externa, ele para quando ler mais de uma Note do usuário e na
  // seguinte não ser dele.
  fseek(data_file, 0, SEEK_SET);
  while (!feof(data_file)) {
    position[count] = ftell(data_file);
    fread(&(n[count]), sizeof(note), 1, data_file);
    if (strcmp(n[count].user, user) == 0) {
      count++;
    } else if (count > 0 && strcmp(n[count].user, user) != 0) {
      break;
    }
  }

  // Modo de apresentação para usuários com Note
  if (count > 0) {
    printf("\n*****************************************************************"
           "*\n");
    printf(
        "**************************** Suas Notes **************************\n");
    printf(
        "******************************************************************\n");

    // Ordenarção interna por data de criação
    quick_sort(n, count, 0);

    // Print de todas as Notes do Usuário
    for (int i = 0; i < count; i++) {
      note_print(&n[i], 0);
    }

    // Menu para Alterar/Criar/Sair/Fechar
    printf("\n*****************************************************************"
           "*\n");
    printf("\nSelecione o que deseja fazer:\n");
    printf("1- Selecionar uma Note para atualizar ou remover\n2- Criar uma "
           "Note\n3- Logout\n4- Fechar o Programa\n");
    scanf("%d", &input_choice);

    switch (input_choice) {
    case 1:
      printf("Digite o número da Note que deseja escolher:\n");
      scanf("%d", &count);
      system(limpar_tela);
      update(&n[count], &position[count]);
      break;
    case 2:
      system(limpar_tela);
      create();
      break;
    case 3:
      system(limpar_tela);
      return main();
    default:
      return 0;
    }
  }
  // Menu para o usuário que não possuir uma Note
  else {
    printf("Esse usuário não possui uma Note!\n");
    printf("\nDeseja criar uma?\n1-Sim\n2-Não\n");
    scanf("%d", &input_choice);

    switch (input_choice) {
    case 1:
      system(limpar_tela);
      create();
      break;
    case 2:
      system(limpar_tela);
      main();
      break;
    case 88:
      system(limpar_tela);
      print_all_notes();
      break;
    case 999:
      system(limpar_tela);
      fseek(data_file, 0, SEEK_SET);
      merging();
      break;
    default:
      break;
    }
  }

  return 0;
}

void print_all_notes() {
  note n;

  // Ler todas as Notes
  fseek(data_file, 0, SEEK_SET);
  while (fread(&(n), sizeof(note), 1, data_file)) {

    // Printar as Notes
    note_print(&n, 1);
    if (feof(data_file)) {
      break;
    }
  }
}

int partition(note *note, int right, int left) {
  time_t pivot = note[left].creation_date;
  int i = left;

  for (int j = left + 1; j <= right; j++) {
    if (note[j].creation_date > pivot) {
      i += 1;
      swap_note(&note[i], &note[j]);
    }
  }

  swap_note(&note[left], &note[i]);

  return i;
}

void quick_sort(note *note, int right, int left) {
  if (left < right) {
    int pivot = partition(note, right, left);
    quick_sort(note, pivot - 1, left);
    quick_sort(note, right, pivot + 1);
  }
}

void swap_note(note *note_initial, note *note_final) {
  note note_copy;

  // note_copy recebe a note_initial

  strcpy(note_copy.user, note_initial->user);
  strcpy(note_copy.title, note_initial->title);
  strcpy(note_copy.body, note_initial->body);
  note_copy.checkbox = note_initial->checkbox;
  note_copy.creation_date = note_initial->creation_date;
  note_copy.due_date = note_initial->due_date;
  note_copy.exclusion_date = note_initial->exclusion_date;
  note_copy.priority = note_initial->priority;

  // note_nitial recebe a note_final

  strcpy(note_initial->user, note_final->user);
  strcpy(note_initial->title, note_final->title);
  strcpy(note_initial->body, note_final->body);
  note_initial->checkbox = note_final->checkbox;
  note_initial->creation_date = note_final->creation_date;
  note_initial->due_date = note_final->due_date;
  note_initial->exclusion_date = note_final->exclusion_date;
  note_initial->priority = note_final->priority;

  // note_final recebe a note_opy

  strcpy(note_final->user, note_copy.user);
  strcpy(note_final->title, note_copy.title);
  strcpy(note_final->body, note_copy.body);
  note_final->checkbox = note_copy.checkbox;
  note_final->creation_date = note_copy.creation_date;
  note_final->due_date = note_copy.due_date;
  note_final->exclusion_date = note_copy.exclusion_date;
  note_final->priority = note_copy.priority;
}

void bubble_sort(note *notes, int length) {
  int j, k;

  for (j = 0; j < length; j++) {
    for (k = 0; k < length; k++) {
      // Compara os nomes
      if (strcmp(notes[k].user, notes[j].user) < 0) {
        swap_note(&notes[k], &notes[j]);
      }
    }
  }
}

void note_print(note *n, int controler) {
  struct tm *date;

  if (controler == 1) {
    printf("\nUsuário: %s\n", n->user);
  }

  printf("\nTitulo: %s\n\n", n->title);

  date = localtime(&(n->creation_date));

  printf("Criação da Note: ");
  printf("%d/%d/%d ", date->tm_mday, date->tm_mon + 1, date->tm_year + 1900);
  printf("- %d:%d:%d\n", date->tm_hour, date->tm_min, date->tm_sec);

  date = localtime(&(n->due_date));

  printf("Data da ultima alteração da Note: ");
  printf("%d/%d/%d ", date->tm_mday, date->tm_mon + 1, date->tm_year + 1900);
  printf("- %d:%d:%d\n", date->tm_hour, date->tm_min, date->tm_sec);

  if (controler == 1) {

    date = localtime(&(n->exclusion_date));

    printf("Data da exclusão da Note: ");
    printf("%d/%d/%d ", date->tm_mday, date->tm_mon + 1, date->tm_year + 1900);
    printf("- %d:%d:%d\n", date->tm_hour, date->tm_min, date->tm_sec);
  }

  if (n->checkbox == false) {
    printf("\nNão Concluído!\n");
  } else {
    printf("\nConcluído!\n");
  }

  printf("Prioridade: %s\n", priority_names[n->priority - 1]);

  printf("################################-Note-############################\n%"
         "s\n",
         n->body);
  printf(
      "\n##################################################################");

  printf(
      "\n------------------------------------------------------------------\n");
}

int create_arq_sort() {
  note Notes_sort[AMOUNT_NOTE];
  int count = 0, amount = 0;
  char arq_temp_name[20];

  // Percorer a DATA FILE lendo as Notes e colocando na memoria interna
  while (fread(&(Notes_sort[amount]), sizeof(note), 1, data_file)) {

    /*if(Notes_sort[amount-1].creation_date == Notes_sort[amount].creation_date
    && Notes_sort[amount-1].due_date == Notes_sort[amount].due_date){ break;
    }*/
    amount++;

    // Chegou na capacidade maxima da memoria interna
    if (amount == AMOUNT_NOTE) {
      count++;
      // Nome do arquivo temporario
      sprintf(arq_temp_name, "Temp%d.txt", count);

      // Ordenacao na memoria interna
      bubble_sort(Notes_sort, AMOUNT_NOTE);
      // quick_sort(Notes_sort, AMOUNT_NOTE, 0);

      // Escrita no arquivo temporario
      save_arq(Notes_sort, arq_temp_name, amount);

      // fwrite(&(Notes_sort[amount]), sizeof(note), 1, p);
      amount = 0;
    }
  }

  // Se chegou ao final do arquivo e a quantidade for menor q a capacidade
  if (amount > 0) {
    count++;
    sprintf(arq_temp_name, "Temp%d.txt", count);

    // Ordena
    bubble_sort(Notes_sort, amount);

    // Cria novo arquivo e escreve
    save_arq(Notes_sort, arq_temp_name, amount);
  }

  // Retorna quantos arquivos ordenados foram criados
  return (count);
}

void save_arq(note *blocos, char *arq_name, int amount) {
  int count;

  // Abre o arquivo
  FILE *f = fopen(arq_name, "a+");

  // Escreve os dados nele
  for (count = 0; count < amount; count++) {
    fwrite(&(blocos[count]), sizeof(note), 1, f);
  }

  // Fecha o arquivo
  fclose(f);
}

void merging() {
  char arq_temp_name[20];
  int num_arq = create_arq_sort();
  int i, k = AMOUNT_NOTE / (num_arq + 1);

  // fclose(data_file);
  remove(data_file_name);
  // data_file = fopen (data_file_name, "w+");
  merge(num_arq, k);

  // Deletar os arquivos temporarios
  for (i = 0; i < num_arq; i++) {
    sprintf(arq_temp_name, "Temp%d.txt", i + 1);
    remove(arq_temp_name);
  }
}

void merge(int arq_count, int K) {
  char arq_temp_name[20];
  int count, qnt_buffer = 0;
  note small_note;

  // Aloca o tanto que cabe na memoria interna de Notes
  note *note_buffer = (note *)malloc(K * sizeof(note));

  sort_notes *arq;

  // Aloca a quantidade de arquivos criados para sua administracao
  arq = (sort_notes *)malloc(arq_count * sizeof(sort_notes));

  // Insere os dados de cada arquivo em uma struct
  for (count = 0; count < arq_count; count++) {
    sprintf(arq_temp_name, "Temp%d.txt", count + 1);
    arq[count].f = fopen(arq_temp_name, "r");
    arq[count].max = 0;
    arq[count].pos = 0;
    arq[count].notes_buffer = (note *)malloc(K * sizeof(note));
    slot_buffer(&arq[count], K);
  }

  // Adicionar o menor
  while (search_small(arq, arq_count, K, &small_note) == 1) {

    // Buffer recebe a menor
    note_buffer[qnt_buffer] = small_note;
    qnt_buffer++;

    // Ao encher o buffer salva os dados na data_file
    if (qnt_buffer == K) {
      save_arq(note_buffer, data_file_name, K);
      qnt_buffer = 0;
    }
  }

  // Se ainda tiver dados no Buffer salva no data_file
  if (qnt_buffer != 0) {
    save_arq(note_buffer, data_file_name, qnt_buffer);
  }

  // Limpa a alocacao no buffer dentro da struct, dos arquivos e do buffer
  for (count = 0; count < arq_count; count++) {
    free(arq[count].notes_buffer);
  }

  free(arq);
  free(note_buffer);
}

int search_small(sort_notes *arq, int arq_count, int K, note *small) {
  int count, index = -1;

  // Procurar menor valor na primeira posicao de cada buffer
  for (count = 0; count < arq_count; count++) {
    if (arq[count].pos < arq[count].max) {
      if (index == -1) {
        index = count;
      } else {
        // Verifica se é menor
        if (strcmp(arq[count].notes_buffer[arq[count].pos].user,
                   arq[index].notes_buffer[arq[index].pos].user) > 0) {
          index = count;
        }
      }
    }
  }

  if (index != -1) {

    *small = arq[index].notes_buffer[arq[index].pos];
    // swap_note(small, &arq[index].notes_buffer[arq[index].pos]);

    arq[index].pos++;
    if (arq[index].pos == arq[index].max) {
      slot_buffer(&arq[index], K);
    }
    return 1;
  } else {
    return 0;
  }
}

void slot_buffer(sort_notes *arq, int K) {
  int count;

  // Se nao houver arquivo retorna
  if (arq->f == NULL) {
    return;
  }

  arq->pos = 0;
  arq->max = 0;

  for (count = 0; count < K; count++) {
    // Ler no arquivo e escrever na memoria interna a note para depois verificar
    // a menor
    if (fread(&(arq->notes_buffer[arq->max]), sizeof(note), 1, arq->f)) {
      arq->max++;
    }
    // Fim o arquivo fechar ele
    else {
      fclose(arq->f);
      arq->f = NULL;
      break;
    }
  }
}