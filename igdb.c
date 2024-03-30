#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "database.h"

int main_loop(Database *db)
{
        char *function;
        char *arg1;
        char *arg2;

        while (1)
        {
                scanf("%s %s %s", function, arg1, arg2);

                if (!strcmp(function, "list"))
                {
                        printf("HANDLE          | FOLLOWERS | LAST MODIFIED | COMMENT                    ");
                        puts("_________________________________________________________________________");

                        for (int i = 0; i < db->size; i++)
                        {       

                                // Truncate handle/comment if necessary
                                char handle[17];
                                char comment[29];
                                Record *ith_rec = (db->records + i);
                                strncpy(handle, ith_rec->handle, 16);
                                strncpy(comment, ith_rec->comment, 28);

                                // Format time
                                //char date_last_modified[16];
                                //strftime(date_last_modified, sizeof(date_last_modified), "%Y-%m-%d %H:%M", ith_rec->date_last_modified);

                                printf("%-16s|%-11s|%-15s|%-28s", handle, ith_rec->followers, ith_rec->date_last_modified, comment);
                        };
                }
                else if (!strcmp(function, "add"))
                {
                        if (!strcmp(arg1, "") || !strcmp(arg2, ""))
                        {
                                printf("Invalid args");
                                continue;
                        }

                        if (db_lookup(db, arg1) == NULL)
                        {
                                db_append(db, arg1);
                        }
                        else
                        {
                                printf("Error: handle %s already exists.", arg1);
                        }
                }
                else if (!strcmp(function, "update"))
                {
                        // Update an entry
                }
                else if (!strcmp(function, "save"))
                {
                        db_write_csv(db, "database.csv");
                }
                else if (!strcmp(function, "exit"))
                {
                        if (!strcmp(arg1, "fr"))
                        {
                                puts("Exit forced.");
                                break;
                        }

                        /* Check if changes were saved
                        if (changesNotSaved) {
                                puts("Error: you did not save your changes. Use `exit fr ` to force exiting anyway.");
                                continue;
                        }

                        break;*/
                }
                else
                {
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
