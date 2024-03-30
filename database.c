#include "database.h"
#include <stdio.h>
#include <stdlib.h>

Database db_create()
{
    // Instantiate database
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
        // Add new item
        *(new_records + db->size) = *item;

        // Free old array
        free(db->records);
        db->records = new_records;
    }
    // Increment size
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
    puts("Handle not found");
    return NULL;
}

void db_free(Database *db)
{
    free(db->records);
    db->size = 0;
    db->capacity = 4;
}

void db_load_csv(Database *db, char const *path)
{
    FILE *file = fopen(path, "r");

    if (file == NULL)
    {
        printf("Error opening file.\n");
        return;
    }

    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file) != NULL)
    {

        // Split lines
        char *token = strtok(line, ",");

        while (token != NULL)
        {
            db_append(db, parse_record(token));
            token = strtok(NULL, ",");
        }
    }
    fclose(file);
}

void db_write_csv(Database *db, char const *path)
{

    FILE *file = fopen(path, "w");

    // Add records
    for (int i = 0; i < (*db).size; i++)
    {
        // Edit the fprintf
        struct Record *my_record = ((*db).records + i);
        char date_last_modified[64];
        strftime(date_last_modified, sizeof(date_last_modified), "%Y-%m-%d %H:%M", (*my_record).date_last_modified);
        fprintf(file, "%s,%li,%s,%s\n", (*my_record).handle, (*my_record).followers, (*my_record).comment, date_last_modified);
    }

    fclose(file);
}

void print_record(Record const *r)
{
    printf("%s %s %ld %ld\n", r->handle, r->comment, r->followers, r->date_last_modified);
}
