#ifndef SSMF_GAME_H
#define SSMF_GAME_H

class SEGame
{
private:
public:
  SEGame();
  ~SEGame();

  int init(const CTFileName &fnGameSettings);
  COLOR LCDGetColor(COLOR colDefault, const char *strName);
};

#endif
