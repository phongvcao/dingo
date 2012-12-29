#ifndef INCLUDED_DINGO_PLAYLISTWRITER_H
#define INCLUDED_DINGO_PLAYLISTWRITER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

namespace Dingo {
  class PlaylistWriter {
    public:
      PlaylistWriter();
      virtual ~PlaylistWriter();
      
      //PUBLIC METHODS - WRITE
      void open(Glib::ustring output_fileuri, Glib::ustring playlist_name, int playlist_size);
      
      //PUBLIC CREATORS
      void createTrack();
      
      //PUBLIC ACCESSORS
      void setTrackArtist(Glib::ustring track_artist);
      void setTrackAlbum(Glib::ustring track_album);
      void setTrackNumber(int track_number);
      void setTrackTitle(Glib::ustring track_title);
      void setTrackGenre(Glib::ustring track_genre);
      void setTrackDuration(gint64 track_duration);
      void setTrackSize(gint64 track_size);
      void setTrackFileName(Glib::ustring track_filename);
      void setTrackFileExtension(Glib::ustring track_fileextension);
      void setTrackURI(Glib::ustring track_uri);
      void setTrackSubtitleURI(Glib::ustring track_subtitleuri);
      void setTrackYear(Glib::ustring track_year);
      void setTrackBitrate(int track_bitrate);
      void setTrackMIMEType(Glib::ustring track_mimetype);
      void setTrackDateAdded(Glib::ustring track_dateadded);
      void setTrackComment(Glib::ustring track_comment);
      void setTrackLyrics(Glib::ustring track_lyrics);
      
      //PUBLIC SAVER/WRITER
      void saveTrack();
      void writeToFile();
    
    private:
      //DATA MEMBERS - WRITE
      Glib::ustring d_write_trackartist;
      Glib::ustring d_write_trackalbum;
      Glib::ustring d_write_tracknumber;
      Glib::ustring d_write_tracktitle;
      Glib::ustring d_write_trackgenre;
      Glib::ustring d_write_trackduration;
      Glib::ustring d_write_tracksize;
      Glib::ustring d_write_trackfilename;
      Glib::ustring d_write_trackfileextension;
      Glib::ustring d_write_trackuri;
      Glib::ustring d_write_tracksubtitleuri;
      Glib::ustring d_write_trackyear;
      Glib::ustring d_write_trackbitrate;
      Glib::ustring d_write_trackmimetype;
      Glib::ustring d_write_trackdateadded;
      Glib::ustring d_write_trackcomment;
      Glib::ustring d_write_tracklyrics;
      
      Glib::ustring d_whole_file_str;
      
      Glib::ustring d_output_fileuri;
  };
}

#endif //INCLUDED_DINGO_PLAYLISTWRITER_H
