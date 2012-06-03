#include "dingo-dbmanagertest.h"

//CONSTRUCTORS - DESTRUCTOR
Dingo::DBManagerTest::DBManagerTest() :
  Gtk::Window(),
  d_dbmanager("./dingo.db"),
  d_button("Import File(s)")
{
  set_title("Dingo DBManagerTest");
  set_size_request(500, 300);
  
  createTreeView();
  createButton();
  packWidgets();
}

Dingo::DBManagerTest::~DBManagerTest() {

}

//CREATORS
void Dingo::DBManagerTest::createTreeView() {
  d_treeview.set_model(Dingo::DBManager::trackModel);

  d_treeview.append_column(" ", (*Dingo::DBManager::trackCR).trackStatus);
  d_treeview.append_column("Track", (*Dingo::DBManager::trackCR).trackNumber);
  d_treeview.append_column("Title", (*Dingo::DBManager::trackCR).trackTitle);
  d_treeview.append_column("Genre", (*Dingo::DBManager::trackCR).trackGenre);
  d_treeview.append_column("Artist", (*Dingo::DBManager::trackCR).trackArtist);
  d_treeview.append_column("Album", (*Dingo::DBManager::trackCR).trackAlbum);
  d_treeview.append_column("Time", (*Dingo::DBManager::trackCR).trackTime);
}

void Dingo::DBManagerTest::createButton() {
  d_button.signal_clicked().connect(sigc::mem_fun(*this, &Dingo::DBManagerTest::onButtonClicked));
}

void Dingo::DBManagerTest::packWidgets() {
  top_vbox.pack_start(d_treeview, Gtk::PACK_EXPAND_WIDGET);
  top_vbox.pack_start(d_button, Gtk::PACK_SHRINK);
  
  add(top_vbox);
  
  show_all_children();
}

//SIGNALS
void Dingo::DBManagerTest::onButtonClicked() {
  Gtk::FileChooserDialog dialog("Choose File(s)", Gtk::FILE_CHOOSER_ACTION_OPEN);
  
  dialog.set_transient_for(*this);
  dialog.set_select_multiple(true);
  
  dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
  dialog.add_button("Import", Gtk::RESPONSE_OK);
  
  int result = dialog.run();
  
  switch (result) {
    case (Gtk::RESPONSE_OK): {
      //call the controller
      d_dbmanager.addTrackToSource(dialog.get_uris());
    }

    case (Gtk::RESPONSE_CANCEL): {
      break;
    }
    
    default: {
      break;
    }
  }
}
