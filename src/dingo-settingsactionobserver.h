#ifndef INCLUDED_DINGO_SETTINGSACTIONOBSERVER_H
#define INCLUDED_DINGO_SETTINGSACTIONOBSERVER_H

namespace Dingo {
  class SettingsActionObserver {
    public:
      virtual void updateSettingsRead() = 0;
      virtual void updateSettingsWrite() = 0;
  };
}

#endif //INCLUDED_DINGO_SETTINGSACTIONOBSERVER_H
