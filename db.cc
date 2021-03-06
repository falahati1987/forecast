#include "db.h"
#include <stdio.h>
#include <stdexcept>

bool Db::isCitiesLoaded = true;
std::map<std::string, std::string> Db::map_suggestions;

Db *Db::instance = UNINTIALIZED_INSTANCE;

Db *Db::getInstance()
{

    if (instance == UNINTIALIZED_INSTANCE)
        instance = new Db();

    return instance;
}

Db::Db()
{
    open();
    setup_db();
}

Db::~Db()
{
    sqlite3_close(db);
}
int Db::callback_get_settings(void *data, int argc, char **argv, char **azColName)
{
    isCitiesLoaded = atoi(argv[0]) == 1;
    return 0;
}

int Db::callback(void *data, int argc, char **argv, char **azColName)
{

    int i;
    for (i = 0; i < argc; i++)
    {
        map_suggestions.insert(std::make_pair<std::string, std::string>(argv[i],
                                                                        std::string(argv[i + 1]) +
                                                                            " | " +
                                                                            std::string(argv[i + 2])));
        i += 2; // We put 2 items in a row
    }
    return 0;
}
int Db::callback_get_history(void *data, int argc, char **argv, char **azColName)
{
    int i;
    
    for (i = 0; i < argc; i++)
    {
        map_suggestions.insert(std::make_pair<std::string, std::string>(argv[i], argv[i+1]));
        i++; // We put 2 items in a row
    }
    return 0;
}

void Db::open()
{
    rc = sqlite3_open("forecast.db", &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}

void Db::setup_db()
{
    /* Execute SQL statement */
    if (db == NULL)
    {
        std::cout << "Reopenning DB" << std::endl;
        open();
    }
    rc = exec("CREATE TABLE IF NOT EXISTS Cities("
              "ID INTEGER PRIMARY KEY NOT NULL,"
              "NAME      TEXT    NOT NULL,"
              "country   CHAR(30)   NOT NULL,"
              "lat       REAL,"
              "long      REAL );");

    rc = exec("CREATE TABLE IF NOT EXISTS Settings("
              "ID INT PRIMARY KEY NOT NULL,"
              "NAME      TEXT    NOT NULL,"
              "status   bool   NOT NULL"
              ");");

    rc = exec("CREATE TABLE IF NOT EXISTS Selected_Cities("
              "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
              "city_id INTEGER UNIQUE, "
              "is_default BOOLEAN, "
              "created_at DATETIME,  "
              "FOREIGN kEY(city_id) REFERENCES Cities(ID) );");

    rc = exec("CREATE TABLE IF NOT EXISTS temperature_history("
              "city_id INTEGER, "
              "temperature FLOAT, "
              "sampled_at BIGINT, "
              "created_at DATETIME,  "
              "FOREIGN kEY(city_id) REFERENCES Cities(ID) , PRIMARY KEY(city_id, sampled_at) "
              ");");

    rc = exec("INSERT OR IGNORE INTO Settings ("
              "ID,"
              "NAME,"
              "status)"
              "VALUES (1, 'cities_loaded', 0)"
              ";");
}

int Db::exec(const char *query)
{
    rc = sqlite3_exec(db, query, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    return rc;
}
/**
 * This method inserts the cities into database. 
 * The @var values are generated so that it can be inserted easily into db
 * 
 * */
ErrorCodes Db::insert_in_db(const char *dbName, const char *values)
{
    std::string query;
    try
    {
        query = "INSERT INTO " + std::string(dbName) + "  " + std::string(values);
        open();
        exec(query.c_str());
    }
    catch (const std::logic_error &e)
    {
        return DATA_INSERTION_ERROR;
        std::cout << e.what();
    }
    return NO_ERROR;
}

void Db::update_status(const char *statusName, bool status)
{
    std::string query;
    std::string str_status = (status ? "0 " : "1 ");
    query = "update Settings set status=" + str_status + " where NAME='" + std::string(statusName) + "';";
    exec(query.c_str());
}

std::map<std::string, std::string> Db::select_from_db(const char *dbName, const char *params, const char *whereClause)
{
    map_suggestions.clear();
    std::string query;
    query = "SELECT " + std::string(params) +
            " from " +
            std::string(dbName) +
            " where NAME like '" +
            std::string(whereClause) +
            "%' limit 5";

    const char *data = "Callback function called";
    /* Execute SQL statement */
    rc = sqlite3_exec(db, query.c_str(), callback, (void *)data, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    return map_suggestions;
}

bool Db::should_update_cities()
{
    std::string query;
    query = "select status from Settings where name='cities_loaded'";

    const char *data = "Callback function called";
    /* Execute SQL statement */
    rc = sqlite3_exec(db, query.c_str(), callback_get_settings, (void *)data, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    return isCitiesLoaded;
}

bool Db::update_in_db(const char *dbName, const char *setValues, const char *whereClause)
{
    std::string query;
    query = "update " + std::string(dbName) + " set " + std::string(setValues) + " where " + std::string(whereClause);
    rc = exec(query.c_str());
    return isCitiesLoaded;
}

std::map<std::string, std::string> Db::select_picked_cities_from_db(const char *dbName,
                                                                    const char *params,
                                                                    const char *whereClause)
{
    map_suggestions.clear();
    std::string query;
    query = "SELECT  B.id , B.name , B.country from " +
            std::string(dbName) +
            " as A inner join Cities as B ON (A.city_id = B.id) order by B.name";

    rc = sqlite3_exec(db, query.c_str(), callback, NULL, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }

    return map_suggestions;
}

void Db::delete_selected_city(int &id)
{
    std::string query;
    query = "delete from Selected_Cities where city_id = " + std::to_string(id);

    const char *data = "Deletion";
    /* Execute SQL statement */
    rc = sqlite3_exec(db, query.c_str(), NULL, (void *)data, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}

void Db::log_temperature(const int &cityId, const int &temp, const int &sampledTime)
{
    std::string query("INSERT OR REPLACE INTO temperature_history (city_id ,"
                      " temperature, sampled_at, created_at) VALUES (?, ?, ?, datetime('now') )");
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, cityId);
    sqlite3_bind_double(stmt, 2, (float)temp);
    sqlite3_bind_int64(stmt, 3, sampledTime);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}
/**
 * 
 * 
 * */
std::map<std::string, std::string> Db::get_temperature_history(const int &cityId, const int &startAt)
{
    map_suggestions.clear();

    std::string query;
    query = "SELECT  sampled_at, temperature from temperature_history "
            " where city_id= " +
            std::to_string(cityId) + " and sampled_at > " + std::to_string(startAt) + " order by sampled_at asc;";

    rc = sqlite3_exec(db, query.c_str(), callback_get_history, NULL, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return map_suggestions;
}