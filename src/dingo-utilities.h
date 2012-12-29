/* 
 * dingo-utilities.h
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

#ifndef INCLUDED_DINGO_UTILITIES_H
#define INCLUDED_DINGO_UTILITIES_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_SQLITE3_H
#define INCLUDED_SQLITE3_H
#include <sqlite3.h>
#endif //INCLUDED_SQLITE3_H

namespace Dingo {
  class Utilities {
    public:
      //BOOL TO STRING
      static Glib::ustring boolToString(bool value);
      
      //STRING TO BOOL
      static bool stringToBool(Glib::ustring input_str);
    
      //NUMERIC TO STRING
      static Glib::ustring tracksToString(int tracks_count);
      static Glib::ustring durationToString(gint64 duration, bool verbose = false);
      static Glib::ustring durationToString(sqlite3_int64 duration);
      static Glib::ustring filesizeToString(gint64 filesize, bool unit_byte = true);
      static Glib::ustring filesizeToString(sqlite3_int64 filesize, bool unit_byte = true);
      static Glib::ustring intToString(int value);
      static Glib::ustring GInt64ToString(gint64 value);     
      static Glib::ustring doubleToString(double value);
      
      //STRING TO NUMERIC
      static sqlite3_int64 stringToSQLite3Int64(const Glib::ustring& input_value);
      static int stringToInt(const Glib::ustring& value);
      static gint64 stringToGInt64(const Glib::ustring& input_str);
      static int bitrateStringToInt(Glib::ustring value);
      static double stringToDouble(const Glib::ustring& value);
      
      //STRING TO STRING
      static Glib::ustring decodeToEntityString(Glib::ustring input_str);
      static Glib::ustring encodeFromEntityString(Glib::ustring input_str);
      static Glib::ustring highlightString(Glib::ustring input_str);
      static Glib::ustring unhighlightString(Glib::ustring input_str);
      static Glib::ustring toLowerCase(Glib::ustring input_str);
      static Glib::ustring toUpperCase(Glib::ustring input_str);
      static Glib::ustring removeFirstHighlightedSubStr(Glib::ustring input_str);
      static Glib::ustring getBeforeCommaString(Glib::ustring input_str);
      static Glib::ustring getAfterCommaString(Glib::ustring input_str);
      static std::wstring stringToWString(Glib::ustring input_str);
      static Glib::ustring wstringToString(std::wstring input_wstr);
      static Glib::ustring addFileExtension(Glib::ustring input_uri, Glib::ustring input_ext);
      static bool matchFileWithExtension(Glib::ustring input_uri, Glib::ustring ext_with_dot);
      
      //NUMERIC TO NUMERIC
      static int countIntDigits(int value);
      static int doubleToInt(double value);
      static int randomInRange(int min, int max);
      static void locatePopupWidgetPosition(int& widget_x, int& widget_y, int widget_width, int widget_height, const Glib::RefPtr<Gdk::Screen>& screen, const Gdk::Rectangle& statusicon_rectangle, const Gtk::Orientation& traybar_orientation);
      
      //FILE TO BOOL
      static bool queryFileExists(Glib::ustring file_uri);
      
      //FILE UTILITIES
      static void createConfigFolders();
      static void createSettingsFolders();
      static void createCacheCoverArtFolders();
      static bool comparePixbufs(const Glib::RefPtr<Gdk::Pixbuf>& org_pixbuf, const Glib::RefPtr<Gdk::Pixbuf>& compare_pixbuf);
      static Glib::ustring getPixbufFileURI(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf, int track_id);
  };
}

#endif //INCLUDED_DINGO_UTILITIES_H
