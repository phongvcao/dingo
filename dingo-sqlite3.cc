#include "dingo-sqlite3.h"

Dingo::SQLite3::SQLite3() :
  index(-1),
  stmt(NULL)
{
  sqlite3_initialize();
}

Dingo::SQLite3::SQLite3(const Glib::ustring& db_name) :
  index(-1),
  stmt(NULL)
{
  sqlite3_initialize();

  int result = sqlite3_open_v2(db_name.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  
  if (result != SQLITE_OK) {
    std::cerr << "Can't open database: ";
    std::cout << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
  }
}

Dingo::SQLite3::~SQLite3() {
  sqlite3_close(db);
}

void Dingo::SQLite3::open(const Glib::ustring& db_name) {
  int result = sqlite3_open_v2(db_name.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  
  if (result != SQLITE_OK) {
    std::cerr << "Can't open database: ";
    std::cout << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
  }
}

void Dingo::SQLite3::close() {
  sqlite3_close(db);
}

void Dingo::SQLite3::prepareStatement(const Glib::ustring& exec_str) {
  int result;
  
  result = sqlite3_prepare_v2(db, exec_str.c_str(), -1, &stmt, NULL);
  
  if (result != SQLITE_OK) {
    std::cout << sqlite3_errmsg(db) << std::endl;
    std::cout << "Error preparing statement" << std::endl;
  }
}

void Dingo::SQLite3::finalizeStatement() {
  sqlite3_finalize(stmt);
}

int Dingo::SQLite3::stepStatement() {
  return sqlite3_step(stmt);
}

void Dingo::SQLite3::bindParaText(const Glib::ustring& para_name, const Glib::ustring& text_value) {
  int index = sqlite3_bind_parameter_index(stmt, para_name.c_str());
  
  sqlite3_bind_text(stmt, index, text_value.c_str(), -1, SQLITE_STATIC);
}

void Dingo::SQLite3::bindParaInt(const Glib::ustring& para_name, int int_value) {
  int index = sqlite3_bind_parameter_index(stmt, para_name.c_str());
  sqlite3_bind_int(stmt, index, int_value);
}

void Dingo::SQLite3::bindParaInt64(const Glib::ustring& para_name, sqlite3_int64 int_value) {
  int index = sqlite3_bind_parameter_index(stmt, para_name.c_str());
  sqlite3_bind_int64(stmt, index, int_value);
  
}

int Dingo::SQLite3::getIntValue(int col_idx) {
  return sqlite3_column_int(stmt, col_idx);
}

gint64 Dingo::SQLite3::getInt64Value(int col_idx) {
  return sqlite3_column_int64(stmt, col_idx);
}

//revise needed: conversion from 'const unsigned char*' to 'const char*'
Glib::ustring Dingo::SQLite3::getStringValue(int col_idx) {
  Glib::ustring return_string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, col_idx)));

  return return_string;
}

void Dingo::SQLite3::quickExecute(const Glib::ustring& exec_str) {
  int result;
  char *errmsg;
  
  result = sqlite3_exec(db, exec_str.c_str(), NULL, NULL, &errmsg);
}
