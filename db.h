#ifndef DB_H
#define DB_H
#define UNINTIALIZED_INSTANCE 0
#include <sqlite3.h>
#include <vector>
#include <iostream>
#include <string>
#include <map>
#include "commons.h"

class Db
{
public:
  Db();
  virtual ~Db();
  static Db *getInstance();

  void notify(std::map<std::string, std::string> vec_Cities);
  ErrorCodes insert_in_db(const char *dbName, const char *values);
  void update_status(const char *statusName, bool status);
  static int callback(void *data, int argc, char **argv, char **azColName);
  void delete_selected_city(int &id);

  // template <typename T>
  std::map<std::string, std::string> select_from_db(
      const char *dbName,
      const char *params,
      const char *whereClause);
  std::map<std::string, std::string> select_picked_cities_from_db(const char *dbName,
                                                                  const char *params,
                                                                  const char *whereClause);
  static int callback_get_settings(void *data, int argc, char **argv, char **azColName);
  bool should_update_cities();
  bool update_in_db(const char *dbName, const char *setValues, const char *whereClause);
  int exec(const char *query);

private:
  static Db *instance;
  sqlite3 *db = nullptr;
  char *zErrMsg = 0;
  int rc;
  const char *sql;
  static bool isCitiesLoaded;
  static std::map<std::string, std::string> map_suggestions;

protected:
  void open();
  void setup_db();
};

#endif // DB_H