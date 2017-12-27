/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/RestoredArchiveListComponent.h $
  * @author:  Wenguang
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * List control for the Restore Archive Dialog.
  *
  */

#if !defined(ICE__RestroredArchiveListComponet__H__)
#define ICE__RestroredArchiveListComponet__H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// RestoredArchiveListComponent.h : header file
//

#include <string>
#include <cstdio>
#include <memory>

class RestoredArchiveListComponent: public CListCtrl
{
public:
	RestoredArchiveListComponent();

	int OnToolHitTest(CPoint point, TOOLINFO * pTI) const; // CL-16726
	BOOL OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult); // CL-16726
	void AutoResizeColumn();
		// Generated message map functions
protected:
	//{{AFX_MSG(EventListComponent)
	
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
   
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
		
private:
	int		CellRectFromPoint( CPoint & point, RECT * cellrect, int * col ) const; // CL-16726
	static int	CALLBACK Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParam);
	static int	SortDate(std::string strBuf1, std::string strBuf2);
	int		m_nCurrentSortColumn;
    bool	m_fSortAscending;

	bool	m_bToolTipCtrlCustomizeDone;	// CL-16726
	
	std::auto_ptr<TCHAR> m_pchTip;	// CL-16726
	std::auto_ptr<WCHAR> m_pwchTip;	// CL-16726
protected:
	virtual void PreSubclassWindow(); // CL-16726
};

#endif