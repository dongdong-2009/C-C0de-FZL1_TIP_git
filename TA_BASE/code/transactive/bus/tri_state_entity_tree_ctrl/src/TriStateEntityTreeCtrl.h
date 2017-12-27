/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/tri_state_entity_tree_ctrl/src/TriStateEntityTreeCtrl.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#if !defined(AFX_TRISTATEENTITYTREECTRL_H__682E0959_7853_4946_9EE6_A7829D2BA1E8__INCLUDED_)
#define AFX_TRISTATEENTITYTREECTRL_H__682E0959_7853_4946_9EE6_A7829D2BA1E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>

#include "bus/mfc_extensions/src/object_tree_control/ObjectTreeCtrl.h"

class CTriStateEntityTreeItem;

typedef std::map<std::string, int> String2IntMap;

/////////////////////////////////////////////////////////////////////////////
// CTriStateEntityTreeCtrl window

#if 0
#ifdef IMPORT_EXTENSIONS
class __declspec(dllimport) CTriStateEntityTreeCtrl : public CObjectTreeCtrl
#else
class AFX_EXT_CLASS CTriStateEntityTreeCtrl : public CObjectTreeCtrl
#endif
#endif
class CTriStateEntityTreeCtrl : public CObjectTreeCtrl
{
// Construction
public:
	CTriStateEntityTreeCtrl();

// Attributes
public:

// Operations
public:
	void setIcons( HICON hNoCheck, HICON hChecked, HICON hIndeterminant )
	{
		m_hCheckedIcon = hChecked;
		m_hUnCheckedIcon = hNoCheck;
		m_hIndeterminantIcon = hIndeterminant;
	}

	void setEntityMap( String2IntMap* pEntityMap ) { m_pOrigEntityMap = pEntityMap; }
	String2IntMap* getEntityMap() { return m_pOrigEntityMap; }

	HICON getCheckedIcon() { return m_hCheckedIcon; }
	HICON getUnCheckedIcon() { return m_hUnCheckedIcon; }
	HICON getIndeterminantIcon() { return m_hIndeterminantIcon; }

	void AddRootItem( CTriStateEntityTreeItem& treeItem, bool deleteItem );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTriStateEntityTreeCtrl)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTriStateEntityTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTriStateEntityTreeCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

private:
	DECLARE_MESSAGE_MAP()

	String2IntMap*		m_pOrigEntityMap;
	HICON		m_hCheckedIcon,
				m_hUnCheckedIcon,
				m_hIndeterminantIcon;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRISTATEENTITYTREECTRL_H__682E0959_7853_4946_9EE6_A7829D2BA1E8__INCLUDED_)
