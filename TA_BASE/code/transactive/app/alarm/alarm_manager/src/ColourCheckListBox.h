/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_manager/src/ColourCheckListBox.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_COLOUR_CHECK_LIST_BOX_H__CCB479D0_5367_4905_B748_6C2429D62472__INCLUDED_)
#define AFX_COLOUR_CHECK_LIST_BOX_H__CCB479D0_5367_4905_B748_6C2429D62472__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColourCheckListBox.h : header file
//

#include <map>
#include "stdafx.h"


namespace TA_Base_App
{
	class AlarmListComponent;
	class CColourCheckListBox : public CCheckListBox
	{
	// Construction
	public:
        CColourCheckListBox();
        
        virtual ~CColourCheckListBox() {}

        void addItemColours(const std::map<int, COLORREF>& itemColours);
	
		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CColourCheckListBox)
		public:
        virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
		protected:
		//}}AFX_VIRTUAL

	// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CColourCheckListBox)
	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    private:

        int				        m_cxExtent;
	    COLORREF		        m_ColorHighlight;
	    COLORREF		        m_ColorHighlightText;
        std::map<int, COLORREF> m_itemColours;

	};
} //TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_COLOUR_CHECK_LIST_BOX_H__CCB479D0_5367_4905_B748_6C2429D62472__INCLUDED_

