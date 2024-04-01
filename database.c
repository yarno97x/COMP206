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
        // puts("Creating first entry...");
        *(db->records) = *item;
    }
    else if (db->size < db->capacity)
    {
        // puts("Size < Capacity. Appending...");

        // Treat size as index
        *(db->records + db->size) = *item;
    }
    else
    {
        // puts("Array is too small. Resizing...");

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
    // printf("New size: %d\n", db->size);
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
    // puts("Handle not found");
    return NULL;
}

void db_free(Database *db)
{
    free(db->records);
    free(db);
}

void print_record(Record const *r)
{
    printf("%s %s %li %li\n", r->handle, r->comment, r->followers, r->date_last_modified);
}

Record parse_record(char *line)
{

    // Copy line
    char copy[128];
    strcpy(copy, line);
    copy[strlen(copy) - 1] = '\0';

    // printf("Line is ..%s.. \n", copy);

    // Define an error record
    Record ERROR = (Record){
        .handle = "@Error_Record",
        .comment = "Error adding this record",
        .followers = 0,
        .date_last_modified = 0,
    };
    // puts("error record initialized");

    // Tokenize copy
    char *token;
    token = strtok(copy, ",");
    // puts("strtok works");

    // Define record fields
    // char handle[32], comment[64];
    // long unsigned int followers, date_last_modified;
    char *endpointer = " ";
    int argc = 0;
    // puts("while loop starting");

    // size_t token_len = 1;
    Record *new_record = (Record *)malloc(sizeof(Record));
    // Assign values
    while (token != NULL)
    {

        switch (argc)
        {
        case 0:
            strcpy(new_record->handle, token);
            break;
        case 1:
            errno = 0;
            new_record->followers = strtol(token, &endpointer, 10);
            if (errno != 0)
            {
                printf("Invalid input --> '%s' contains non-numeric chars\n", token);
                return ERROR;
            }
            break;
        case 2:
            strcpy(new_record->comment, token);
            break;
        case 3:
            errno = 0;
            new_record->date_last_modified = strtol(token, &endpointer, 10);
            if (errno != 0)
            {
                printf("Invalid input --> '%s' contains non-numeric chars\n", token);
                return ERROR;
            }
        default:
            break;
        }
        argc++;
        token = strtok(NULL, ",");
    }

    // Check error cases
    // puts("checking if # of args is ok");
    if (argc != 4)
    {
        //    puts("Invalid number of args");
        return ERROR;
    }
    // puts("checking if too many args");
    if (token != NULL)
    {
        //    puts("Invalid CSV format");
        return ERROR;
    }
    // puts(" ");
    // print_record(new_record);

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
    char *line = NULL; // this NULL initialization is important
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
/*int main()
{
    Record *point = (Record *)malloc(4 * sizeof(Record));
    struct Database hello = {point, 0, 4};
    char *path = "database.csv";
    db_load_csv(&hello, path);
    for (int i = 0; i <= 4; i++)
    {
        print_record(db_index(&hello, i));
        puts(" ");
    }

    printf("FINAL SIZE: %d\n", hello.size);
    Record r = (Record){
        .handle = "@spottedmcgill",
        .comment = "a bit cringe tbh",
        .followers = 14900,
        .date_last_modified = 100,
    };
    puts("Appending new value");
    db_append(&hello, &r);
    printf("size: %d and cap: %d\n", hello.size, hello.capacity);
    print_record(db_index(&hello, 6));
}
*/