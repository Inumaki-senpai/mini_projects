#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mariadb/mysql.h>

#define SERVER "localhost"
#define USER "meow"
#define PASSWORD "MeowNagar.."
#define DATABASE "mini_projects"

// TIME DELAY FUNCTION;
int delay(unsigned int seconds) {
    struct timespec req = {0};
    req.tv_sec = seconds;
    nanosleep(&req, NULL);
}

// SQL ERROR HANDLING;
void finish_with_error(MYSQL *con){
    fprintf(stderr, "\033[1;31m%s\n", mysql_error(con));
    mysql_close(con);
    delay(1);
    exit(1);
}

// FUNCTIONS FOR MANAGING INDIVIDUAL FUNCTIONALITIES;
int quiz(MYSQL *con) {
    system("clear");
    printf("\033[1;33m`````   Quiz:  `````\n");

    //  DECLARING VARIABLES FOR THE FUNCTION;
    int num = 0, limit = 0, score = 0;
    char ques[1001], ans[5001], query[100];
    time_t t;

    // CHECKING AVAILABILITY OF QUESTIONS;
    if(mysql_query(con,"SELECT MAX(srno) FROM quiz")) {
        finish_with_error(con);
    }

    MYSQL_RES *result = mysql_store_result(con);

    if(result == NULL) {
        finish_with_error(con);
    }

    MYSQL_ROW row = mysql_fetch_row(result);

    mysql_free_result(result);

    if (row[0] != NULL) {
        limit = atoi(row[0]);
    }
    else {
        printf("\033[1;31mError: No questions to use for quiz..\n");
        delay(1);
        return 0;
    }
    
    printf("\033[0;1mNumber of questions to ask: ");
    scanf(" %d", &num);

    //      QUIZ IN PROGRESS;
    srand((unsigned) time(&t));

    for (int i = 0; i < num; i++) {
        int value = (rand() % limit)+1;             // GENERATION OF RANDOM srno TO PICK QUESTION;

        //      RETRIEVING QUESTION 
        sprintf(query,"SELECT question, answer FROM quiz WHERE srno = %d", value);
        if(mysql_query(con,query)) {
            finish_with_error(con);
        }
        MYSQL_RES *result = mysql_store_result(con);

        if(result == NULL) {
            finish_with_error(con);
        }

        MYSQL_ROW row = mysql_fetch_row(result);

        mysql_free_result(result);

        if (row[0] == NULL) {
            finish_with_error(con);
        }
        
        //      QUESTION       ANSWER;
        printf("%d. %s\n", i+1, row[0]);
        printf("Answer: ");
        scanf(" %[^\n]s", ans);

        if(!strcmp(ans,row[1])) {
            score++;
        }
    }

    //      GENERATING  RESULT;
    printf("\033[1;35mHere's your result..\n");

    if((score*100/num) < 27) {
        printf("\033[1;31mYou fail!\n");
    }
    else if((score*100/num) < 50) {
        printf("\033[34mNeeds improvement!\n\033[0;1mYou got %d correct.. %d\%\n", score, (score*100/num));
    }
    else if((score*100/num) < 75) {
        printf("\033[33mGreat work! Keep it up.\n\033[0;1mYou got %d correct.. %d\%\n", score, (score*100/num));
    }
    else if((score*100/num) < 90) {
        printf("\033[36mAwesome! Just a little more and you'll be one of the best.\n\033[0;1mYou got %d correct.. %d\%\n", score, (score*100/num));
    }
    else {
        printf("\033[32mIncredible performance! Your score is one of the top scores.\n\033[0;1mYou got %d correct.. %d\%\n", score, (score*100/num));
    }

    return 0;
}

int remove_ques(MYSQL *con) {
    int num = 1, select = 0;
    char ques[1001], query[6100];

    // RECIEVING REQUEST TO REMOVE QUESTIONS;
    system("clear");
    
    printf("\033[0;1;33mRemove Question: \n\033[0m");
    printf("1. Enter serial no\n");
    printf("2. Enter question\n");
    printf("3. Delete all\n");
    printf("4. Delete all by year\n\033[0;1m");
    printf("Enter selection number: ");
    scanf(" %d", &select);

    // REMOVING QUESTIONS AS PER REQUEST;
    switch (select) {
    case 1:
        printf("Enter serial number: ");
        scanf(" %d", &select);

        sprintf(query, "DELETE FROM quiz WHERE srno = %d", select);
        if (mysql_query(con, query)) {
            finish_with_error(con);
        }
        
        printf("\033[1;32mSuccessfully deleted all questions..\n\033[0m");
        break;
    
    case 2:
        printf("Enter question: ");
        scanf(" %[^\n]s", ques);

        sprintf(query, "DELETE FROM quiz WHERE question = '%s'", ques);
        if (mysql_query(con, query)) {
            finish_with_error(con);
        }
        
        printf("\033[1;32mSuccessfully deleted all questions..\n\033[0m");
        break;
    
    case 3:
        sprintf(query, "DELETE FROM quiz");
        if(mysql_query(con, query)) {
            finish_with_error(con);
        }

        printf("\033[1;32mSuccessfully deleted all questions..\n\033[0m");
        break;
    
    case 4:
        printf("Enter year: ");
        scanf(" %d", &select);
        
        sprintf(query, "DELETE FROM quiz WHERE year = %d", select);
        if(mysql_query(con, query)) {
            finish_with_error(con);
        }

        printf("\033[1;32mSuccessfully deleted all questions..\n\033[0m");
        break;
    
    default:
        printf("\033[1;31mError: Incorrect Selection!\nTerminating Program..\n");
        mysql_close(con);
        delay(1);
        exit(1);
    }

    // UPDATING srno TO BE CONTIGUOUS;

    if (mysql_query(con, "SET @count = 0;")) {
        finish_with_error(con);
    }

    if (mysql_query(con, "UPDATE quiz SET srno = @count:= @count + 1;")) {
        finish_with_error(con);
    }

    // RESETTING AUTO_INCREMENT FACTOR OF TABLE;
    sprintf(query,"SELECT MAX(srno) FROM quiz");
    if(mysql_query(con,query)) {                                            // REQUESTING MAX(srno);
        finish_with_error(con);
    }

    MYSQL_RES *result = mysql_store_result(con);

    if(result == NULL) {
        finish_with_error(con);
    }

    MYSQL_ROW row = mysql_fetch_row(result);

    mysql_free_result(result);

    if (row[0] != NULL) {
        num = atoi(row[0]);
    }

    if (num > 1) {                                                            // TABLE AUTO_INCREMENT RESET;
        sprintf(query, "ALTER TABLE quiz AUTO_INCREMENT = %d", num);
    } else {
        sprintf(query, "ALTER TABLE quiz AUTO_INCREMENT = 1");
    }
    
    if(mysql_query(con, query)) {
        finish_with_error(con);
    }

    return 0;
}

int add_ques(MYSQL *con) {
    char ques[1001], ans[5001], query[6100], year[5];

    system("clear");
    printf("\033[0;1;33mAdd Question: \n");
    
    printf("\033[0;1mQuestion: \033[0m");
    scanf(" %[^\n]s", ques);
    printf("\033[1mAnswer: \033[0m");
    scanf(" %[^\n]s", ans);
    printf("\033[1mYear Related: \033[0m");
    scanf(" %s", year);

    sprintf(query, "INSERT INTO quiz(question, answer, year) VALUES('%s', '%s', '%s')", ques, ans, year);

    if(mysql_query(con,query)) {
        finish_with_error(con);
    }
    printf("\033[1;32mSuccessfully added the new question..\n\033[0;1m");

    return 0;
}

// MAIN FUNCTION

int main() {

    // SETTING UP DATABASE CONNECTION;
    MYSQL *con = mysql_init(NULL);
    if (con == NULL){
        fprintf(stderr, "mysql_init() failed\n");
        mysql_close(con);
        delay(1);
        exit(1);
    }
    if (mysql_real_connect(con, SERVER, USER, PASSWORD,
                           DATABASE, 0, NULL, 0) == NULL){
        finish_with_error(con);
    }

    // DECLARING PROGRAM VARIABLES;
    int select;

    // PROGRAM EXECUTION;
    main_menu:
    printf("\033[1;31m*** The Quiz ***\033[33m\n");
    printf("1. Add question\n");
    printf("2. Remove questions\n");
    printf("3. Take quiz\n");
    printf("4. Exit\n");
    
    printf("\033[0;1mEnter selection number: ");
    scanf(" %d", &select);

    switch (select) {
    case 4:
        printf("\033[0;1mSee you later..\n");
        mysql_close(con);
        delay(1);
        exit(1);
    case 3:
        quiz(con);
        break;
    case 2:
        remove_ques(con);
        delay(2);
        system("clear");
        goto main_menu;
        break;
    case 1:
        add_ques(con);
        delay(2);
        system("clear");
        goto main_menu;
        break;
    default:
        printf("\033[1;31mError: Incorrect Selection!\n\n");
        delay(1);
        goto main_menu;
        break;
    }

    mysql_close(con);
    delay(1);

    return 0;
}
