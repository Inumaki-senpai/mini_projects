#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mariadb/mysql.h>


//  GLOBAL VARIABLES
#define SERVER "localhost"
#define USER "meow"
#define PASSWORD "MeowNagar.."
#define DATABASE "mini_projects"


//  FUNCTIONS

int delay(unsigned int seconds) {
    struct timespec req = {0};
    req.tv_sec = seconds;
    nanosleep(&req, NULL);
}

void finish_with_error(MYSQL *conn){
    fprintf(stderr, "\033[1;31m%s\n", mysql_error(conn));
    mysql_close(conn);
    delay(1);
    exit(1);
}

int create(MYSQL *conn) {
    char name[30] = {'\0'}, number[20] = {'\0'}, query[100] = {'\0'};
    
    delay(1);
    system("clear");
    
    //  RETRIEVING CONTACT INFORMATION
    printf("\033[33m`````\tNew Contact\t`````\n\033[0;1m");
    printf("Name: ");
    scanf(" %[^\n]s", name);
    printf("Contact no: ");
    scanf(" %[^\n]", number);

    //  QUERY SETUP AND EXECUTION
    sprintf(query, "INSERT INTO contacts(name, phone) VALUES('%s','%s')", name, number);
    if(mysql_query(conn, query)) {
        finish_with_error(conn);
    }
    else {
        printf("\033[1;32mContact created successfully..\n");
    }

    return 0;
}

int delete(MYSQL *conn) {
    char choice = '\0', query[100] = {'\0'};

    system("clear");

    printf("\033[33m`````\tDelete Contact\t`````\n\033[0;1m");
    printf("1. By name\n");
    printf("2. By number\n");
    printf("3. Delete all\n");
    printf("Enter selection number: ");
    scanf(" %c", &choice);

    //  QUERY SETUP TO SEARCH ALL SUCH CONTACTS   -> QUERY1
    switch (choice){
    case '1':
        char name[30] = {'\0'};
        printf("Enter Name: ");
        scanf(" %[^\n]s", name);
        sprintf(query, "SELECT * FROM contacts WHERE name Like '%s\%' ORDER BY name ASC", name);
        break;

    case '2':
        char number[20] = {'\0'};
        printf("Enter Number: ");
        scanf(" %[^\n]s", number);
        sprintf(query, "SELECT * FROM contacts WHERE phone LIKE '%s\%' ORDER BY name ASC", number);
        break;

    case '3':
        sprintf(query,"DELETE FROM contacts");
        if (mysql_query(conn, query)) {
            finish_with_error(conn);
        }
        else {
            printf("\033[1;32mContacts deleted successfully..\n");
        }
        return 0;
        break;
    
    default:
        printf("\033[1;31mIncorrect selection!\n");
        break;
    }

    //  QUERY1 EXECUTION AND DISPLAYING CONTACTS
    if(mysql_query(conn, query)) {
        finish_with_error(conn);
    }
    else {
        MYSQL_RES *result = mysql_store_result(conn);
        if (result == NULL) {
            finish_with_error(conn);
        }
        MYSQL_ROW row;
        for (int i = 0; i < mysql_num_rows(result); i++) {
            row = mysql_fetch_row(result);
            printf("\033[1;35m Name: \033[0;1m%s\n", row[0]);
            printf("\033[1;35m Contact: \033[0;1m%s\n\n", row[1]);
        }

        //  CONFIRMING DELETION OF CONTACT  -> QUERY2
        char number[20] = {'\0'};

        printf("\033[31mEnter number to delete: \033[0;1m");
        scanf(" %[^\n]s", number);

        sprintf(query, "DELETE FROM contacts WHERE phone='%s'", number);

        if(mysql_query(conn, query)) {
            finish_with_error(conn);
        }
        else {
            printf("\033[1;32mContact deleted successfully..\n");
        }
    }

    return 0;
}

int search(MYSQL *conn) {
    char choice = '\0', query[100] = {'\0'};

    system("clear");

    printf("\033[1;33m`````\tSearch contact!\t`````\n\033[0;1m");
    printf("1. Search by name\n");
    printf("2. Search by number\n");
    printf("Enter selection number: ");
    scanf(" %c", &choice);

    //  QUERY SETUP
    switch (choice) {
    case '1':
        char name[30] = {'\0'};
        printf("Enter name: ");
        scanf(" %[^\n]s", name);
        sprintf(query, "SELECT * FROM contacts WHERE name LIKE '%s\%' ORDER BY name ASC", name);
        break;

    case '2':
        char number[20] = {'\0'};
        printf("Enter Number: ");
        scanf(" %[^\n]s", number);
        sprintf(query, "SELECT * FROM contacts WHERE phone LIKE '%s\%' ORDER BY name ASC", number);
        break;
    
    default:
        printf("\033[1;31mIncorrect selection!\n");
        break;
    }

    //  QUERY EXECUTION
    if(mysql_query(conn, query)) {
        finish_with_error(conn);
    }

    //  EXTRACTION AND DISPLAYING MATCHING CONTACTS
    MYSQL_RES *result = mysql_store_result(conn);

    if (result == NULL) {
        finish_with_error(conn);
    }

    MYSQL_ROW row;
    for (int i = 0; i < mysql_num_rows(result); i++) {
        row = mysql_fetch_row(result);
        printf("\033[1;35m%d. Name: \033[0;1m%s\n", i+1, row[0]);
        printf("\033[1;35m  Contact: \033[0;1m%s\n\n", row[1]);
    }

    mysql_free_result(result);
    return 0;
}

int show(MYSQL *conn) {
    char *query = "SELECT * FROM contacts ORDER BY name ASC";
    
    system("clear");
    printf("\033[1;33m`````\tAll Contacts\t`````\n");

    if(mysql_query(conn, query)) {
        finish_with_error(conn);
    }

    //  CONTACTS EXTRACTION
    
    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        finish_with_error(conn);
    }

    MYSQL_ROW row;
    
    if(mysql_num_rows(result) == 0) {
        printf("\033[35m  No contacts\n");
    }

    //  FETCH AND DISPLAY ALL CONTACTS
    for (int i = 0; i < mysql_num_rows(result); i++)
    {
        row = mysql_fetch_row(result);
        printf("\033[35m%d. Name: \033[0;1m%s\n", i+1, row[0]);
        printf("\033[35m  Contact: \033[0;1m%s\n\n", row[1]);
    }

    mysql_free_result(result);
    return 0;
}


//  MAIN FUNCTION
void main() {
    //  VARIABLE DECLARATION;
    char select = '\0';

    //  DATABASE SETUP
    MYSQL *conn = mysql_init(NULL);
    if(conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        mysql_close(conn);
        delay(1);
        exit(1);
    }
    if(mysql_real_connect(conn, SERVER, USER, PASSWORD,
                            DATABASE, 0, NULL, 0) == NULL) {
        finish_with_error(conn);
    }

    //  PROGRAM EXECUTION
    main_menu:                                                          // USING LABEL main_menu
    system("clear");

    printf("\033[1;31m`````\tThe Contacts\t`````\n\033[33m");
    printf("1. Create contact\n");
    printf("2. Delete contact\n");
    printf("3. Search contact\n");
    printf("4. Show all contacts\n");
    printf("5. Exit\n\033[0;1m");

    printf("Enter selection number: ");
    scanf(" %c", &select);

    switch (select) {
    case '1':
        create(conn);
        delay(1);
        goto main_menu;
        break;

    case '2':
        delete(conn);
        delay(1);
        goto main_menu;
        break;

    case '3':
        search(conn);
        // getch();
        // goto main_menu;
        break;

    case '4':
        show(conn);
        break;

    case '5':
        printf("\033[32mByee byee!\n");
        mysql_close(conn);
        delay(1);
        exit(1);
    
    default:
        printf("\033[1;31mIncorrect selection!\n");
        goto main_menu;
        break;
    }

    mysql_close(conn);
    delay(1);
    exit(1);
}