#include "dingo-converter.h"
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

Glib::ustring Dingo::Converter::durationToString(gint64 duration) {
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

Glib::ustring durationToString(sqlite3_int64 duration) {
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

Glib::ustring Dingo::Converter::filesizeToString(gint64 filesize) {
  double temp_filesize = filesize;
  std::stringstream ss;
  
  ss.setf(std::ios::fixed, std::ios::floatfield);
  ss.precision(2);
  
  if (filesize >= 1024 * 1024 * 1024) {
    temp_filesize = temp_filesize / (1024 * 1024 * 1024);
    
    ss << temp_filesize << " GB";
  }
  
  else {
    if (filesize >= 1024 * 1024) {
      temp_filesize = temp_filesize / (1024 * 1024);
      
      ss << temp_filesize << " MB";
    }
    
    else {
      if (filesize >= 1024) {
        temp_filesize = temp_filesize / 1024;
        
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

Glib::ustring filesizeToString(sqlite3_int64 filesize) {
  double temp_filesize = filesize;
  std::stringstream ss;
  
  ss.setf(std::ios::fixed, std::ios::floatfield);
  ss.precision(2);
  
  if (filesize >= 1024 * 1024 * 1024) {
    temp_filesize = temp_filesize / (1024 * 1024 * 1024);
    
    ss << temp_filesize << " GB";
  }
  
  else {
    if (filesize >= 1024 * 1024) {
      temp_filesize = temp_filesize / (1024 * 1024);
      
      ss << temp_filesize << " MB";
    }
    
    else {
      if (filesize >= 1024) {
        temp_filesize = temp_filesize / 1024;
        
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

Glib::ustring Dingo::Converter::intToString(int value) {
  std::stringstream ss;
  
  ss << value;
  
  Glib::ustring ustring(ss.str());
  
  return ustring;
}

int Dingo::Converter::stringToInt(Glib::ustring value) {
  return atoi(value.c_str());
}
