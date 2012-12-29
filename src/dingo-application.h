/* 
 * dingo-application.h
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

#ifndef INCLUDED_DINGO_APPLICATION_H
#define INCLUDED_DINGO_APPLICATION_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_MEMORY
#define INCLUDED_MEMORY
#include <memory>
#endif //INCLUDED_MEMORY

#include "dingo-mainwindow.h"
#include "dingo-dbmanager.h"
#include "dingo-playerengine.h"
#include "dingo-settings.h"
#include "dingo-mprisv2.h"
#include "dingo-mpriscommon.h"
#include "dingo-programoptions.h"

namespace Dingo {
  class Application : public Gtk::Application {
    public:
      Application(int argc, char** argv);   
      virtual ~Application(); 
      
      //INHERITED PUBLIC METHODS
      int run();
    
    private:
      //CREATORS
      void createDingoObjects();
    
      //SIGNAL HANDLERS
      int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line);
      
      //DATA MEMBERS
      std::auto_ptr<DBManager> d_dbmanager;
      std::auto_ptr<PlayerEngine> d_playerengine;
      std::auto_ptr<Settings> d_settings;
      std::auto_ptr<MPRISCommon> d_mpriscommon;
      std::auto_ptr<ProgramOptions> d_programoptions;
      std::auto_ptr<MainWindow> d_mainwindow;
  };
}

#endif //INCLUDED_DINGO_APPLICATION_H
