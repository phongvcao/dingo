#ifndef INCLUDED_DINGO_NOTIFICATION_H
#define INCLUDED_DINGO_NOTIFICATION_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#ifndef INCLUDED_NOTIFY_H
#define INCLUDED_NOTIFY_H
#include <libnotify/notify.h>
#endif //INCLUDED_NOTIFY_H

#include "dingo-tracktreeviewobserver.h"

namespace Dingo {
  class PlayerEngine;

  class Notification : public TrackTreeViewObserver {
    public:
      Notification(PlayerEngine* playerengine);
      virtual ~Notification();
      
      //INHERITED OBSERVER'S METHODS
      void updateTrackTreeRowActivated(Gtk::TreeRowReference cur_track_row_ref);
      void updateTrackTreeRowDeactivated();
    
    private:
      //DATA MEMBERS
      PlayerEngine* d_playerengine;
      NotifyNotification* d_notification;
  };
}

#endif //INCLUDED_DINGO_NOTIFICATION_H
