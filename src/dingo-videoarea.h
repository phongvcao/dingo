/* 
 * dingo-videoarea.h
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

#ifndef INCLUDED_DINGO_VIDEOAREA_H
#define INCLUDED_DINGO_VIDEOAREA_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#include "dingo-header.h"
#include "dingo-fullscreenobserver.h"
#include "dingo-enginestatusobserver.h"

namespace Dingo {
  class DBManager;
  class PlayerEngine;
  class MouseMotionObserver;
  class StatusBar;

  class VideoArea : public Gtk::DrawingArea, public FullscreenObserver, public EngineStatusObserver {
    public:
      VideoArea(DBManager* dbmanager, PlayerEngine* playerengine, StatusBar* statusbar);
      virtual ~VideoArea();
      
      //OBSERVER'S METHODS
      void registerMouseMotionObserver(MouseMotionObserver* observer);
      void removeMouseMotionObserver(MouseMotionObserver* observer);
      void notifyMouseMotionObserver(VideoAreaSignals signal_type);
      
      //INHERITED OBSERVER'S METHODS
      void updateFullscreenActivated();
      void updateUnFullscreenActivated();
      
      void updateEnginePlaying();
      void updateEnginePaused();
      void updateEngineStopped();
      void updateEngineNewPlaying();
    
    private:
      //CREATORS
      void createVideoArea();
      
      //SIGNAL HANDLERS
      bool onVideoAreaDraw(const Cairo::RefPtr<Cairo::Context>& cairo_context);
      bool on_button_press_event(GdkEventButton* event);
      bool on_motion_notify_event(GdkEventMotion* event);
      bool onCursorHideTimeOut();
    
      //DATA MEMBERS
      std::vector<MouseMotionObserver*> mousemotion_obs_vector;
      Gdk::RGBA temp_rgba;
      sigc::connection d_cursor_hide_connection;
      sigc::connection d_draw_connection;
      int d_cursor_movement_count;
      
      DBManager* d_dbmanager;
      PlayerEngine* d_playerengine;
      StatusBar* d_statusbar;
  };
}

#endif //INCLUDED_DINGO_VIDEOAREA_H
