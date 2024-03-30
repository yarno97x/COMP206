#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "database.h"

int main_loop(Database * db)
{
    char* function;
    char* arg1;
    char* arg2;

    while(1) {
        scanf("%s %s %s", function, arg1, arg2);

        if (!strcmp(function, "list")) {

        } else if (!strcmp(function, "add")) {
                if (!strcmp(arg1, "") || !strcmp(arg2, "")) {
                        printf("Invalid args");
                        continue;
                }

                if(db_lookup(*db, *arg1) == NULL) {
                        db_append(*db, *arg1);
                } else {
                        printf("Error: handle %s already exists.", arg1);
                }
        } else if (!strcmp(function, "update")) {
                // Update an entry
        } else if (!strcmp(function, "save")) {
                db_write_csv(*db, "database.csv");
        } else if (!strcmp(function, "exit")) {
                if (!strcmp(arg1, "fr")) {
                        puts("Exit forced.");
                        break;
                }

                // Check if changes were saved
                if (changesNotSaved) {
                        puts("Error: you did not save your changes. Use `exit fr ` to force exiting anyway.");
                        continue;
                }

                break;


        } else {
                puts("Invalid function. Try again.");
                continue;
        }
    }
    return 0;
}

int main()
{
    Database db = db_create();
    db_load_csv(&db, "database.csv");
    return main_loop(&db);
}
