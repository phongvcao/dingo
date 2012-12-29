/* 
 * dingo-tagreader.cc
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA."
 *
 */

#include "dingo-tagreader.h"
#include "dingo-utilities.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

//CONSTRUCTORS - DESTRUCTOR
Dingo::TagReader::TagReader() {

}

Dingo::TagReader::~TagReader() {

}

//READER
bool Dingo::TagReader::read(Glib::ustring fileuri) {
  d_fileuri = fileuri;
  
  bool return_value = d_mediainfo.Open(Dingo::Utilities::stringToWString(Glib::filename_from_uri(d_fileuri)));
  
  if (return_value) {
    Glib::ustring audio_format_list = Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("Audio_Format_List")));
    
    audio_format_list.empty() ? return_value = false : return_value = true;
  }
  
  return return_value;
}

//GETTERS
gint64 Dingo::TagReader::getFileSize() {
  return Dingo::Utilities::stringToGInt64(Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("FileSize"))));
}

Glib::ustring Dingo::TagReader::getFileURI() {
  return Glib::locale_to_utf8(d_fileuri);
}

Glib::ustring Dingo::TagReader::getFileName() {
  return Glib::locale_to_utf8(Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("FileName"))));
}

Glib::ustring Dingo::TagReader::getFileExtension() {
  return Glib::locale_to_utf8("." + Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("FileExtension"))));
}

int Dingo::TagReader::getTrackNumber() {
  return Dingo::Utilities::stringToInt(Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("Track/Position"))));
}

Glib::ustring Dingo::TagReader::getTitle() {
  Glib::ustring title_value = Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("Title")));
  
  if (!title_value.empty()) {
    return Glib::locale_to_utf8(title_value);  
  }
  
  else {
    return Glib::locale_to_utf8(Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("FileName"))));
  }
}

Glib::ustring Dingo::TagReader::getArtist() {
  Glib::ustring artist_value = Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("Performer")));
  
  if (!artist_value.empty()) {
    return Glib::locale_to_utf8(artist_value);
  }
  
  else {
    return Glib::locale_to_utf8("Unknown Artist");
  }
}

Glib::ustring Dingo::TagReader::getAlbum() {
  Glib::ustring album_value = Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("Album")));
  
  if (!album_value.empty()) {
    return Glib::locale_to_utf8(album_value);
  }
  
  else {
    return Glib::locale_to_utf8("Unknown Album");
  }
}

Glib::ustring Dingo::TagReader::getGenre() {
  Glib::ustring genre_value = Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("Genre")));
  
  if (!genre_value.empty()) {
    return Glib::locale_to_utf8(genre_value);
  }
  
  else {
    return Glib::locale_to_utf8("Unknown Genre");
  }
}

gint64 Dingo::TagReader::getDuration() {
  gint64 dur = Dingo::Utilities::stringToGInt64(Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("Duration"))));
  
  return dur / 1000;
}

Glib::ustring Dingo::TagReader::getYear() {
  Glib::ustring year_value = Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("Recorded_Date")));
  
  if (!year_value.empty()) {
    return Glib::locale_to_utf8(year_value);
  }
  
  else {
    return Glib::locale_to_utf8("Unknown Year");
  }
}

Glib::ustring Dingo::TagReader::getComment() {
  Glib::ustring comment_value = Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("Comment")));
  
  if (!comment_value.empty()) {
    return Glib::locale_to_utf8(comment_value);
  }
  
  else {
    return Glib::locale_to_utf8("");
  }
}

Glib::ustring Dingo::TagReader::getLyrics() {
  Glib::ustring lyrics_value = Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("Lyrics")));
  
  if (!lyrics_value.empty()) {
    return Glib::locale_to_utf8(lyrics_value);
  }
  
  else {
    return Glib::locale_to_utf8("");
  }
}

int Dingo::TagReader::getBitrate() {
  return Dingo::Utilities::stringToInt(Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("OverallBitRate")))) / 1000;
}

Glib::ustring Dingo::TagReader::getMIMEType() {
  Glib::ustring video_format_list = Dingo::Utilities::wstringToString(d_mediainfo.Get(MediaInfoLib::Stream_General, 0, Dingo::Utilities::stringToWString("Video_Format_List")));

  if (!video_format_list.empty()) {
    return Glib::locale_to_utf8("video/*");
  }
  
  else {
    return Glib::locale_to_utf8("audio/*");
  }
}

Glib::ustring Dingo::TagReader::getDateAdded() {
  time_t rawtime;
  struct tm * timeinfo;
  std::string s = "Unknown Date";
  
  time(&rawtime);
  
  timeinfo = localtime(&rawtime);
  
  s = asctime(timeinfo);

  s.erase(s.size() - 1, 1);

  return Glib::locale_to_utf8(s);
}
