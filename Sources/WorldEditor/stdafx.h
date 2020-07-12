/* Copyright (c) 2002-2012 Croteam Ltd. 
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
#ifndef STDAFH_H
#define STDAFX_H

#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers
#include <Engine/Base/Types.h>

#ifdef PLATFORM_WIN32
#include <afxadv.h>
#include <afxext.h> // MFC extensions
#include <afxole.h>
#include <afxwin.h> // MFC core and standard components
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h> // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <htmlhelp.h>
#else
#include <wx/combobox.h>
#include <wx/dataobj.h>
#include <wx/dialog.h>
#include <wx/mdi.h>
#include <wx/scrolbar.h>
#include <wx/statusbr.h>
#include <wx/toolbar.h>
#include <wx/validate.h>
#include <wx/window.h>
#include <wx/button.h>
#include <wx/treectrl.h>
#include <wx/docview.h>
#include <wx/generic/splitter.h>
#include <wx/propdlg.h>
#include <wx/checklst.h>
#include <wx/slider.h>
#include <wx/gauge.h>
#include <wx/spinbutt.h>
#include <wx/imaglist.h>
#include <wx/listctrl.h>
#include <wx/app.h>

#define AfxGetApp() ::wxGetApp()
#define CObject wxObject

#define CRect wxRect
#define CSize wxSize
#define CDC wxDC
#define CPoint wxPoint
#define CFont wxFont
#define CPen wxPen
#define CBrush wxBrush
#define CMetaFileDC wxMetaFileDC

#define CWnd wxWindow
#define CDialog wxDialog
#define CButton wxButton
#define CListBox wxListBox
#define CComboBox wxComboBox
#define CToolBar wxToolBar
#define CStatusBar wxStatusBar
#define CTreeCtrl wxTreeCtrl
#define MywxListBox CB_ListBox

#define CDocument wxDocument

#define AfxMessageBox wxMessageBox
#define DECLARE_MESSAGE_MAP DECLARE_EVENT_TABLE
#define BEGIN_MESSAGE_MAP BEGIN_EVENT_TABLE
#define END_MESSAGE_MAP END_EVENT_TABLE

#define IMPLEMENT_SERIAL_CLASS_CLASS( IMPLEMENT_SERIAL_CLASS(x)
#define DECLARE_SERIAL_CLASS DECLARE_SERIAL_CLASS
#define IMPLEMENT_SERIAL(x) IMPLEMENT_SERIAL_CLASS(x)
#define DECLARE_DYNCREATE DECLARE_DYNAMIC_CLASS
#define IMPLEMENT_DYNCREATE(x) IMPLEMENT_DYNAMIC_CLASS(x)

#define OnInitDialog(x) InitDialog(x)
#define CompareNoCase CmpNoCase
#define LVITEM wxListItem

#define VERIFY wxCHECK
#define IDOK wxOK
#define IDCANCEL wxCANCEL
#define IDYES wxYES
#define IDNO wxNO
#define MB_YESNO wxYES_NO
#define MB_OK wxOK
#define MB_CENTRE wxCENTRE
#define MB_ICONEXCLAMATION wxICON_EXCLAMATION
#define MB_ICONHAND wxICON_HAND
#define MB_ICONERROR wxICON_ERROR
#define MB_ICONQUESTION wxICON_QUESTION
#define MB_ICONINFORMATION wxICON_INFORMATION

#define afx_msg
#define AFXAPI
#define CDataExchange wxValidator
#define CString wxString
#define CCmdUI void
#define CScrollBar wxScrollBar
#define CMDIFrameWnd wxMDIParentFrame
#define CPoint 	wxPoint
#define CWnd wxWindow
#define LPCTSTR char*
#define TOOLINFO tagTOOLINFOA
#define LPCREATESTRUCT tagCREATESTRUCTA
#define CREATESTRUCT tagCREATESTRUCTA 
#define CMultiDocTemplate wxDocTemplate
#define CWinApp wxApp
#define CMDIChildWnd wxMDIChildFrame
#define CSplitterWnd wxSplitterWindow
#define CEdit wxTextCtrl
#define CMenu wxMenu
#define CPropertySheet wxPropertySheetDialog
#define CCheckListBox wxCheckListBox
#define CSliderCtrl wxSlider
#define CProgressCtrl wxGauge
#define CPropertyPage wxPanel
#define CSpinButtonCtrl wxSpinButton
#define CImageList wxImageList
#define CListCtrl wxListCtrl

#define ON_COMMAND EVT_MENU
#define ON_UPDATE_COMMAND_UI EVT_UPDATE_UI
#define DECLARE_DYNAMIC DECLARE_CLASS

typedef struct HDROP__ { int unused; } *HDROP;


typedef void* HACCEL;
typedef void* HBRUSH;
typedef void* HDC;
typedef void* HFONT;
typedef void* HICON;
typedef void* HMENU;
typedef void* HWND;
typedef HICON HCURSOR;

/* ANSI string types */
typedef CHAR           *PCH,        *LPCH,      *PNZCH;
typedef const CHAR     *PCCH,       *LPCCH,     *PCNZCH;
typedef CHAR           *PSTR,       *LPSTR,     *NPSTR;
typedef const CHAR     *PCSTR,      *LPCSTR;
typedef CHAR           *PZZSTR;
typedef const CHAR     *PCZZSTR;

/* Unicode string types */
typedef unsigned short  WCHAR,      *PWCHAR;
typedef const WCHAR    *PCWCHAR,    *LPCUWCHAR, *PCUWCHAR;
typedef WCHAR          *PWCH,       *LPWCH;
typedef const WCHAR    *PCWCH,      *LPCWCH;
typedef WCHAR          *PNZWCH,     *PUNZWCH;
typedef const WCHAR    *PCNZWCH,    *PCUNZWCH;
typedef WCHAR          *PWSTR,      *LPWSTR,    *NWPSTR;
typedef const WCHAR    *PCWSTR,     *LPCWSTR;
typedef WCHAR          *PZZWSTR,    *PUZZWSTR;
typedef const WCHAR    *PCZZWSTR,   *PCUZZWSTR;
typedef PWSTR          *PZPWSTR;
typedef PCWSTR         *PZPCWSTR;

typedef long INT_PTR, *PINT_PTR;
typedef unsigned long UINT_PTR, *PUINT_PTR;
typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;

typedef LONG_PTR LRESULT;


#define HELP_CONTEXT NULL

struct _TREEITEM{

};
typedef struct _TREEITEM *HTREEITEM;
//#define CView xsView
class CMiniFrameWnd {

};

class CView {

};

class CDumpContext {
};

class CControlBar {
};

class CCreateContext {

};

//#define COleDataSource wxDataObject
class COleDataSource {

};

class COleDropTarget {

};

class CArchive {

};

class CDialogBar {
};

typedef struct tagCREATESTRUCTA {
  LPVOID    lpCreateParams;
  HINSTANCE hInstance;
  HMENU     hMenu;
  HWND      hwndParent;
  int       cy;
  int       cx;
  int       y;
  int       x;
  LONG      style;
  LPCSTR    lpszName;
  LPCSTR    lpszClass;
  DWORD     dwExStyle;
} CREATESTRUCTA, *LPCREATESTRUCTA;

typedef struct tagTOOLINFOA {
  UINT      cbSize;
  UINT      uFlags;
  HWND      hwnd;
  UINT_PTR  uId;
  RECT      rect;
  HINSTANCE hinst;
  LPSTR     lpszText;
  LPARAM    lParam;
  void      *lpReserved;
} TTTOOLINFOA, *PTOOLINFOA, *LPTTTOOLINFOA;

typedef struct tagWINDOWPLACEMENT {
    UINT length;
    UINT flags;
    UINT showCmd;
    POINT ptMinPosition;
    POINT ptMaxPosition;
    RECT rcNormalPosition;
} WINDOWPLACEMENT, *PWINDOWPLACEMENT, *LPWINDOWPLACEMENT;

typedef struct _nmhdr {
  HWND hwndFrom;
  UINT idFrom;
  UINT code;
} NMHDR;


/* Sent as the lParam of a WM_DRAWITEM message to instruct how an
 * owner drawn control is to be drawn */
typedef struct tagDRAWITEMSTRUCT {
    UINT CtlType; /* Type of control (ODT_* flags from "winuser.h") */
    UINT CtlID; /* Control ID */
    UINT itemID; /* Menu item ID */
    UINT itemAction; /* Action to perform (ODA_* flags from "winuser.h") */
    UINT itemState; /* Item state (ODS_* flags from "winuser.h") */
    HWND hwndItem; /* Control window */
    HDC hDC; /* Device context to draw to */
    RECT rcItem; /* Position of the control in hDC */
    ULONG_PTR itemData; /* Extra data added by the application, if any */
} DRAWITEMSTRUCT, *PDRAWITEMSTRUCT, *LPDRAWITEMSTRUCT;

typedef struct tagMEASUREITEMSTRUCT {
    UINT CtlType;
    UINT CtlID;
    UINT itemID;
    UINT itemWidth;
    UINT itemHeight;
    ULONG_PTR itemData;
} MEASUREITEMSTRUCT, *PMEASUREITEMSTRUCT, *LPMEASUREITEMSTRUCT;

#endif

#define ENGINE_INTERNAL 1
#include <Engine/Base/ChangeableRT.h>
#include <Engine/Base/SDL/SDLEvents.h>
#include <Engine/Base/UpdateableRT.h>
#include <Engine/Base/Types.h>
#include <Engine/GameShell.h>
#include <Engine/Terrain/Terrain.h>
#include <Engine/Terrain/TerrainMisc.h>
#include <EngineGui/EngineGUI.h>

#include "WorldEditor.h"

#define SNAP_FLOAT_GRID 0.25f

#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif