#include "dingo-dbmanagertest.h"

int main(int argc, char *argv[]) {
  Gtk::Main kit(argc, argv);
  
  Gst::init(argc, argv);
  
  Dingo::DBManagerTest dbmanagertest;
  
  Gtk::Main::run(dbmanagertest);

  return 0;
}
