/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/outlook_bar_control/GfxPopupMenu.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// GfxPopupMenu.h: interface for the CGfxPopupMenu class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GFXPOPUPMENU_H__F2CEE8B2_014D_11D2_83E4_0000B43382FE__INCLUDED_)
#define AFX_GFXPOPUPMENU_H__F2CEE8B2_014D_11D2_83E4_0000B43382FE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class AFX_EXT_CLASS CGfxPopupMenu : public CMenu  
{
public:
	struct ToolBarData	
	{ // struct for toolbar resource; guess you already know it ;)
		WORD wVersion;
		WORD wWidth;			
		WORD wHeight;			
		WORD wItemCount;		
	};
	struct SpawnItem
	{
		int		iImageIdx;
		int		iCmd;
		char	cText[128];
	};
	struct ImageItem
	{
		int		iImageIdx;
		int		iCmd;
	};

	CGfxPopupMenu();
	virtual ~CGfxPopupMenu();

	HFONT hMenuBoldFont;
	bool CreateBoldFont();

	HFONT	hMenuFont, hGuiFont;
	CSize szImage;
	int FindImageItem(const int cmd);

	SpawnItem * AddSpawnItem(const char * txt, const int cmd);
	void AddImageItem(const int idx, WORD cmd);

	void RemapMenu(CMenu * pMenu);

	CImageList ilList;
	CImageList ilOther;

	COLORREF crMenuText, crMenuTextSel;
	COLORREF cr3dFace, crMenu, crHighlight, cr3dHilight, cr3dShadow, crGrayText;
	COLORREF m_clrBtnFace, m_clrBtnHilight, m_clrBtnShadow;

	int iSpawnItem;
	SpawnItem ** pSpawnItem;

	int iImageItem;
	ImageItem * pImageItem;

	bool LoadToolBarResource(unsigned int resId);
	bool AddToolBarResource(unsigned int resId);

	void EnableMenuItems(CMenu * pMenu, CWnd * pParent);

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
};

#endif // !defined(AFX_GFXPOPUPMENU_H__F2CEE8B2_014D_11D2_83E4_0000B43382FE__INCLUDED_)
