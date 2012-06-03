#ifndef INCLUDED_DINGO_DBMANAGERTEST_H
#define INCLUDED_DINGO_DBMANAGERTEST_H

#ifndef INCLUDED_GTKMM_H
#define INCLUDED_GTKMM_H
#include <gtkmm.h>
#endif //INCLUDED_GTKMM_H

#include "dingo-dbmanager.h"

namespace Dingo {
  class DBManagerTest : public Gtk::Window {
    public:
      DBManagerTest();
      virtual ~DBManagerTest();
      
    private:
      //CREATORS
      void createTreeView();
      void createButton();
      void packWidgets();
    
      //SIGNALS
      void onButtonClicked();
    
      //DATA MEMBERS
      Gtk::TreeView d_treeview;
      Gtk::Button d_button;
      Gtk::VBox top_vbox;
      DBManager d_dbmanager;
  };
}

#endif //INCLUDED_DINGO_DBMANAGERTEST_H
