/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#ifndef SE_INCL_MENUPRINTING_H
#define SE_INCL_MENUPRINTING_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

FLOATaabbox2D BoxTitle(void);
FLOATaabbox2D BoxVersion(void);
FLOATaabbox2D BoxBigRow(FLOAT fRow);
FLOATaabbox2D BoxBigLeft(FLOAT fRow);
FLOATaabbox2D BoxBigRight(FLOAT fRow);
FLOATaabbox2D BoxSaveLoad(FLOAT fRow);
FLOATaabbox2D BoxMediumRow(FLOAT fRow);
FLOATaabbox2D BoxMediumLeft(FLOAT fRow);
FLOATaabbox2D BoxPlayerSwitch(FLOAT fRow);
FLOATaabbox2D BoxPlayerEdit(FLOAT fRow);
FLOATaabbox2D BoxMediumMiddle(FLOAT fRow);
FLOATaabbox2D BoxMediumRight(FLOAT fRow);
FLOATaabbox2D BoxPopup(void);
FLOATaabbox2D BoxPopupLabel(void);
FLOATaabbox2D BoxPopupYesLarge(void);
FLOATaabbox2D BoxPopupNoLarge(void);
FLOATaabbox2D BoxPopupYesSmall(void);
FLOATaabbox2D BoxPopupNoSmall(void);
FLOATaabbox2D BoxInfoTable(INDEX iTable);
FLOATaabbox2D BoxChangePlayer(INDEX iTable, INDEX iButton);
FLOATaabbox2D BoxKeyRow(FLOAT fRow);
FLOATaabbox2D BoxArrow(enum ArrowDir ad);
FLOATaabbox2D BoxBack(void);
FLOATaabbox2D BoxNext(void);
FLOATaabbox2D BoxLeftColumn(FLOAT fRow);
FLOATaabbox2D BoxNoUp(FLOAT fRow);
FLOATaabbox2D BoxNoDown(FLOAT fRow);
FLOATaabbox2D BoxPlayerModel(void);
FLOATaabbox2D BoxPlayerModelName(void);
PIXaabbox2D FloatBoxToPixBox(const CDrawPort *pdp, const FLOATaabbox2D &boxF);
FLOATaabbox2D PixBoxToFloatBox(const CDrawPort *pdp, const PIXaabbox2D &boxP);
void SetFontTitle(CDrawPort *pdp);
void SetFontBig(CDrawPort *pdp);
void SetFontMedium(CDrawPort *pdp);
void SetFontSmall(CDrawPort *pdp);


#endif  /* include-once check. */

