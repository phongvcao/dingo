/* 
 * dingo-sqlite3.h
 *
 *  Copyright (C) 2011-2013 Phong V. Cao <phongvcao@phongvcao.com>
 * 
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation; either version 3 of 
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA."
 *
 */

#ifndef INCLUDED_DINGO_SQLITE3
#define INCLUDED_DINGO_SQLITE3

#ifndef INCLUDED_SQLITE3_H
#define INCLUDED_SQLITE3_H
#include <sqlite3.h>
#endif //INCLUDED_SQLITE3_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  namespace SQLite {
    enum ResultCode {
      OK = 0,
      ERROR = 1,
      ROW = 100,
      DONE = 101
    };
    
    enum Limit {
      LIMIT_VARIABLE_NUMBER = 999
    };
  }

  class SQLite3 {
    public:
      SQLite3();
      virtual ~SQLite3();
      
      //file read
      bool open(const Glib::ustring& db_name);
      void close();
      
      //Wrapper for prepare & execute statement steps
      void prepareStatement(const Glib::ustring& exec_str);
      void finalizeStatement();
      void resetStatement();
      void clearStatementBindings();
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
