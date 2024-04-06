// Made by Yarno Grenier -- ID 261179835

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include "database.h"

int SAVED = 1;

int check_if_comma(char *ptr)
{
        for (int i = 0; i < strlen(ptr); i++)
        {
                if (*(ptr + i) == ',')
                {
                        return 1;
                }
        }
        return 0;
}

int list(Database *db)
{
        printf("HANDLE                          | FOLLOWERS        | LAST MODIFIED       | COMMENT                                                    \n");
        puts("______________________________________________________________________________________________________________________________________");

        for (int i = 0; i < db->size; i++)
        {

                Record *ith_rec = (db->records + i);

                // Format time
                char date_last_modified[22];
                time_t converted_time = (time_t)(ith_rec->date_last_modified);
                strftime(date_last_modified, sizeof(date_last_modified), "%Y-%m-%d %H:%M", localtime(&converted_time));

                // Print single record
                printf("%-32s|%-18li|%-21s|%-64s\n", ith_rec->handle, ith_rec->followers, date_last_modified, ith_rec->comment);
        };
}

int authorize_exit(char *handle, int saved, int argc)
{
        int exit_authorized = 1;
        if (argc > 1)
        {
                if (!strcmp(handle, "fr") && argc == 2)
                {
                        puts("Exit forced");
                }
                else
                {
                        puts("USAGE > exit OR > exit fr");
                        exit_authorized = 0;
                }
        }
        else
        {

                // Check if changes were saved
                if (SAVED == 0)
                {
                        puts("Error: you did not save your changes. Use `exit fr ` to force exiting anyway.");
                        exit_authorized = 0;
                }
        }

        return exit_authorized;
}

int main_loop(Database *db)
{

        char function[32];
        char handle[32];
        long unsigned int followers;

        while (1)
        {
                printf("> ");
                char *line = NULL;
                size_t size = 0;
                size_t nread;
                nread = getline(&line, &size, stdin);
                line[strlen(line) - 1] = '\0';

                // Tokenize input
                char *token;
                char *endptr;
                token = strtok(line, " ");

                int argc = 0;
                int formatbreak = 0;

                // Assign values
                while (token != NULL && strcmp(token, " "))
                {
                        if (argc == 0)
                        {
                                strcpy(function, token);
                        }
                        else if (argc == 1)
                        {
                                if (strlen(token) > 31)
                                {
                                        puts("Handle is too long.");
                                        formatbreak = 1;
                                        break;
                                }
                                strcpy(handle, token);
                        }
                        else if (argc == 2)
                        {
                                // Convert followers to int
                                char *endpointer;
                                errno = 0;
                                followers = strtol(token, &endpointer, 10);
                                if (*endpointer != 0 || errno != 0 || token == endpointer)
                                {
                                        printf("Couldn't add %s -> follower count is NaN\n", handle);
                                        formatbreak = 1;
                                        break;
                                }

                                if (token[0] == '-')
                                {
                                        printf("Follower count cannot be negative\n");
                                        formatbreak = 1;
                                        break;
                                }
                        }
                        else
                        {
                                puts("Invalid number of args");
                                formatbreak = 1;
                                break;
                        }
                        token = strtok(NULL, " ");
                        argc++;
                }

                if (formatbreak == 1)
                {
                        continue;
                }

                if (!strcmp(function, "list"))
                {
                        if (db->size == 0)
                        {
                                puts("Database is empty.");
                                continue;
                        }
                        if (argc != 1)
                        {
                                puts("USAGE > list");
                                continue;
                        }
                        list(db);
                }
                else if ((!strcmp(function, "add") || !strcmp(function, "update")))
                {
                        if (argc != 3)
                        {
                                printf("USAGE > %s HANDLE FOLLOWERS\n", function);
                                continue;
                        }

                        SAVED = 0;

                        // Add @ if not there
                        char username[32] = "@";
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
                        printf("Comment> ");
                        char *comment = NULL;
                        size_t size = 0;
                        size_t nread;
                        nread = getline(&comment, &size, stdin);
                        comment[strlen(comment) - 1] = '\0';

                        // Get length restriction
                        if (strlen(comment) > 63)
                        {
                                puts("Comment is too long.");
                                continue;
                        }

                        // Check if comma in comment
                        if (check_if_comma(comment) != 0)
                        {
                                puts("Comment cannot contain commas.");
                                continue;
                        }

                        if (!strcmp(function, "update"))
                        {
                                // Check error case
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
                        else if (!strcmp(function, "add"))
                        {
                                // Check error case
                                if (db_lookup(db, username) != NULL)
                                {
                                        printf("ERROR: record %s already exists\n", username);
                                        continue;
                                }

                                // Assign values
                                Record new_rec;
                                strcpy(new_rec.handle, username);
                                strcpy(new_rec.comment, comment);
                                new_rec.followers = followers;
                                new_rec.date_last_modified = (long unsigned int)date_last_modified;
                                db_append(db, &new_rec);
                        }
                }
                else if (!strcmp(function, "save"))
                {
                        if (argc != 1)
                        {
                                puts("USAGE > save");
                        }
                        SAVED = 1;
                        printf("Wrote %d record(s)\n", db->size);
                        db_write_csv(db, "database.csv");
                }
                else if (!strcmp(function, "exit"))
                {
                        if (authorize_exit(handle, SAVED, argc) == 1)
                        {
                                break;
                                db_free(db);
                        }
                        continue;
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
                free(line);
        }
        return 0;
}

int main()
{
        Database db = db_create();
        db_load_csv(&db, "database.csv");
        return main_loop(&db);
}
