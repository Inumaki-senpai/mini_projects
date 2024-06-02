#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mariadb/mysql.h>
#include <time.h>

//  --  GLOBAL VARIABLES  --
#define SERVER "localhost"
#define USER "meow"
#define PASSWORD "MeowNagar.."
#define DATABASE "mini_projects"

//  --  SQL ERROR HANDLING  --
void finish_with_error(MYSQL *conn) {
    fprintf(stderr, "\033[1;31m%s\n", mysql_error(conn));
    mysql_close(conn);
    exit(1);
}

//  --  Delay function in milliseconds  --
void delay(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

//  --  Function to clear the screen  --
void clear_screen() {
    printf("\033[H\033[J");
}

//  --  BILL GENERATION  --
int make_bill(MYSQL *conn, char **bill, char *query, char *select) {
    //  --  RETRIEVING ITEMS FROM DB  --
    clear_screen();
    printf("\033[31m `` Make Bill `` \n");
    printf("\033[35m-----------------\n\033[0;1m");
    sprintf(query, "SELECT * FROM items");
    
    if (mysql_query(conn, query)) {
        finish_with_error(conn);
    }

    MYSQL_RES *result = mysql_store_result(conn);

    if (result == NULL) {
        finish_with_error(conn);
    }

    MYSQL_ROW row;

    //  --  Displaying ITEMS in a Tabular Format --
    int srno_width = 5;
    int item_width = 20;
    int price_width = 10;

    printf("\033[33m  Sr No   |   Item                  |   Price\n");
    printf("-------------------------------------------------\n\033[32m");

    while ((row = mysql_fetch_row(result))) {
        printf("   %-5s  |   %-20s  |   %s\n", row[0], row[1], row[2]);
    }

    mysql_free_result(result);

    printf("\033[33mEnter 0 when selected all items to make up the bill..\033[0;1m\n");
    printf("Adding items to bill...\n");
    
    for (int i = 0; select[0] != '0'; i++) {
        printf("\033[33msrno: \033[0;1m");
        scanf(" %s", select);
        strcpy(bill[i], select);
    }

    //  --  GENERATING BILL  --
    clear_screen();
    int cost = 0;

    printf("\033[35m*******************************************\n");
    printf("\t\033[31m `` Generating Bill.. ``\n\033[0;1m");
    printf("\033[35m------------------------------------\n");
    delay(1000);

    printf("\033[33m %-20s | %10s\n", "Item", "Cost");
    printf("-------------------------------------\n");

    for (int i = 0; bill[i][0] != '\0'; i++) {
        sprintf(query, "SELECT * FROM items WHERE srno='%s'", bill[i]);
        if (mysql_query(conn, query)) {
            finish_with_error(conn);
        }
        result = mysql_store_result(conn);

        if ((row = mysql_fetch_row(result))) {
            cost += atoi(row[2]);
            printf("\033[0;1m %-20s | %10s\n", row[1], row[2]);
        }
        mysql_free_result(result);
    }

    delay(1000);
    printf("\033[35m--------------------------------------\n    \033[31mTotal Cost: %d\n", cost);
    printf("\033[35m--------------------------------------\n");

    return 0;
}

//  --  ADD ITEMS --
int add_item(MYSQL *conn, char *query) {
    char name[100] = {'\0'}, cost[10] = {'\0'}; 

    clear_screen();
    printf("\033[31m `` Add Item `` \n\033[0;1m");
    printf("\033[35m----------------\n");

    printf("\033[33m Item name: \033[0;1m");
    scanf(" %[^\n]s", name);
    printf("\033[33m Item cost: \033[0;1m");
    scanf(" %9s", cost); 

    sprintf(query, "INSERT INTO items(name, price) VALUES('%s','%s')", name, cost);
    if (mysql_query(conn, query)) {
        finish_with_error(conn);
    }

    delay(1000); 
    printf("\033[32mItem successfully added..\n");
    return 0;
}

//  --  REMOVE ITEMS  --
int remove_item(MYSQL *conn, char *select, char *query) {
    clear_screen();
    printf("\033[31m `` Remove Item `` \n\033[33m");
    printf("\033[35m-------------------\n");

    //  --  RETRIEVING EXISTING ITEMS  --
    if (mysql_query(conn, "SELECT * FROM items")) {
        finish_with_error(conn);
    }
    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        finish_with_error(conn);
    }
    MYSQL_ROW row;
    printf("\033[33m  srno\t|\tItem\t|\tPrice\n");
    printf("-----------------------------------------\n\033[32m");

    while ((row = mysql_fetch_row(result))) {
        printf("   %s\t\033[33m|\033[32m\t%s\t\033[33m|\033[32m\t %s\n", row[0], row[1], row[2]);
    }

    mysql_free_result(result);

    //  --  ITEMS TO REMOVE  --
    printf("\033[0;1m Select item to remove: ");
    scanf(" %2s", select);  // Limiting input size

    sprintf(query, "DELETE FROM items WHERE srno = '%s'", select);
    if (mysql_query(conn, query)) {
        finish_with_error(conn);
    }

    //  --  RESETTING AUTO-INCREMENT AND srno COLUMN --
    sprintf(query, "UPDATE items SET srno = srno-1 WHERE srno > %s", select);
    if (mysql_query(conn, query)) {
        finish_with_error(conn);
    }

    if (mysql_query(conn, "SELECT MAX(srno) FROM items")) {
        finish_with_error(conn);
    }
    result = mysql_store_result(conn);
    if (result == NULL) {
        finish_with_error(conn);
    }

    row = mysql_fetch_row(result);
    mysql_free_result(result);

    if (row[0] != NULL) {
        sprintf(query, "ALTER TABLE items AUTO_INCREMENT = %s", row[0]);
    } else {
        sprintf(query, "ALTER TABLE items AUTO_INCREMENT = 1");
    }
    if (mysql_query(conn, query)) {
        finish_with_error(conn);
    }

    delay(1000); 
    printf("\033[32mItem successfully removed..\n");
    return 0;
}

//  --  MAIN FUNCTION  --
int main() {
    char query[100], select[3] = {0};  // Increased query buffer size

    //  --  Allocate memory for the bill  --
    char **bill = malloc(35 * sizeof(char *));
    for (int i = 0; i < 35; i++) {
        bill[i] = malloc(35 * sizeof(char));
        bill[i][0] = '\0';  // Initialize to empty strings
    }

    //  --  DATABASE CONNECTION SETUP  --
    MYSQL *conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed..\n");
        exit(1);
    }

    if (mysql_real_connect(conn, SERVER, USER, PASSWORD, DATABASE, 0, NULL, 0) == NULL) {
        finish_with_error(conn);
    }

    //  --  PROGRAM EXECUTION  --
    clear_screen();

    printf("\033[1;31m ```````     Item Bill Generator     ```````\n");
    printf("\033[35m---------------------------------------------\n\033[33m");
    printf(" 1. Add Item\n");
    printf(" 2. Remove Item\n");
    printf(" 3. Make Bill\n");
    printf(" 4. Exit\n");
    printf("\033[0;1mSelect: ");
    scanf(" %2s", select);

    switch (select[0]) {
        case '1':
            add_item(conn, query);
            break;
        case '2':
            remove_item(conn, select, query);
            break;
        case '3':
            make_bill(conn, bill, query, select);
            break;
        case '4':
            printf("See you later..\n");
            break;
        default:
            printf("\033[31mIncorrect Selection!!\n");
            break;
    }

    //  --  Free allocated memory for the bill  --
    for (int i = 0; i < 35; i++) {
        free(bill[i]);
    }
    free(bill);

    mysql_close(conn);
    return 0;
}
