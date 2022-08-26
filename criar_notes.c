#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_USER_LENGTH 11
#define MAX_NOTE_TITLE_LENGTH 100
#define MAX_NOTE_BODY_LENGTH 5000
#define MAX_titleS_PER_NOTE 5
#define MAX_title_BODY_LENGTH 50


//Estrutura da Note
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


char *priority_names[3] = {"Baixo", "Médio", "Alto"};
FILE *data_file;
char user[11], data_file_name[30] = "Banco de Dados Note.txt";
int Quantidade = 0;



void create();




int main(){

    data_file = fopen("Banco de Dados Note.txt", "ab");
    data_file = fopen("Banco de Dados Note.txt", "r+b");


    printf("\n******************************************************************\n");
    printf("**************** Bem Vindo(a) ao Note Block! *********************\n");
    printf("******************************************************************\n");

    printf("Entre com seu nome de usuário(máximo 10 caracteres e sem espaços):\n");
    scanf("%s", user);

    printf("Quantidade de notes:");
    scanf("%d", &Quantidade);


    create();

    return 0;


}

void create(){
    note new_note;
    time_t now;
    int count = 1;

    while(count <= Quantidade){

        strcpy(new_note.user, user);


        sprintf(new_note.title, "Note %d", count);

        new_note.checkbox = false;

        
        time(&now);
        new_note.creation_date = now;
        new_note.due_date = now;
        new_note.exclusion_date = -1;

        srand((unsigned)time(NULL));
        new_note.priority = 1 + (rand() % 3);

        sprintf(new_note.body, "Note %d", count);

        fseek(data_file, 0, SEEK_END);
        fwrite(&new_note, sizeof(note), 1, data_file);

        count ++;
    }

    printf("Foram criadas %d Notes de %s\n", count - 1, user);
}