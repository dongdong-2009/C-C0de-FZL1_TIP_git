/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationProgressDlg.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class implements a dialog that is used to represent the percentage of
  * configuration plugins that have been loaded.
  *
  */

#if !defined(AFX_CONFIGURATIONPROGRESSDLG_H__1481185A_C5F3_4631_942F_AFA674786193__INCLUDED_)
#define AFX_CONFIGURATIONPROGRESSDLG_H__1481185A_C5F3_4631_942F_AFA674786193__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigurationProgressDlg.h : header file
//

#include "app/configuration/configuration_editor/src/resource.h"
#include "app/configuration/configuration_editor/src/ComponentLibraryAccessor.h"

/////////////////////////////////////////////////////////////////////////////
// CConfigurationProgressDlg dialog

class CConfigurationProgressDlg : public CDialog, public TA_Base_App::IComponentLibraryAccessorCallback
{
// Construction
public:
	CConfigurationProgressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConfigurationProgressDlg)
	enum { IDD = IDD_LOADPROGRESS };
	CProgressCtrl	m_progress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConfigurationProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

    /**
     * updateLoadProgress
     *
     * Is called whenever this is an update in the percentage of components
     * loaded.
     *
     * @param percentage The percentage of components that have been loaded.
     */
    virtual void updateLoadProgress( unsigned char percentage );
    virtual void setTotalComponentNumber(size_t totalComponentNumber);
    virtual void setLoadedComponentNumber(size_t loadedComponentNumber);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConfigurationProgressDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

    int m_totalComponentNumber;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGURATIONPROGRESSDLG_H__1481185A_C5F3_4631_942F_AFA674786193__INCLUDED_)
