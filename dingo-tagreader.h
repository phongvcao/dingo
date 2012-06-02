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

namespace Dingo {
  class TagReader {
    public:
      TagReader();
      virtual ~TagReader();      
      
      void read(Glib::ustring fileuri);
      
      //GETTERS
      Glib::ustring getTitle();
      Glib::ustring getArtist();
      Glib::ustring getAlbum();
      Glib::ustring getGenre();
      unsigned int getYear();
      unsigned int getTrackNumber();
      gint64 getDuration();
      unsigned int getBitrate();
      std::string getDateAdded();
      Glib::ustring getFileURI();
      Glib::ustring getFileName();
      gint64 getSize();
      
      //SETTERS
      void setTitle(const Glib::ustring title);
      void setArtist(const Glib::ustring artist);
      void setAlbum(const Glib::ustring album);
      void setGenre(const Glib::ustring genre);
      void setYear(unsigned int year);
      void setTrackNumber(unsigned int track);
      
    private:
      Gst::TagList d_taglist;
      int clocktime;
      
      Glib::ustring d_fileuri;
      std::string d_filepath;
  };
}

#endif //INCLUDED_DINGO_TAGREADER_H
