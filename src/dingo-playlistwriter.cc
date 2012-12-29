/* 
 * dingo-playlistwriter.cc
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

#include "dingo-playlistwriter.h"
#include "dingo-utilities.h"

#ifndef INCLUDED_IOSTREAM
#define INCLUDED_IOSTREAM
#include <iostream>
#endif //INCLUDED_IOSTREAM

#ifndef INCLUDED_FSTREAM
#define INCLUDED_FSTREAM
#include <fstream>
#endif //INCLUDED_FSTREAM

//CONSTRUCTORS - DESTRUCTOR
Dingo::PlaylistWriter::PlaylistWriter() {

}

Dingo::PlaylistWriter::~PlaylistWriter() {

}
      
//PUBLIC METHODS - WRITE
void Dingo::PlaylistWriter::open(Glib::ustring output_fileuri, Glib::ustring playlist_name, int playlist_size) {
  d_output_fileuri = output_fileuri;
  
  d_whole_file_str = "<dingo version='0.5.1' author='Phong V. Cao' email='phongvcao@phongvcao.com' website='http://www.phongvcao.com/'>\n";
  
  d_whole_file_str += "  <playlist name='" + Dingo::Utilities::encodeFromEntityString(playlist_name) + "' type='basic'" + " size='" + Dingo::Utilities::intToString(playlist_size) + "'>\n";
}

//PUBLIC CREATORS
void Dingo::PlaylistWriter::createTrack() {
  //reset all older data
  d_write_trackartist = "";
  d_write_trackalbum = "";
  d_write_tracknumber = "0";
  d_write_tracktitle = "";
  d_write_trackgenre = "";
  d_write_trackduration = "0";
  d_write_tracksize = "0";
  d_write_trackfilename = "";
  d_write_trackfileextension = "";
  d_write_trackuri = "";
  d_write_trackyear = "0";
  d_write_trackbitrate = "0";
  d_write_trackmimetype = "";
  d_write_trackdateadded = "";
  d_write_trackcomment = "";
  d_write_tracklyrics = "";
}
 
//PUBLIC ACCESSORS     
void Dingo::PlaylistWriter::setTrackArtist(Glib::ustring track_artist) {
  d_write_trackartist = "      <trackartist>" + Dingo::Utilities::encodeFromEntityString(track_artist) + "</trackartist>\n";
}

void Dingo::PlaylistWriter::setTrackAlbum(Glib::ustring track_album) {
  d_write_trackalbum = "      <trackalbum>" + Dingo::Utilities::encodeFromEntityString(track_album) + "</trackalbum>\n";
}

void Dingo::PlaylistWriter::setTrackNumber(int track_number) {
  d_write_tracknumber = "      <tracknumber>" + Dingo::Utilities::intToString(track_number) + "</tracknumber>\n";
}

void Dingo::PlaylistWriter::setTrackTitle(Glib::ustring track_title) {
  d_write_tracktitle = "      <tracktitle>" + Dingo::Utilities::encodeFromEntityString(track_title) + "</tracktitle>\n";
}

void Dingo::PlaylistWriter::setTrackGenre(Glib::ustring track_genre) {
  d_write_trackgenre = "      <trackgenre>" + Dingo::Utilities::encodeFromEntityString(track_genre) + "</trackgenre>\n";
}

void Dingo::PlaylistWriter::setTrackDuration(gint64 track_duration) {
  d_write_trackduration = "      <trackduration>" + Dingo::Utilities::GInt64ToString(track_duration) + "</trackduration>\n";
}

void Dingo::PlaylistWriter::setTrackSize(gint64 track_size) {
  d_write_tracksize = "      <tracksize>" + Dingo::Utilities::GInt64ToString(track_size) + "</tracksize>\n";
}

void Dingo::PlaylistWriter::setTrackFileName(Glib::ustring track_filename) {
  d_write_trackfilename = "      <trackfilename>" + Dingo::Utilities::encodeFromEntityString(track_filename) + "</trackfilename>\n";
}

void Dingo::PlaylistWriter::setTrackFileExtension(Glib::ustring track_fileextension) {
  d_write_trackfileextension = "      <trackfileextension>" + track_fileextension + "</trackfileextension>\n";
}

void Dingo::PlaylistWriter::setTrackURI(Glib::ustring track_uri) {
  d_write_trackuri = "      <trackuri>" + Dingo::Utilities::encodeFromEntityString(track_uri) + "</trackuri>\n";
}

void Dingo::PlaylistWriter::setTrackSubtitleURI(Glib::ustring track_subtitleuri) {
  d_write_tracksubtitleuri = "      <tracksubtitleuri>" + Dingo::Utilities::encodeFromEntityString(track_subtitleuri) + "</tracksubtitleuri>\n";
}

void Dingo::PlaylistWriter::setTrackYear(Glib::ustring track_year) {
  d_write_trackyear = "      <trackyear>" + Dingo::Utilities::encodeFromEntityString(track_year) + "</trackyear>\n";
}

void Dingo::PlaylistWriter::setTrackBitrate(int track_bitrate) {
  d_write_trackbitrate = "      <trackbitrate>" + Dingo::Utilities::intToString(track_bitrate) + "</trackbitrate>\n";
}

void Dingo::PlaylistWriter::setTrackMIMEType(Glib::ustring track_mimetype) {
  d_write_trackmimetype = "      <trackmimetype>" + track_mimetype + "</trackmimetype>\n";
}

void Dingo::PlaylistWriter::setTrackDateAdded(Glib::ustring track_dateadded) {
  d_write_trackdateadded = "      <trackdateadded>" + track_dateadded + "</trackdateadded>\n";
}

void Dingo::PlaylistWriter::setTrackComment(Glib::ustring track_comment) {
  d_write_trackcomment = "      <trackcomment>" + Dingo::Utilities::encodeFromEntityString(track_comment) + "</trackcomment>\n";
}

void Dingo::PlaylistWriter::setTrackLyrics(Glib::ustring track_lyrics) {
  d_write_tracklyrics = "      <tracklyrics>" + Dingo::Utilities::encodeFromEntityString(track_lyrics) + "</tracklyrics>\n";
}

//PUBLIC SAVER/WRITER
void Dingo::PlaylistWriter::saveTrack() {
  //concatenate
  d_whole_file_str += "    <track>\n" + d_write_trackartist + d_write_trackalbum + d_write_tracknumber + d_write_tracktitle + d_write_trackgenre + d_write_trackduration + d_write_tracksize + d_write_trackfilename + d_write_trackfileextension + d_write_trackuri + d_write_tracksubtitleuri + d_write_trackyear + d_write_trackbitrate + d_write_trackmimetype + d_write_trackdateadded + d_write_trackcomment + d_write_tracklyrics + "    </track>\n";
}

void Dingo::PlaylistWriter::writeToFile() {
  d_whole_file_str += "  </playlist>\n";
  d_whole_file_str += "</dingo>\n";
  
  Glib::ustring d_output_filepath = Glib::filename_from_uri(d_output_fileuri);
  
  std::ofstream outfile(d_output_filepath.c_str());
  
  if (outfile) {
    outfile << d_whole_file_str;
  }
  
  outfile.close();
}
