/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/event/event_viewer/src/SaveFilterDlg.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2009/12/03 15:46:08 $
  * Last modified by:  $Author: huirong.luo $
  *
  * This class provides a simple dialog for obtaining a filter name from the user
  * and then performing some limit validation of the result.
  */

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "stdafx.h"
#include "eventviewer.h"
#include "SaveFilterDlg.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/NamedFilter.h"
#include "core/data_access_interface/src/NamedFilterAccessFactory.h"

using TA_Base_Core::NamedFilter;
using TA_Base_Core::NamedFilterAccessFactory;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SaveFilterDlg::SaveFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SaveFilterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SaveFilterDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void SaveFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SaveFilterDlg)
	DDX_Control(pDX, IDC_EDIT_FILTER_NAME, m_filterNameEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SaveFilterDlg, CDialog)
	//{{AFX_MSG_MAP(SaveFilterDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SaveFilterDlg message handlers

BOOL SaveFilterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// A filter name cannot exceed 32 characters.
    m_filterNameEdit.SetLimitText( 32 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SaveFilterDlg::OnOK()
{
    CString nameStr;
    m_filterNameEdit.GetWindowText( nameStr );
    std::string newName( nameStr );

    if ( ( 0 == newName.size() ) || ( NamedFilter::UNNAMED == newName ) )
    {
        // The name must not be nothing, and not UNNAMED
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg.showMsgBox(IDS_UE_040040);
        return;
    }

    // Make sure the name isn't already taken.
    NamedFilterAccessFactory::FilterNameList filterNames =
        NamedFilterAccessFactory::getInstance().getAllFilterNames();
    NamedFilterAccessFactory::FilterNameList::iterator it;
    for ( it=filterNames.begin(); it!=filterNames.end(); it++ )
    {
        if ( it->first == newName )
        {
            // The chosen filter name is already in use.
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_040039);
            return;
        }
    }

    // Save the name.
    m_filterName = newName;

    // If we get here the name is valid.
    CDialog::OnOK();
}

std::string SaveFilterDlg::getFilterName()
{
    return m_filterName;
}