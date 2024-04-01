#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "database.h"
int SAVED = 1;

int main_loop(Database *db)
{
        char operation[1024];
        char function[32];
        char handle[32];
        long unsigned int followers;

        while (1)
        {
                printf("> ");

                // Get operation and remove the whitespaces
                fgets(operation, sizeof(operation), stdin);
                operation[strlen(operation) - 1] = '\0';
                // printf("%s is the function\n", operation);

                // Tokenize input
                char *token;
                char *endptr;
                token = strtok(operation, " ");
                int argc = 0;

                // Assign values
                while (token != NULL && strcmp(token, " "))
                {
                        // printf("%s\n", token);
                        if (argc == 0)
                        {
                                strcpy(function, token);
                        }
                        else if (argc == 1)
                        {
                                strcpy(handle, token);
                        }
                        else if (argc == 2)
                        {
                                errno = 0;
                                followers = strtol(token, &endptr, 10);
                                if (errno != 0)
                                {
                                        printf("Invalid input --> '%s' contains non-numeric chars\n", token);
                                        break;
                                }
                        }
                        else
                        {
                                puts("Invalid number of args");
                                break;
                        }
                        token = strtok(NULL, " ");
                        argc++;
                }

                if (!strcmp(function, "list"))
                {
                        if (argc != 1)
                        {
                                puts("USAGE > list");
                                continue;
                        }
                        printf("HANDLE                          | FOLLOWERS        | LAST MODIFIED       | COMMENT                                                    \n");
                        puts("____________________________________________________________________________________________________________________________________");

                        for (int i = 0; i < db->size; i++)
                        {

                                Record *ith_rec = (db->records + i);

                                // Format time
                                char date_last_modified[22];
                                time_t converted_time = (time_t)(ith_rec->date_last_modified);
                                strftime(date_last_modified, sizeof(date_last_modified), "%Y-%m-%d %H:%M", localtime(&converted_time));

                                printf("%-32s|%-18li|%-21s|%-64s\n", ith_rec->handle, ith_rec->followers, date_last_modified, ith_rec->comment);
                        };
                }
                else if ((!strcmp(function, "add") || !strcmp(function, "update")))
                {
                        if (argc != 3)
                        {
                                printf("USAGE > %s HANDLE FOLLOWERS\n", function);
                                continue;
                        }

                        SAVED = 0;
                        if (!strcmp(handle, ""))
                        {
                                printf("Invalid args");
                                continue;
                        }

                        char username[32] = "@";
                        // Add @ if not there
                        if (handle[0] != '@')
                        {
                                strcat(username, handle);
                        }
                        else
                        {
                                strcpy(username, handle);
                        }

                        // Get time
                        time_t date_last_modified;
                        time(&date_last_modified);

                        // Get comment
                        char comment[64];
                        printf("Comment> ");
                        fgets(comment, sizeof(comment) - 1, stdin);
                        comment[strlen(comment) - 1] = '\0';

                        if (!strcmp(function, "update") && (db_lookup(db, username) != NULL))
                        {
                                if (db_lookup(db, username) == NULL)
                                {
                                        printf("Record with handle %s not found\n", username);
                                        continue;
                                }

                                // Assign values
                                strcpy(db_lookup(db, username)->handle, username);
                                strcpy(db_lookup(db, username)->comment, comment);
                                db_lookup(db, username)->followers = followers;
                                db_lookup(db, username)->date_last_modified = (long unsigned int)date_last_modified;
                        }
                        else if (!strcmp(function, "add") && (db_lookup(db, username) == NULL))
                        {
                                if (db_lookup(db, username) != NULL)
                                {
                                        printf("ERROR: record %s already exists", username);
                                        continue;
                                }

                                Record new_rec;
                                strcpy(new_rec.handle, username);
                                strcpy(new_rec.comment, comment);
                                new_rec.followers = followers;
                                new_rec.date_last_modified = (long unsigned int)date_last_modified;
                                db_append(db, &new_rec);
                        }
                }
                else if (!strcmp(function, "save") && argc == 1)
                {
                        SAVED = 1;
                        db_write_csv(db, "database.csv");
                }
                else if (!strcmp(function, "exit") && (argc == 1 || argc == 2))
                {
                        if (argc == 2)
                        {
                                if (!strcmp(handle, "fr"))
                                {
                                        puts("Exit forced");
                                        break;
                                }
                                puts("USAGE > exit OR > exit fr");
                                continue;
                        }

                        // Check if changes were saved
                        if (SAVED == 0)
                        {
                                puts("Error: you did not save your changes. Use `exit fr ` to force exiting anyway.");
                                continue;
                        }

                        break;
                }
                else if (!strcmp(function, "clear"))
                {
                        system("clear");
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
