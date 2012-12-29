#ifndef INCLUDED_DINGO_SETTINGSREADER_H
#define INCLUDED_DINGO_SETTINGSREADER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_LIBXMLPP_H
#define INCLUDED_LIBXMLPP_H
#include <libxml++/libxml++.h>
#endif //INCLUDED_LIBXMLPP_H

#ifndef INCLUDED_SQLITE3_H
#define INCLUDED_SQLITE3_H
#include <sqlite3.h>
#endif //INCLUDED_SQLITE3_H

#ifndef INCLUDED_MEMORY
#define INCLUDED_MEMORY
#include <memory>
#endif //INCLUDED_MEMORY

#include "dingo-header.h"

namespace Dingo {
  class SettingsReader {
    public:
      SettingsReader();
      virtual ~SettingsReader();
      
      //PUBLIC METHODS - READ
      bool open(Glib::ustring input_fileuri);
      
      //PUBLIC ACCESSORS
      RepeatMode getRepeat();
      PlayOrder getPlayOrder();
      double getVolume();
      int getSortColumnID();
      Gtk::SortType getSortType();
      bool getSubtitleShow();
      Glib::ustring getSubtitleFontDesc();
      bool getVisible(Glib::ustring widget_name);
      bool getActive(Glib::ustring toggle_name);
      int getWidth(Glib::ustring widget_name);
      int getHeight(Glib::ustring widget_name);
    
    private:
      //DATA MEMBERS
      std::auto_ptr<xmlpp::TextReader> d_textreader;
      
      //<repeat> values
      RepeatMode d_read_repeat;
      
      //<playorder> values
      PlayOrder d_read_playorder;
      
      //<volume> values
      double d_read_volume;
      
      //<sort> values
      int d_read_sort_column_id;
      Gtk::SortType d_read_sort_type;
      
      //<subtitle> values
      bool d_read_subtitle_show;
      Glib::ustring d_read_font_desc;
      
      //<visible> values
      bool d_read_playlistbrowser_visible;
      bool d_read_trackbrowser_visible;
      bool d_read_playbackstatus_visible;
      bool d_read_tracknumber_visible;
      bool d_read_tracktitle_visible;
      bool d_read_trackgenre_visible;
      bool d_read_trackartist_visible;
      bool d_read_trackalbum_visible;
      bool d_read_trackyear_visible;
      bool d_read_tracktime_visible;
      bool d_read_trackfileextension_visible;
      bool d_read_trackbitrate_visible;
      bool d_read_genrefilter_visible;
      bool d_read_artistfilter_visible;
      bool d_read_yearfilter_visible;
      bool d_read_albumfilter_visible;
      
      //<active> values
      bool d_read_home_active;
      bool d_read_nowplaying_active;
      bool d_read_playqueue_active;
      bool d_read_audioonly_active;
      bool d_read_videoonly_active;
      
      //<width> values
      int d_read_topwindow_width;
      int d_read_playlisthpaned_width;
      int d_read_genrehpaned_width;
      int d_read_artisthpaned_width;
      int d_read_yearhpaned_width;
      int d_read_tracktitle_width;
      int d_read_trackgenre_width;
      int d_read_trackartist_width;
      int d_read_trackalbum_width;
      int d_read_trackyear_width;
      
      //<height> values
      int d_read_topwindow_height;
      int d_read_topvpaned_height;
      
      /* ============================ */
      /* CHECK EMPTINESS */      
      //<repeat> values
      bool d_got_repeat;
      
      //<playorder> values
      bool d_got_playorder;
      
      //<volume> values
      bool d_got_volume;
      
      //<sort> values
      bool d_got_sort_column_id;
      bool d_got_sort_type;
      
      //<subtitle> values
      bool d_got_subtitle_show;
      bool d_got_font_desc;
      
      //<visible> values
      bool d_got_playlistbrowser_visible;
      bool d_got_trackbrowser_visible;
      bool d_got_playbackstatus_visible;
      bool d_got_tracknumber_visible;
      bool d_got_tracktitle_visible;
      bool d_got_trackgenre_visible;
      bool d_got_trackartist_visible;
      bool d_got_trackalbum_visible;
      bool d_got_trackyear_visible;
      bool d_got_tracktime_visible;
      bool d_got_trackfileextension_visible;
      bool d_got_trackbitrate_visible;
      bool d_got_genrefilter_visible;
      bool d_got_artistfilter_visible;
      bool d_got_yearfilter_visible;
      bool d_got_albumfilter_visible;
      
      //<active> values
      bool d_got_home_active;
      bool d_got_nowplaying_active;
      bool d_got_playqueue_active;
      bool d_got_audioonly_active;
      bool d_got_videoonly_active;
      
      //<width> values
      bool d_got_topwindow_width;
      bool d_got_playlisthpaned_width;
      bool d_got_genrehpaned_width;
      bool d_got_artisthpaned_width;
      bool d_got_yearhpaned_width;
      bool d_got_tracktitle_width;
      bool d_got_trackgenre_width;
      bool d_got_trackartist_width;
      bool d_got_trackalbum_width;
      bool d_got_trackyear_width;
      
      //<height> values
      bool d_got_topwindow_height;
      bool d_got_topvpaned_height;
  };
}

#endif //INCLUDED_DINGO_SETTINGSREADER_H
