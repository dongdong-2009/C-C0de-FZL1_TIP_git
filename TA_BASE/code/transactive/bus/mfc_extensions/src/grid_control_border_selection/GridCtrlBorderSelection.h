/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/grid_control_border_selection/GridCtrlBorderSelection.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#if !defined(GRID_CONTROL_BORDER_SELECTION)
#define GRID_CONTROL_BORDER_SELECTION

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/grid_control_border_selection/GridCtrlBorderSelection.h $
  * @author:  Andy Siow
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Specialised derivation of TA_Base_Bus::CGridCtrl to allow border selection
  * instead of system highlight. This allows selection row selection of cells
  * without the loss of colour information.
  */


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//#define IMPORT_EXTENSIONS
#include "bus/mfc_extensions/src/grid_control/GridCtrl.h"

#include <vector>
#include <string>

#ifdef IMPORT_EXTENSIONS
class __declspec(dllimport) GridCtrlBorderSelection : public CGridCtrl
#else
class AFX_EXT_CLASS GridCtrlBorderSelection : public CGridCtrl
#endif
{

	//class GridCtrlBorderSelection : public CGridCtrl  
//{
	

public:

    /** 
      * Constructor
      */
    GridCtrlBorderSelection();


    /** 
      * Destructor
      */
    virtual ~GridCtrlBorderSelection();
	
	// Nested Class
	struct SelectedCell
	{
		bool isRowSelected;
		CCellRange selectedRange;
	};

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(StationModesOverviewGridCtrl)
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

    // Generated message map functions
protected:

	virtual void OnDraw(CDC* pDC);
	virtual void SelectRows(CCellID currentCell);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

protected:
	// Added selected row feature.
	std::auto_ptr<SelectedCell> m_selectedRow;
//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:


    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
};

#endif  // !defined(GRID_CONTROL_BORDER_SELECTION)
