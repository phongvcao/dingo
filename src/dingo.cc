#include "dingo-application.h"
#include <libintl.h>

int main(int argc, char** argv) {
  Dingo::Application dingo_application(argc, argv);
  
  bindtextdomain(GETTEXT_PACKAGE, DINGO_LOCALEDIR);
  bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
  textdomain(GETTEXT_PACKAGE);
  
  return dingo_application.run();
}
