#if !defined(AFX_DUTYMANAGERPROPERTYSHEET_H__DFC83EDB_89EF_4845_A38F_43F0AB0AB4B9__INCLUDED_)
#define AFX_DUTYMANAGERPROPERTYSHEET_H__DFC83EDB_89EF_4845_A38F_43F0AB0AB4B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/src/DutyManagerPropertySheet.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class works around a shortcoming in CMultiFontPropertySheet which causes 
  * pages to be resized to their original dimensions upon activation.
  *
  */

namespace TA_Base_App
{
    class DutyManagerPropertySheet : public CMultiFontPropertySheet
    {
	    DECLARE_DYNAMIC(DutyManagerPropertySheet)

    // Operations
    public:
	    DutyManagerPropertySheet();
	    virtual ~DutyManagerPropertySheet();


    // Overrides
    protected:
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(DutyManagerPropertySheet)
	    //}}AFX_VIRTUAL


    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(DutyManagerPropertySheet)
	    afx_msg void OnSize(UINT nType, int cx, int cy);
	    //}}AFX_MSG

	    DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // !defined(AFX_DUTYMANAGERPROPERTYSHEET_H__DFC83EDB_89EF_4845_A38F_43F0AB0AB4B9__INCLUDED_)
