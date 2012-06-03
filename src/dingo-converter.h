#ifndef INCLUDED_DINGO_CONVERTER_H
#define INCLUDED_DINGO_CONVERTER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_SQLITE3_H
#define INCLUDED_SQLITE3_H
#include "sqlite3.h"
#endif //INCLUDED_SQLITE3_H

namespace Dingo {
  class Converter {
    public:
      static Glib::ustring durationToString(gint64 duration);
      static Glib::ustring durationToString(sqlite3_int64 duration);
      static Glib::ustring filesizeToString(gint64 filesize);
      static Glib::ustring filesizeToString(sqlite3_int64 filesize);
      static Glib::ustring intToString(int value);
      static int stringToInt(Glib::ustring value);
  };
}

#endif //INCLUDED_DINGO_CONVERTER_H
