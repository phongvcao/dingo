#include "dingo-utilities.h"
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <ctype.h>

#ifndef INCLUDED_GLIBMM_I18N_H
#define INCLUDED_GLIBMM_I18N_H
#include <glibmm/i18n.h>
#endif //INCLUDED_GLIBMM_I18N_H

//BOOL TO STRING
Glib::ustring Dingo::Utilities::boolToString(bool value) {
  return value ? "true" : "false";
}

bool Dingo::Utilities::stringToBool(Glib::ustring input_str) {
  return input_str == "true" ? true : false;
}

//NUMERIC TO STRING
Glib::ustring Dingo::Utilities::tracksToString(int tracks_count) {
  return Glib::ustring::compose(_("%1 tracks"), tracks_count);
}

Glib::ustring Dingo::Utilities::durationToString(gint64 duration, bool verbose) {
  int seconds = 0;
  int minutes = 0;
  int hours = 0;
  int days = 0;
  int years = 0;
  
  char secondsString[3];
  
  Glib::ustring return_string = "";
  
  years = duration / 31536000;
  days = (duration - years * 31536000) / 86400;
  hours = (duration - years * 31536000 - days * 86400) / 3600;
  minutes = (duration - years * 31536000 - days * 86400 - hours * 3600) / 60;
  seconds = duration - years * 31536000 - days * 86400 - hours * 3600 - minutes * 60;
  
  if (verbose) {
    if (years) {
      return_string += years > 1 ? Glib::ustring::compose(_("%1 years "), years) : Glib::ustring::compose(_("%1 year "), years);
    }
    
    if (days) {
      return_string += days > 1 ? Glib::ustring::compose(_("%1 days "), days) : Glib::ustring::compose(_("%1 day "), days);
    }
  
    if (hours) {
      return_string += hours > 1 ? Glib::ustring::compose(_("%1 hours "), hours) : Glib::ustring::compose(_("%1 hour "), hours);
    }
    
    return_string += minutes > 1 ? Glib::ustring::compose(_("%1 min "), minutes) : Glib::ustring::compose(_("%1 min "), minutes);
    
    return_string += seconds > 1 ? Glib::ustring::compose(_("%1 sec"), seconds) : Glib::ustring::compose(_("%1 sec"), seconds);
  }
  
  else {
    if (years) {
      return_string += Glib::ustring::compose("%1:", years);
    }
    
    if (days) {
      return_string += Glib::ustring::compose("%1:", days);
    }
    
    if (hours) {
      return_string += Glib::ustring::compose("%1:", hours);
    }
    
    return_string += Glib::ustring::compose("%1:", minutes);
    
    sprintf(secondsString, "%02i", seconds);
    
    return_string += Glib::ustring::compose("%1", secondsString);
  }
  
  return return_string;
}

Glib::ustring Dingo::Utilities::durationToString(sqlite3_int64 duration) {
  int seconds, minutes, hours;
  char secondsString[3];
  std::stringstream ss;
  
  if (duration / 3600 > 0) {
    seconds = duration % 60;
    minutes = (duration - seconds) % 60;
    hours = (duration - seconds - minutes * 60) / 60;
    
    sprintf(secondsString, "%02i", seconds);
    
    ss << hours << ":" << minutes << ":" << secondsString;
  }
  
  else {
    if (duration / 60 > 0) {
      seconds = duration % 60;
      minutes = (duration - seconds) / 60;
      
      sprintf(secondsString, "%02i", seconds);
      
      ss << minutes << ":" << secondsString;
    }
    
    else {
      seconds = duration;
      
      sprintf(secondsString, "%02i", seconds);
      
      ss << secondsString;
    }
  }
  
  Glib::ustring ustring(ss.str());
  
  return ustring;
}

Glib::ustring Dingo::Utilities::filesizeToString(gint64 filesize, bool is_unit_byte) {
  double temp_filesize = filesize;
  std::stringstream ss;
  int unit_divider = 1024;
  
  if (is_unit_byte) {
    unit_divider = 1000;
  }
  
  ss.setf(std::ios::fixed, std::ios::floatfield);
  ss.precision(2);
  
  if (filesize >= unit_divider * unit_divider * unit_divider) {
    temp_filesize = temp_filesize / (unit_divider * unit_divider * unit_divider);
    
    ss << temp_filesize << " GB";
  }
  
  else {
    if (filesize >= unit_divider * unit_divider) {
      temp_filesize = temp_filesize / (unit_divider * unit_divider);
      
      ss << temp_filesize << " MB";
    }
    
    else {
      if (filesize >= unit_divider) {
        temp_filesize = temp_filesize / unit_divider;
        
        ss << temp_filesize << " KB";
      }
      
      else {
        ss << temp_filesize << " B";
      }
    }
  }
  
  Glib::ustring ustring(ss.str());
  
  return ustring;
}

Glib::ustring Dingo::Utilities::filesizeToString(sqlite3_int64 filesize, bool is_unit_byte) {
  double temp_filesize = filesize;
  std::stringstream ss;
  int unit_divider = 1024;
  
  if (is_unit_byte) {
    unit_divider = 1000;
  }
  
  ss.setf(std::ios::fixed, std::ios::floatfield);
  ss.precision(2);
  
  if (filesize >= unit_divider * unit_divider * unit_divider) {
    temp_filesize = temp_filesize / (unit_divider * unit_divider * unit_divider);
    
    ss << temp_filesize << " GB";
  }
  
  else {
    if (filesize >= unit_divider * unit_divider) {
      temp_filesize = temp_filesize / (unit_divider * unit_divider);
      
      ss << temp_filesize << " MB";
    }
    
    else {
      if (filesize >= unit_divider) {
        temp_filesize = temp_filesize / unit_divider;
        
        ss << temp_filesize << " KB";
      }
      
      else {
        ss << temp_filesize << " B";
      }
    }
  }
  
  Glib::ustring ustring(ss.str());
  
  return ustring;
}

Glib::ustring Dingo::Utilities::intToString(int value) {
  std::stringstream ss;
  
  ss << value;
  
  Glib::ustring ustring(ss.str());
  
  return ustring;
}

Glib::ustring Dingo::Utilities::GInt64ToString(gint64 value) {
  std::stringstream ss;
  
  ss << value;
  
  Glib::ustring ustring(ss.str());
  
  return ustring;
}

Glib::ustring Dingo::Utilities::doubleToString(double value) {
  std::stringstream ss;
  
  ss << value;
  
  Glib::ustring ustring(ss.str());
  
  return ustring;
}

//STRING TO NUMERIC
sqlite3_int64 Dingo::Utilities::stringToSQLite3Int64(const Glib::ustring& input_value) {
  sqlite3_int64 return_value = atoi(input_value.c_str());
  
  return return_value;
}

int Dingo::Utilities::stringToInt(const Glib::ustring& value) {
  return atoi(value.c_str());
}

gint64 Dingo::Utilities::stringToGInt64(const Glib::ustring& input_str) {
  return atoll(input_str.c_str());
}

int Dingo::Utilities::bitrateStringToInt(Glib::ustring value) {
  Glib::ustring::size_type unit_index = value.find(" kbps");
  
  while (unit_index != Glib::ustring::npos) {
    value.erase(unit_index, 5);
    
    unit_index = value.find(" kbps");
  }
  
  return Dingo::Utilities::stringToInt(value);
}

double Dingo::Utilities::stringToDouble(const Glib::ustring& value) {
  std::istringstream i_str_stream(value);
  
  double return_value;
  
  if (!(i_str_stream >> return_value)) {
    return 0;
  }
  
  return return_value;
}

//STRING TO STRING
Glib::ustring Dingo::Utilities::encodeFromEntityString(Glib::ustring input_str) {
  Glib::ustring::size_type index = 0;
  
  while (true) {
    index = input_str.find("&", index);
    
    if (index == Glib::ustring::npos) {
      break;
    }
    
    input_str.replace(index, 1, "&amp;");
    
    ++index;
  }

  return input_str;
}

Glib::ustring Dingo::Utilities::decodeToEntityString(Glib::ustring input_str) {
  Glib::ustring::size_type index = 0;
  
  while (true) {
    index = input_str.find("&amp;", index);
    
    if (index == Glib::ustring::npos) {
      break;
    }
    
    input_str.replace(index, 5, "&");
    
    ++index;
  }

  return input_str;
}

Glib::ustring Dingo::Utilities::highlightString(Glib::ustring input_str) {
  return "<b>" + input_str + "</b>";
}

Glib::ustring Dingo::Utilities::unhighlightString(Glib::ustring input_str) {
  Glib::ustring::size_type open_bold_tag_pos = input_str.find("<b>");
  
  while (open_bold_tag_pos != Glib::ustring::npos) {
    input_str.erase(open_bold_tag_pos, 3);
    
    open_bold_tag_pos = input_str.find("<b>");
  }
  
  Glib::ustring::size_type close_bold_tag_pos = input_str.rfind("</b>");
  
  while (close_bold_tag_pos != Glib::ustring::npos) {
    input_str.erase(close_bold_tag_pos, 4);
    
    close_bold_tag_pos = input_str.find("</b>");
  }
  
  return input_str;
}

Glib::ustring Dingo::Utilities::toLowerCase(Glib::ustring input_str) {
  Glib::ustring return_str;

  for (Glib::ustring::iterator iter = input_str.begin(); iter != input_str.end(); ++iter) {
    char lower_letter = (char)(tolower(*iter));
  
    return_str.push_back(lower_letter);
  }
  
  return return_str;
}

Glib::ustring Dingo::Utilities::toUpperCase(Glib::ustring input_str) {
  Glib::ustring return_str;
  
  for (Glib::ustring::iterator iter = input_str.begin(); iter != input_str.end(); ++iter) {
    char upper_letter = (char)(toupper(*iter));
  
    return_str.push_back(upper_letter);
  }
  
  return return_str;
}

Glib::ustring Dingo::Utilities::removeFirstHighlightedSubStr(Glib::ustring input_str) {
  Glib::ustring::size_type close_bold_tag_pos = input_str.find("</b>");
  
  if (close_bold_tag_pos != Glib::ustring::npos) {
    return input_str.substr(close_bold_tag_pos + 4);
  }
  
  else {
    return input_str;
  }
}

Glib::ustring Dingo::Utilities::getBeforeCommaString(Glib::ustring input_str) {
  Glib::ustring::size_type comma_pos = input_str.find(", ");
  
  if (comma_pos != Glib::ustring::npos) {
    return input_str.substr(0, comma_pos);
  }
  
  else {
    return "Unreadable";
  }
}

Glib::ustring Dingo::Utilities::getAfterCommaString(Glib::ustring input_str) {
  Glib::ustring::size_type comma_pos = input_str.find(", ");
  
  if (comma_pos != Glib::ustring::npos) {
    return input_str.substr(comma_pos + 2);
  }
  
  else {
    return "Unreadable";
  }
}

std::wstring Dingo::Utilities::stringToWString(Glib::ustring input_str) {
  std::wstring return_wstring(input_str.length(), L' ');
  
  std::copy(input_str.begin(), input_str.end(), return_wstring.begin());
  
  return return_wstring;
}

Glib::ustring Dingo::Utilities::wstringToString(std::wstring input_wstr) {
  return Glib::ustring::format(input_wstr);
}

Glib::ustring Dingo::Utilities::addFileExtension(Glib::ustring input_uri, Glib::ustring input_ext) {
  Glib::ustring::size_type ext_dot_pos = input_uri.rfind(input_ext);
  
  if (ext_dot_pos == Glib::ustring::npos) {
    input_uri += input_ext;
  }
  
  else {
    if (ext_dot_pos + input_ext.size() != input_uri.size()) {
      input_uri += input_ext;
    }
  }
  
  return input_uri;
}

bool Dingo::Utilities::matchFileWithExtension(Glib::ustring input_uri, Glib::ustring ext_with_dot) {
  if (input_uri.substr(input_uri.size() - ext_with_dot.size()) == ext_with_dot) {
    return true;
  }
  
  else {
    return false;
  }
}

//NUMERIC TO NUMERIC
int Dingo::Utilities::countIntDigits(int value) {
  int digits_count = 0;
  
  while (value) {
    value /= 10;
    digits_count++;
  }
  
  return digits_count;
}

int Dingo::Utilities::doubleToInt(double value) {
  value += 0.5;
  
  int return_value = static_cast<int>(value);
  
  return return_value;
}

int Dingo::Utilities::randomInRange(int min, int max) {
  int base_random = rand();
  
  if (RAND_MAX == base_random) {
    return randomInRange(min, max);
  }
  
  int range = max - min;
  int remainder = RAND_MAX % range;
  int bucket = RAND_MAX / range;
      
  if (base_random < RAND_MAX - remainder) {
    return min + base_random / bucket;
  } 
  
  else {
    return randomInRange(min, max);
  }
}

void Dingo::Utilities::locatePopupWidgetPosition(int& widget_x, int& widget_y, int widget_width, int widget_height, const Glib::RefPtr<Gdk::Screen>& screen, const Gdk::Rectangle& statusicon_rectangle, const Gtk::Orientation& traybar_orientation) {
  int statusicon_center_to_right_edge = screen->get_width() - statusicon_rectangle.get_x() - statusicon_rectangle.get_width() / 2;
  int statusicon_center_to_left_edge = statusicon_rectangle.get_x() + statusicon_rectangle.get_width() / 2;
  
  int statusicon_center_to_bottom_edge = screen->get_height() - statusicon_rectangle.get_y() - statusicon_rectangle.get_height() / 2;
  int statusicon_center_to_top_edge = statusicon_rectangle.get_y() + statusicon_rectangle.get_height() / 2;
  
  int screen_edges_offset = 10;
  
  //if the traybar is horizontal
  if (traybar_orientation == Gtk::ORIENTATION_HORIZONTAL) {
    //first calculate widget_x
    if (statusicon_center_to_right_edge <= statusicon_center_to_left_edge) {
      //the statusicon is on the right side of the traybar
      if (statusicon_center_to_right_edge - screen_edges_offset <= widget_width / 2) {
        widget_x = screen->get_width() - screen_edges_offset - widget_width;
      }
      
      else {
        widget_x = statusicon_rectangle.get_x() + statusicon_rectangle.get_width() / 2 - widget_width / 2;
      }
    }
  
    else {
      //the statusicon is on the left side of the traybar
      if (statusicon_center_to_left_edge - screen_edges_offset <= widget_width / 2) {
        widget_x = screen_edges_offset;
      }
      
      else {
        widget_x = statusicon_rectangle.get_x() + statusicon_rectangle.get_width() / 2 - widget_width / 2;
      }
    }
  
    //then calculate widget_y
    if (statusicon_center_to_bottom_edge <= statusicon_center_to_top_edge) {
      //the traybar is on the bottom half of the screen
      widget_y = statusicon_rectangle.get_y() - screen_edges_offset - widget_height;
    }
    
    else {
      //the traybar is on the top half of the screen
      widget_y = statusicon_rectangle.get_y() + statusicon_rectangle.get_height() + screen_edges_offset;
    }
  }
  
  //if the traybar is vertical
  else {
    //first calculate widget_y
    if (statusicon_center_to_bottom_edge <= statusicon_center_to_top_edge) {
      //the statusicon is on the bottom of the traybar
      if (statusicon_center_to_bottom_edge - screen_edges_offset <= widget_height / 2) {
        widget_y = screen->get_height() - screen_edges_offset - widget_height;
      }
      
      else {
        widget_y = statusicon_rectangle.get_y() + statusicon_rectangle.get_height() / 2 - widget_height / 2;
      }
    }
    
    else {
      //the statusicon is on the top of the traybar
      if (statusicon_center_to_top_edge - screen_edges_offset <= widget_height / 2) {
        widget_y = screen_edges_offset;
      }
      
      else {
        widget_y = statusicon_rectangle.get_y() + statusicon_rectangle.get_height() / 2 - widget_height / 2;
      }
    }
    
    //then calculate widget_x
    if (statusicon_center_to_right_edge <= statusicon_center_to_left_edge) {
      //the traybar is on the right half of the screen
      widget_x = statusicon_rectangle.get_x() - screen_edges_offset - widget_width;
    }
    
    else {
      //the traybar is on the left half of the screen
      widget_x = statusicon_rectangle.get_x() + statusicon_rectangle.get_width() + screen_edges_offset;
    }
  }
}

//FILE TO BOOL
bool Dingo::Utilities::queryFileExists(Glib::ustring file_uri) {
  Glib::RefPtr<Gio::File> temp_file = Gio::File::create_for_uri(file_uri);
  
  return temp_file->query_exists();
}

void Dingo::Utilities::createConfigFolders() {
  Glib::RefPtr<Gio::File> database_dir = Gio::File::create_for_path(Glib::get_home_dir() + "/.config/dingo-0.5/database");
  
  if (!database_dir->query_exists()) {
    database_dir->make_directory_with_parents();
  }
}

void Dingo::Utilities::createSettingsFolders() {
  Glib::RefPtr<Gio::File> settings_dir = Gio::File::create_for_path(Glib::get_home_dir() + "/.config/dingo-0.5/settings");
  
  if (!settings_dir->query_exists()) {
    settings_dir->make_directory_with_parents();
  }
}

void Dingo::Utilities::createCacheCoverArtFolders() {
  Glib::RefPtr<Gio::File> cache_coverart_dir = Gio::File::create_for_path(Glib::get_home_dir() + "/.cache/dingo/cover-art");
  
  if (!cache_coverart_dir->query_exists()) {
    cache_coverart_dir->make_directory_with_parents();
  }
  
  //create the default preview_image
  Glib::RefPtr<Gdk::Pixbuf> default_preview_pixbuf = Gdk::Pixbuf::create_from_file(DATADIR"/dingo/icons/128x128/dingo.png");
  
  Glib::RefPtr<Gio::File> cached_coverart_file = Gio::File::create_for_path(Glib::get_home_dir() + "/.cache/dingo/cover-art/0.png");
  
  if (!cached_coverart_file->query_exists()) {
    default_preview_pixbuf->save(Glib::get_home_dir() + "/.cache/dingo/cover-art/0.png", "png");
  }
}

bool Dingo::Utilities::comparePixbufs(const Glib::RefPtr<Gdk::Pixbuf>& org_pixbuf, const Glib::RefPtr<Gdk::Pixbuf>& compare_pixbuf) {
  if ((org_pixbuf->get_width() != compare_pixbuf->get_width()) || (org_pixbuf->get_height() != compare_pixbuf->get_height()) || (org_pixbuf->get_n_channels() != compare_pixbuf->get_n_channels()) || (org_pixbuf->get_has_alpha() != compare_pixbuf->get_has_alpha()) || (org_pixbuf->get_bits_per_sample() != compare_pixbuf->get_bits_per_sample())) {
    return false;  
  }
  
  else {
    guint8* org_pixels;
    guint8* compare_pixels;
    int org_rowstride;
    int compare_rowstride;
    int width;
    int height;
    int bytes_per_pixel;
    int x, y;
    guint8* p_org;
    guint8* p_compare;
    int count = 0;
    
    org_pixels = org_pixbuf->get_pixels();
    compare_pixels = compare_pixbuf->get_pixels();
    
    org_rowstride = org_pixbuf->get_rowstride();
    compare_rowstride = compare_pixbuf->get_rowstride();
    
    width = org_pixbuf->get_width();
    height = org_pixbuf->get_height();
    
    bytes_per_pixel = org_pixbuf->get_n_channels();
    
    p_org = org_pixels;
    p_compare = compare_pixels;
    
    for (int i = 0; i < height; ++i) {
      for (int j = 0; j < width * bytes_per_pixel; ++j) {
        count += (*p_org++ != *p_compare++);
        
        p_org = org_pixels;
        p_compare = compare_pixels;
      }
    }
    
    if (count > 0) {
      return false;
    }
  }
  
  return true;
}

Glib::ustring Dingo::Utilities::getPixbufFileURI(const Glib::RefPtr<Gdk::Pixbuf>& pixbuf, int track_id) {
  Glib::ustring folder_path = Glib::get_home_dir() + "/.cache/dingo/cover-art";
  Glib::Dir top_dir(folder_path);
  Glib::ustring file_name = top_dir.read_name();
  
  while (!file_name.empty()) {
    if (Glib::file_test(folder_path + "/" + file_name, Glib::FILE_TEST_IS_REGULAR)) {
      Glib::RefPtr<Gdk::Pixbuf> cache_file_pixbuf = Gdk::Pixbuf::create_from_file(folder_path + "/" + file_name);
      
      if (cache_file_pixbuf) {
        if (Dingo::Utilities::comparePixbufs(pixbuf, cache_file_pixbuf)) {
          return Glib::filename_to_uri(folder_path + "/" + file_name);
        }
      }
    }
    
    file_name = top_dir.read_name();
  }
  
  pixbuf->save(folder_path + "/" + Dingo::Utilities::intToString(track_id) + ".png", "png");
    
  return Glib::filename_to_uri(folder_path + "/" + Dingo::Utilities::intToString(track_id) + ".png");
}
