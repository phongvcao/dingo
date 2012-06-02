#include "dingo-tagreader.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

#ifndef INCLUDED_UNINSTD_H
#define INCLUDED_UNINSTD_H
#include <unistd.h>
#endif //INCLUDED_UNINSTD_H

//CONSTRUCTORS - DESTRUCTOR
Dingo::TagReader::TagReader() :
  clocktime(10000000000)
{
}

Dingo::TagReader::~TagReader() {

}

//READER
void Dingo::TagReader::read(Glib::ustring fileuri) {
  d_fileuri = fileuri;

  Glib::RefPtr<Gst::Discoverer> d_discoverer = Gst::Discoverer::create(clocktime);
  
  d_filepath = Glib::filename_from_uri(d_fileuri);
  
  Glib::RefPtr<Gst::DiscovererInfo> d_discovererinfo = d_discoverer->discover_uri(fileuri);
  
  d_taglist = d_discovererinfo->get_tags();
}

//GETTERS
gint64 Dingo::TagReader::getSize() {
  Glib::RefPtr<Gio::File> f = Gio::File::create_for_uri(d_fileuri);
  
  Glib::RefPtr<Gio::FileInfo> f_info = f->query_info("standard::size");
  
  return f_info->get_size();
}

Glib::ustring Dingo::TagReader::getFileURI() {
  return d_fileuri;
}

Glib::ustring Dingo::TagReader::getFileName() {
  return Glib::path_get_basename(d_fileuri);
}

unsigned int Dingo::TagReader::getTrackNumber() {
  Glib::Value<unsigned int> temp_track_number;

  if (d_taglist.get_value(Gst::TAG_TRACK_NUMBER, temp_track_number)) {
    return temp_track_number.get();
  }
  
  else {
    temp_track_number.init(temp_track_number.value_type());
    return 0;
  }
}

Glib::ustring Dingo::TagReader::getTitle() {
  Glib::Value<Glib::ustring> temp_title;
  
  if (d_taglist.get_value(Gst::TAG_TITLE, temp_title)) {
    return temp_title.get();
  }
  
  else {
    temp_title.init(temp_title.value_type());
    return Glib::path_get_basename(d_fileuri);
  }
}

Glib::ustring Dingo::TagReader::getArtist() {
  Glib::Value<Glib::ustring> temp_artist;

  if (d_taglist.get_value(Gst::TAG_ARTIST, temp_artist)) {
    return temp_artist.get();
  }
  
  else {
    temp_artist.init(temp_artist.value_type());
    return "Unknown Artist";
  }
}

Glib::ustring Dingo::TagReader::getAlbum() {
  Glib::Value<Glib::ustring> temp_album;

  if (d_taglist.get_value(Gst::TAG_ALBUM, temp_album)) {
    return temp_album.get();
  }
  
  else {
    temp_album.init(temp_album.value_type());
    return "Unknown Album";
  }
}

Glib::ustring Dingo::TagReader::getGenre() {
  Glib::Value<Glib::ustring> temp_genre;  

  if (d_taglist.get_value(Gst::TAG_GENRE, temp_genre)) {
    return temp_genre.get();
  }
  
  else {
    temp_genre.init(temp_genre.value_type());
    return "Unknown Genre";
  }
}

gint64 Dingo::TagReader::getDuration() {
  Gst::Format fmt = Gst::FORMAT_TIME;
  gint64 dur = 0;
  
  Glib::RefPtr<Gst::PlayBin2> playbin = Gst::PlayBin2::create("playbin");
  
  if (!playbin) {
    std::cout << "Can not create playbin" << std::endl;
  }
  
  playbin->property_uri() = d_fileuri;
  
  playbin->set_state(Gst::STATE_PAUSED);
  
  usleep(50000);
  
  playbin->query_duration(fmt, dur);
  
  playbin->set_state(Gst::STATE_NULL);
  
  playbin.reset();
 
  std::cout << "Duration (Raw): " << dur << std::endl;
  std::cout << Gst::TagList::get_description("duration") << std::endl;
  
  return dur / Gst::SECOND;
}

unsigned int Dingo::TagReader::getYear() {
  Glib::Value<Glib::Date> temp_date;

  if (d_taglist.get_value(Gst::TAG_DATE, temp_date)) {
    return temp_date.get().get_year();
  }
  
  else {
    temp_date.init(temp_date.value_type());
    return 0;
  }
}

unsigned int Dingo::TagReader::getBitrate() {
  Glib::Value<unsigned int> temp_bitrate;
  
  if (d_taglist.get_value(Gst::TAG_BITRATE, temp_bitrate)) {
    return temp_bitrate.get();
  }
  
  else {
    temp_bitrate.init(temp_bitrate.value_type());
    return 0;
  }
}

std::string Dingo::TagReader::getDateAdded() {
  time_t rawtime;
  struct tm * timeinfo;
  std::string s = "Unknown Date";
  
  time(&rawtime);
  
  timeinfo = localtime(&rawtime);
  
  s = asctime(timeinfo);

  return s;
}

//SETTERS 
void Dingo::TagReader::setTitle(const Glib::ustring title) {
  Glib::Value<Glib::ustring> new_title;
  
  new_title.set(title);

  d_taglist.add_value(Gst::TAG_TITLE, new_title, Gst::TAG_MERGE_REPLACE);
}

void Dingo::TagReader::setArtist(const Glib::ustring artist) {
  Glib::Value<Glib::ustring> new_artist;
  
  new_artist.set(artist);
  
  d_taglist.add_value(Gst::TAG_ARTIST, new_artist, Gst::TAG_MERGE_REPLACE_ALL);
}
      
void Dingo::TagReader::setAlbum(const Glib::ustring album) {
  Glib::Value<Glib::ustring> new_album;
  
  new_album.set(album);
  
  d_taglist.add_value(Gst::TAG_ALBUM, new_album, Gst::TAG_MERGE_REPLACE_ALL);
}

void Dingo::TagReader::setGenre(const Glib::ustring genre) {
  Glib::Value<Glib::ustring> new_genre;
  
  new_genre.set(genre);
  
  d_taglist.add_value(Gst::TAG_GENRE, new_genre, Gst::TAG_MERGE_REPLACE_ALL);
}

void Dingo::TagReader::setYear(unsigned int year) {
  Glib::Value<Glib::Date> cur_date;
  Glib::Date new_date;
  
  d_taglist.get_value(Gst::TAG_DATE, cur_date);
  
  new_date.set_year(year);
  
  cur_date.set(new_date);
  
  d_taglist.add_value(Gst::TAG_DATE, cur_date, Gst::TAG_MERGE_REPLACE_ALL);
}

void Dingo::TagReader::setTrackNumber(unsigned int track) {
  Glib::Value<unsigned int> new_track;
  
  new_track.set(track);
  
  d_taglist.add_value(Gst::TAG_TRACK_NUMBER, new_track, Gst::TAG_MERGE_REPLACE_ALL);
}
