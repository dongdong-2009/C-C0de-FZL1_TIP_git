/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_viewer/src/SaveFilterDlg.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class provides a simple dialog for obtaining a filter name from the user
  * and then performing some limit validation of the result.
  */

#if !defined(AFX_SAVEFILTERDLG_H__74F3A220_0E9C_4720_83F0_F71359FC042C__INCLUDED_)
#define AFX_SAVEFILTERDLG_H__74F3A220_0E9C_4720_83F0_F71359FC042C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SaveFilterDlg : public CDialog
{
// Construction
public:
    /**
     * Constructor
     */
	SaveFilterDlg(CWnd* pParent = NULL);

    /**
     * getFilterName
     *
     * Returns the name chosen for the filter.
     *
     * @return std::string The chosen name.
     */
    std::string getFilterName();

// Dialog Data
	//{{AFX_DATA(SaveFilterDlg)
	enum { IDD = IDD_FILTER_NAME };
	CEdit	m_filterNameEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SaveFilterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

    /**
     * OnOK
     *
     * This method is overridden from CDialog to allow validation to be performed
     * on the chosen filter name.
     */
    virtual void OnOK();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SaveFilterDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    // This variable stores the filter name so it can be accessed after the dialog
    // is destroyed.
    std::string m_filterName;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SAVEFILTERDLG_H__74F3A220_0E9C_4720_83F0_F71359FC042C__INCLUDED_)
