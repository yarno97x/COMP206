#include "database.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

void print_record(Record const *r)
{
    printf("%s %s %ld %ld\n", r->handle, r->comment, r->followers, r->date_last_modified);
}

Database db_create() {
    Record *point = (Record *)malloc(4*sizeof(Record));
    struct Database new_db = {point, 0, 4};

    return new_db;
}

int main()
{
    Record *point = (Record *)malloc(sizeof(Record));
    struct Database hello = {point, 0, 1};

    Record r = (Record){
        .handle = "@spottedmcgill",
        .comment = "a bit cringe tbh",
        .followers = 14900,
        .date_last_modified = 10202030,
    };
    db_append(&hello, &r);
    print_record(db_index(&hello, 0));
    printf("%d and size=%d", hello.capacity, hello.size);

    Record s = (Record){
        .handle = "@potedmcgill",
        .comment = "a cringe tbh",
        .followers = 1900,
        .date_last_modified = 10230,
    };
    db_append(&hello, &s);
    print_record(db_index(&hello, 1));
    printf("%d and size=%d", hello.capacity, hello.size);

    Record u = (Record){
        .handle = "@potedmll",
        .comment = "a cr tbh",
        .followers = 190220,
        .date_last_modified = 1022230,
    };
    db_append(&hello, &u);
    print_record(db_index(&hello, 2));
    printf("%d and size=%d", hello.capacity, hello.size);

    Record p = (Record){
        .handle = "@potedmll",
        .comment = "a ch",
        .followers = 190220,
        .date_last_modified = 1022230,
    };
    db_append(&hello, &p);
    print_record(db_index(&hello, 3));
    printf("%d and size=%d", hello.capacity, hello.size);
    printf("\n%s", db_lookup(&hello, "@potedmll")->comment);

    return 0;
}
