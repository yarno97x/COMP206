#include "database.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

Database db_create()
{
    Record *point = (Record *)malloc(4 * sizeof(Record));
    struct Database new_db = {point, 0, 4};
    return new_db;
}

void db_append(Database *db, Record const *item)
{
    if (db->size == 0)
    {
        *(db->records) = *item;
    }
    else if (db->size < db->capacity)
    {
        // Treat size as index
        *(db->records + db->size) = *item;
    }
    else
    {

        // Resize to double
        db->capacity *= 2;
        Record *new_records = (Record *)malloc(db->capacity * sizeof(Record));

        // Reassign values
        for (int i = 0; i < (*db).size; i++)
        {
            *(new_records + i) = *(db->records + i);
        }
        *(new_records + db->size) = *item;

        // Free old array
        free(db->records);
        db->records = new_records;
    }

    db->size++;
    return;
}

Record *db_index(Database *db, int index)
{
    if (index >= db->size || index < 0)
    {
        puts("Invalid index");
        return NULL;
    }
    return (db->records + index);
}

Record *db_lookup(Database *db, char const *handle)
{
    for (int i = 0; i < db->size; i++)
    {
        if (!strcmp(handle, (db->records + i)->handle))
        {
            return (db->records + i);
        }
    }
    return NULL;
}

void db_free(Database *db)
{
    free(db->records);
}

Record parse_record(char *line)
{

    // Copy line
    char copy[128];
    strcpy(copy, line);
    copy[strlen(copy) - 1] = '\0';

    // Define an error record
    Record ERROR = (Record){
        .handle = "@Error_Record",
        .comment = "Error adding this record",
        .followers = 0,
        .date_last_modified = 0,
    };

    // Tokenize copy
    char *token;
    token = strtok(copy, ",");

    char *endpointer = " ";
    int argc = 0;

    // Instantiate new record
    Record *new_record = (Record *)malloc(sizeof(Record));

    // Assign values depending on their position (0->handle,1->followers,2->comment,3->date)
    while (token != NULL)
    {

        switch (argc)
        {
        case 0:
            strcpy(new_record->handle, token);
            break;
        case 1:
            char *endpointer;
            errno = 0;
            new_record->followers = strtol(token, &endpointer, 10);
            if (*endpointer != 0 || errno != 0 || token == endpointer)
            {
                printf("Couldn't add %s -> follower count is NaN\n", new_record->handle);
                return ERROR;
            }

            if (new_record->followers < 0)
            {
                printf("Follower count cannot be negative\n");
                return ERROR;
            }
            break;
        case 2:
            strcpy(new_record->comment, token);
            break;
        case 3:
            char *endptr;
            errno = 0;
            new_record->date_last_modified = strtol(token, &endptr, 10);
            if (*endptr != 0 || errno != 0 || token == endptr)
            {
                printf("Couldn't add %s -> Date Last Modified is NaN\n", new_record->handle);
                return ERROR;
            }

            if (new_record->date_last_modified < 0)
            {
                printf("Date last modified cannot be negative\n");
                return ERROR;
            }
            break;
        default:
            break;
        }
        argc++;
        token = strtok(NULL, ",");
    }

    // Check possible error cases
    if (argc != 4)
    {
        return ERROR;
    }
    if (token != NULL)
    {
        return ERROR;
    }

    return *new_record;
}

void db_load_csv(Database *db, char const *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }
    char *line = NULL;
    size_t size = 0;
    size_t nread;

    do
    {
        nread = getline(&line, &size, file);
        Record new_rec = parse_record(line);
        if (strcmp(new_rec.comment, "Error adding this record"))
        {
            db_append(db, &new_rec);
        }
        line = NULL;
    } while (nread != -1);
    free(line);
    fclose(file);
}

void db_write_csv(Database *db, char const *path)
{

    FILE *file = fopen(path, "w");

    // Add records
    for (int i = 0; i < db->size; i++)
    {
        // Edit the fprintf
        struct Record *my_record = (db->records + i);
        fprintf(file, "%s,%li,%s,%li\n", my_record->handle, my_record->followers, my_record->comment, my_record->date_last_modified);
    }

    fclose(file);
}
