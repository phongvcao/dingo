#ifndef INCLUDED_DINGO_TAGREADER_H
#define INCLUDED_DINGO_TAGREADER_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_SSTREAM_H
#define INCLUDED_SSTREAM_H
#include <sstream>
#endif //INCLUDED_SSTREAM_H

#ifndef INCLUDED_TIME_H
#define INCLUDED_TIME_H
#include <time.h>
#endif //INCLUDED_TIME_H

#ifndef INCLUDED_GSTREAMERMM_H
#define INCLUDED_GSTREAMERMM_H
#include <gstreamermm.h>
#endif //INCLUDED_GSTREAMERMM_H

#include <stdint.h>

#ifndef INCLUDED_MEDIAINFO_H
#define INCLUDED_MEDIAINFO_H
#include "MediaInfo/MediaInfo.h"
#endif //INCLUDED_MEDIAINFO_H

namespace Dingo {
  class TagReader {
    public:
      TagReader();
      virtual ~TagReader();      
      
      bool read(Glib::ustring fileuri);
      
      //GETTERS
      Glib::ustring getTitle();
      Glib::ustring getArtist();
      Glib::ustring getAlbum();
      Glib::ustring getGenre();
      Glib::ustring getYear();
      Glib::ustring getComment();
      Glib::ustring getLyrics();
      int getTrackNumber();
      gint64 getDuration();
      int getBitrate();
      Glib::ustring getMIMEType();
      Glib::ustring getDateAdded();
      Glib::ustring getFileURI();
      Glib::ustring getFileName();
      Glib::ustring getFileExtension();
      gint64 getFileSize();
      
    private:
      MediaInfoLib::MediaInfo d_mediainfo;
      
      Glib::ustring d_fileuri;
  };
}

#endif //INCLUDED_DINGO_TAGREADER_H
