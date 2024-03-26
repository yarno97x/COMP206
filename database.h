#ifndef DB_H
#define DB_H

typedef struct Record { /* TODO */ } Record;

typedef struct Database { /* TODO */ } Database;

Database db_create();

void db_append(Database * db, Record const * item);

void db_index(Database * db, int index);

void db_lookup(Database * db, char const * handle);

void db_free(Database * db);

void db_load_csv(Database * db, char const * path);

void db_write_csv(Database * db, char const * path);

#endif
