#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "database.h"

int main_loop(Database * db)
{
    /* TODO */
}

int main()
{
    Database db = db_create();
    db_load_csv(&db, "database.csv");
    return main_loop(&db);
}
