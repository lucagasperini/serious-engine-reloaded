#ifndef SSMF_MENU_H
#define SSMF_MENU_H

#include <Engine/Graphics/Texture.h>
#include "Render.h"
#include "MenuGadget.h"
#include "Button.h"

class CGameMenu {
public:
    SERender *render;
  CListHead gm_lhGadgets;
  CGameMenu *gm_pgmParentMenu;
  BOOL gm_bPopup;
  const char *gm_strName;   // menu name (for mod interface only)
  class CMenuGadget *gm_pmgSelectedByDefault;
  class CMenuGadget *gm_pmgArrowUp;
  class CMenuGadget *gm_pmgArrowDn;
  class CMenuGadget *gm_pmgListTop;
  class CMenuGadget *gm_pmgListBottom;
  INDEX gm_iListOffset;
  INDEX gm_iListWantedItem;   // item you want to focus initially
  INDEX gm_ctListVisible;
  INDEX gm_ctListTotal;
  CGameMenu(void);
  void ScrollList(INDEX iDir);
  void KillAllFocuses(void);
  virtual void Initialize_t(void);
  virtual void Destroy(void);
  virtual void StartMenu(void);
  virtual void FillListItems(void);
  virtual void EndMenu(void);
  // return TRUE if handled
  virtual BOOL OnKeyDown( int iVKey);
  virtual BOOL OnChar(MSG msg);
  virtual void Think(void);
};


class CMainMenu : public CGameMenu {
public:
    //CMGTitle mgMainTitle;
SEButton mgMainVersionLabel;
SEButton mgMainModLabel;
SEButton mgMainSingle;
SEButton mgMainNetwork;
SEButton mgMainSplitScreen;
SEButton mgMainDemo;
SEButton mgMainMods;
SEButton mgMainHighScore;
SEButton mgMainOptions;
SEButton mgMainQuit;

FLOAT fontSize;
FLOAT positionStart;

  void Initialize_t(SERender* _render);
  void StartMenu(void);
  PIXaabbox2D positionMenuItem(FLOAT fRow);
};



class SEMenu 
{
private:
    CTextureObject _toPointer;
    CTextureObject _toLogoMenuA;
    CTextureObject _toLogoMenuB;

    SERender* render;

// logo textures
    CTextureObject *_ptoLogoCT  = NULL;
    CTextureObject *_ptoLogoODI = NULL;
    CTextureObject *_ptoLogoEAX = NULL;
    
    CMainMenu gmMainMenu;

    CGameMenu* pgmCurrentMenu = NULL;

    BOOL active;
public:
    SEMenu();
    ~SEMenu();

    void setActive(BOOL _active) { active = _active; }

    BOOL getActive() { return active; }

    void LoadAndForceTexture(CTextureObject &to, CTextureObject *&pto, const CTFileName &fnm);




    void init(SERender* _render);
    BOOL run();
};

#endif