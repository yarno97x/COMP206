#include "database.h"

Database db_create()
{
    Record *p;
    struct Database myDatabase = {p, 0, 1};
    return myDatabase;
}

void db_append(Database *db, Record const *item);

Record db_index(Database *db, int index);

Record db_lookup(Database *db, char const *handle);

void db_free(Database *db);

void db_load_csv(Database *db, char const *path);

void db_write_csv(Database *db, char const *path);
