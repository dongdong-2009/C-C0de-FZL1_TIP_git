/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/list_ctrl_on_off_selection/ListCtrlOnOffSel.h $
  * @author Adam Radics
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ListCtrlOnOffSel inherits from ListCtrlSelNoFocus.
  * it allows selection to be click on, click off style
  * as if the user was holding ctrl constantly.
  *
  */

#if !defined(LIST_CTRL_ON_OFF_SEL_H)
#define LIST_CTRL_ON_OFF_SEL_H

#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"

namespace TA_Base_Bus
{
    #ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) ListCtrlOnOffSel : public ListCtrlSelNoFocus
    #else
    class AFX_EXT_CLASS ListCtrlOnOffSel : public ListCtrlSelNoFocus
    #endif
    {

    public:

	    ListCtrlOnOffSel();
	    virtual ~ListCtrlOnOffSel();

    public:

    // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ListCtrlOnOffSel)
	//}}AFX_VIRTUAL


	// Generated message map functions
    protected:

	//{{AFX_MSG(ListCtrlOnOffSel)
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	    
    DECLARE_MESSAGE_MAP()

    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} //TA_Base_Bus

#endif // !defined(LIST_CTRL_ON_OFF_SEL_H)
