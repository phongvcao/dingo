/* 
 * dingo-playerengine.h
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

#ifndef INCLUDED_DINGO_PLAYERENGINE_H
#define INCLUDED_DINGO_PLAYERENGINE_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_GDKX_H
#define INCLUDED_GDKX_H
#include <gdk/gdkx.h>
#endif //INCLUDED_GDKX_H

#ifndef INCLUDED_GSTREAMERMM_H
#define INCLUDED_GSTREAMERMM_H
#include <gstreamermm.h>
#endif //INCLUDED_GSTREAMERMM_H

#include "dingo-header.h"

namespace Dingo {
  class RepeatModeObserver;
  class EngineStatusObserver;
  class EngineNavigationObserver;
  class FullscreenObserver;
  class PlayOrderObserver;
  class RepeatToggleObserver;

  class PlayerEngine {
    public:
      PlayerEngine();
      virtual ~PlayerEngine();
      
      //OBSERVER'S METHODS
      void registerRepeatModeObserver(RepeatModeObserver* observer);
      void removeRepeatModeObserver(RepeatModeObserver* observer);
      void notifyRepeatModeObserver(RepeatMode repeat_mode);
      
      void registerEngineStatusObserver(EngineStatusObserver* observer);
      void removeEngineStatusObserver(EngineStatusObserver* observer);
      void notifyEngineStatusObserver(EngineStatus engine_status);
      
      void registerEngineNavigationObserver(EngineNavigationObserver* observer);
      void removeEngineNavigationObserver(EngineNavigationObserver* observer);
      void notifyEngineNavigationObserver(EngineNavigationSignals engine_nav_status);
      
      void registerFullscreenObserver(FullscreenObserver* observer);
      void removeFullscreenObserver(FullscreenObserver* observer);
      void notifyFullscreenObserver(FullscreenSignals signal_type);
      
      void registerPlayOrderObserver(PlayOrderObserver* observer);
      void removePlayOrderObserver(PlayOrderObserver* observer);
      void notifyPlayOrderObserver(PlayOrder play_order);
      
      void registerRepeatToggleObserver(RepeatToggleObserver* observer);
      void removeRepeatToggleObserver(RepeatToggleObserver* observer);
      void notifyRepeatToggleObserver(RepeatMode repeat_mode, Glib::ustring section_button_code = "");
      
      //PUBLIC CONTROLLERS
      void read(Glib::ustring fileuri, Glib::ustring track_mimetype, int track_id, Glib::ustring suburi = "");
      
      void setStatus(EngineStatus status);
      EngineStatus getStatus();
      
      void play();
      void pause();
      void next();
      void previous();
      void stop();
      void replay();
      
      void seek(gint64 new_pos);
      void seekForward(gint64 forward_pos);

      //ACCESSORS      
      Glib::ustring getPositionString();
      Glib::ustring getDurationString();
      
      double getPercentage();
      void setPercentage(double percentage);
      
      double getSectionStart();
      void setSectionStart(double percentage);
      
      double getSectionEnd();
      void setSectionEnd(double percentage);
      
      Glib::ustring getSectionStartString();
      Glib::ustring getSectionEndString();
      
      void setRepeatMode(RepeatMode repeat_mode, Glib::ustring section_button_code = "");
      RepeatMode getRepeatMode();
      
      void setPlayOrder(PlayOrder playorder);
      PlayOrder getPlayOrder();
      
      void setXWindowID(gulong x_window_id);
      gulong getXWindowID();
      
      void setFullscreen(bool is_fullscreen);
      bool getFullscreen();
    
      void setSubtitle(Glib::ustring suburi);
      void setShowSubtitle(bool is_visible);
      
      void setSubtitleFontDesc(Glib::ustring subfont);
      Glib::ustring getSubtitleFontDesc();
      
      void setVolume(double value);
      double getVolume();
      
      Glib::RefPtr<Gdk::Pixbuf> getPreviewImage(bool is_resize = true);
      Glib::ustring getPreviewImageURI();
      Glib::RefPtr<Gdk::Pixbuf> getDefaultPreviewImage(bool is_resize = true);
      
      Glib::ustring getMIMEType();
      gint64 getPosition();
      
    private:
      //SIGNAL HANDLERS
      void onBusMessageSync(const Glib::RefPtr<Gst::Message>& message);
      bool onBusMessage(const Glib::RefPtr<Gst::Bus>& bus, const Glib::RefPtr<Gst::Message>& message);
      bool onVideoPadGotBuffer(const Glib::RefPtr<Gst::Pad>& pad, const Glib::RefPtr<Gst::MiniObject>& data);
      
      bool restoreBeforeSubtitleSetPosition(double percentage);
      
      //DATA MEMBERS
      Glib::RefPtr<Gst::PlayBin2> d_playbin;
      Glib::RefPtr<Gst::XImageSink> d_videosink;
      Glib::RefPtr<Gst::XOverlay> d_xoverlay;
      guint d_watch_id;
      gulong d_x_window_id;
      Glib::ustring d_track_mimetype;
      gint64 d_pos, d_duration;
      Glib::ustring d_fileuri;
      Glib::ustring d_suburi, d_subfont;
      bool d_is_fullscreen;
      Glib::RefPtr<Gdk::Pixbuf> d_preview_pixbuf;
      int d_track_id;
      
      bool d_is_start_toggle_active;
      bool d_is_end_toggle_active;
      
      PlayOrder d_playorder;
      
      double d_section_start;
      double d_section_end;
      
      std::vector<RepeatModeObserver*> repeat_obs_vector;
      std::vector<EngineStatusObserver*> status_obs_vector;
      std::vector<EngineNavigationObserver*> nav_obs_vector;
      std::vector<FullscreenObserver*> fullscreen_obs_vector;
      std::vector<PlayOrderObserver*> playorder_obs_vector;
      std::vector<RepeatToggleObserver*> repeat_toggle_obs_vector;
      
      RepeatMode d_repeat_mode;
      EngineStatus d_status;
  };
}

#endif //INCLUDED_DINGO_PLAYERENGINE_H
