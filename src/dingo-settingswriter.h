#ifndef INCLUDED_DINGO_SETTINGSWRITER_H
#define INCLUDED_DINGO_SETTINGSWRITER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#include "dingo-header.h"

namespace Dingo {
  class SettingsWriter {
    public:
      SettingsWriter();
      virtual ~SettingsWriter();
      
      //PUBLIC METHODS - WRITE
      void open(Glib::ustring output_fileuri, Glib::ustring settings_name = "settings");
      
      //PUBLIC CREATORS
      void createSettings();
      
      //PUBLIC ACCESSORS
      void setRepeat(RepeatMode repeat_mode);
      void setPlayOrder(PlayOrder play_order);
      void setVolume(double volume);
      void setSortColumnID(int column_id);
      void setSortType(Gtk::SortType sort_type);
      void setSubtitleShow(bool is_show);
      void setSubtitleFontDesc(Glib::ustring font_desc);
      void setVisible(Glib::ustring widget_name, bool is_visible);
      void setActive(Glib::ustring toggle_name, bool is_active);
      void setWidth(Glib::ustring widget_name, int width);
      void setHeight(Glib::ustring widget_name, int height);
      
      //PUBLIC SAVER/WRITER
      void saveSettings();
      void writeToFile();
    
    private:
      //DATA MEMBERS - WRITE
      Glib::ustring d_write_repeat;
      Glib::ustring d_write_playorder;
      Glib::ustring d_write_volume;
      Glib::ustring d_write_sort;
      Glib::ustring d_write_subtitle;
      Glib::ustring d_write_visible;
      Glib::ustring d_write_active;
      Glib::ustring d_write_width_paneds_wins;
      Glib::ustring d_write_width_columns;
      Glib::ustring d_write_height;
      
      Glib::ustring d_whole_file_str;
      
      Glib::ustring d_output_fileuri;
  };
}

#endif //INCLUDED_DINGO_SETTINGSWRITER_H
