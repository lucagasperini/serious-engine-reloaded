#ifndef SSMF_GAME_H
#define SSMF_GAME_H

#include "MainWindow.h"
#include "Menu.h"

class SEGame
{
private:
  SEMainWindow* mainWin;
  SEMenu* menu;
public:
  SEGame();
  ~SEGame();

  void setMainWindow(SEMainWindow* _mainWin) { mainWin = _mainWin; }
  void setMenu(SEMenu* _menu) { menu = _menu; }

  SEMainWindow* getMainWindow() { return mainWin; }
  SEMenu* getMenu() { return menu; }

  int init(const CTFileName &fnGameSettings);
  void run();
  COLOR LCDGetColor(COLOR colDefault, const char *strName);
};

#endif
