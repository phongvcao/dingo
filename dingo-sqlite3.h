#ifndef INCLUDED_DINGO_SQLITE3
#define INCLUDED_DINGO_SQLITE3

#ifndef INCLUDED_SQLITE3_H
#define INCLUDED_SQLITE3_H
#include "sqlite3.h"
#endif //INCLUDED_SQLITE3_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

namespace Dingo {
  namespace SQLite {
    enum ResultCode {
      OK = 0,
      ERROR = 1,
      ROW = 100,
      DONE = 101
    };
  }

  class SQLite3 {
    public:
      SQLite3();
      SQLite3(const Glib::ustring& db_name);
      virtual ~SQLite3();
      
      //file read
      void open(const Glib::ustring& db_name);
      void close();
      
      //Wrapper for prepare & execute statement steps
      void prepareStatement(const Glib::ustring& exec_str);
      void finalizeStatement();
      int stepStatement();
      
      //Bind necessary values to the parameters
      void bindParaText(const Glib::ustring& para_name, const Glib::ustring& text_value);
      void bindParaInt(const Glib::ustring& para_name, int int_value);
      void bindParaInt64(const Glib::ustring& para_name, sqlite3_int64 int_value);
      
      //Get values of a returned row according to column_idx
      int getIntValue(int col_idx);
      gint64 getInt64Value(int col_idx);
      Glib::ustring getStringValue(int col_idx);

      //Quick execute a custom SQL Statement      
      void quickExecute(const Glib::ustring& exec_str);
      
    private:
      sqlite3_stmt *stmt;
      sqlite3 *db;
      int index;
  };
}

#endif //INCLUDED_DINGO_SQLITE3
