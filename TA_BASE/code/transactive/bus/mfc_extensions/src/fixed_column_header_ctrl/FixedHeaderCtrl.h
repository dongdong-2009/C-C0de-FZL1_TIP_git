/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This provides a fixed column header control (for a CListCtrl).
  * The user cant resize columns, and the resize cursor isnt shown,
  * you cant simply disable the header in case you want a clickable sorting header.
  *
  * To use this, in the OnCreate, PreSubclassWindow or OnInitDialog just call subclassHeader
  * passing the existing header control (using GetHeaderCtrl).
  */

#if !defined(FIXEDHEADERCTRL_H)
#define FIXEDHEADERCTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus
{

    #ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) FixedHeaderCtrl : public CHeaderCtrl
    #else
    class AFX_EXT_CLASS FixedHeaderCtrl : public CHeaderCtrl
    #endif
    {
    public:

        FixedHeaderCtrl();


        virtual ~FixedHeaderCtrl();


        /** 
          * subclassHeader
          *
          * Dynamically subclass an existing header to this one.
          *
          * @param *pHeader The existing header
          *
          * @return TRUE on success
          */
        BOOL subclassHeader(CHeaderCtrl *pHeader);
        
        /**
         * Helper function to expand the column width of a list control
         *  so that the full space of the list control is taken up.
         * If the current set of columns exceeds the width of the list control,
         *  then nothing is done
         *
         * @pre columnIndex is a valid column index (not out of range)
         *
         * @param listCtrl the list control whose column width is to be adjusted
         * @param columnIndex defines the column which should receive the extra width
         * @param accountForScrollBar true if we want to leave some space for the vertical
         *          scrollbar to appear, without having the horizontal one appearing
         */
        static void expandColumnToFitListWidth(CListCtrl& listCtrl, DWORD columnIndex, bool accountForScrollBar);

    public:

	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(FixedHeaderCtrl)
	    //}}AFX_VIRTUAL

    protected:

        //{{AFX_MSG(FixedHeaderCtrl)
	    afx_msg void onBeginTrack(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onDividerDblClick(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point); // TD 13832  - lizettejl
	    //}}AFX_MSG

		CRect m_hiddenHdRect; // TD 13832 - lizettejl

	    DECLARE_MESSAGE_MAP()
    };

}

#endif // !defined(FIXEDHEADERCTRL_H)
