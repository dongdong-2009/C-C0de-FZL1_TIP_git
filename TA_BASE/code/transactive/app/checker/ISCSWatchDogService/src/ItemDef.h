// ItemDef.h: interface for the CItemDef class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMDEF_H__8288088F_171D_4606_ACF6_62B73CF597A4__INCLUDED_)
#define AFX_ITEMDEF_H__8288088F_171D_4606_ACF6_62B73CF597A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

/////////////////////////////////////////////////////////////////////////////
//
// CItemDef defines the local instance for each defined item
//
//////////////////////////////////////////////////////////////////////////////
class CItemDef : public CObject  
{
public:
	CItemDef();
	virtual ~CItemDef();

	CString		m_csName;			// Item Name
	CString		m_csPath;			
	HANDLE		m_hItem;			// Handle returned from the Server
	DWORD		m_dwAccessRights;	// Access Rights     "    "    "
	FILETIME	m_timestamp;		// Current TimeStamp, Quality & Value
	DWORD		m_dwQuality;		// Updated from the Server
	VARIANT		m_value;
};

#endif // !defined(AFX_ITEMDEF_H__8288088F_171D_4606_ACF6_62B73CF597A4__INCLUDED_)
