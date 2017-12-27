/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/DatabaseLoginDlg.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This is the dialog used to allow the user to enter their login details to connect directly to a database.
  */

#include "app/configuration/configuration_editor/src/stdafx.h"
#include "app/configuration/configuration_editor/src/DatabaseLoginDlg.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/Hostname.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include <boost/lexical_cast.hpp>

#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    DatabaseLoginDlg::DatabaseLoginDlg(const CString& reasonMessage)
        : m_reasonMessage(reasonMessage),
          CDialog(DatabaseLoginDlg::IDD, NULL)
    {
        //{{AFX_DATA_INIT(DatabaseLoginDlg)
        // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT
        m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    }

    void DatabaseLoginDlg::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(DatabaseLoginDlg)
        DDX_Control(pDX, IDC_USERNAME, m_user);
        DDX_Control(pDX, IDC_PASSWORD, m_password);
        DDX_Control(pDX, IDC_DATABASE, m_database);
        //}}AFX_DATA_MAP
    }

    BEGIN_MESSAGE_MAP(DatabaseLoginDlg, CDialog)
        //{{AFX_MSG_MAP(DatabaseLoginDlg)
        ON_EN_CHANGE(IDC_DATABASE, onChangeDatabase)
        ON_EN_CHANGE(IDC_PASSWORD, onChangePassword)
        ON_EN_CHANGE(IDC_USERNAME, onChangeUsername)
        ON_BN_CLICKED(IDOK, onOK)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // DatabaseLoginDlg message handlers

    BOOL DatabaseLoginDlg::OnInitDialog()
    {
        CDialog::OnInitDialog();

        // Set the icon for this dialog.  The framework does this automatically
        //  when the application's main window is not a dialog
        SetIcon(m_hIcon, TRUE);			// Set big icon
        SetIcon(m_hIcon, FALSE);		// Set small icon

        GetDlgItem(IDC_REASON)->SetWindowText(m_reasonMessage);

        refresh();

        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
    }

    void DatabaseLoginDlg::onOK()
    {
        FUNCTION_ENTRY("onOK");

        CString userString;
        m_user.GetWindowText(userString);

        CString passwordString;
        m_password.GetWindowText(passwordString);

        CString databaseString;
        m_database.GetWindowText(databaseString);

        if (/*userString.IsEmpty() || passwordString.IsEmpty() || */databaseString.IsEmpty())
        {
            FUNCTION_EXIT;
            return;
        }

        // Now set up the dbconnection string and set it in runparams
        // This is all broken with the distrubed db schema.
        // not sure how to fix it either...
        CString dbConnectionString = databaseString + ":" + userString + ":" + passwordString;
        TA_Base_Core::RunParams::getInstance().set(RPARAM_DBCONNECTION, dbConnectionString);
        TA_Base_Core::RunParams::getInstance().set(std::string(RPARAM_DBPREFIX + databaseString).c_str(), RPARAM_DBONLINE);

        // Now we should test this while we are still here
        try
        {
            // Now attempt to connect again.
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::OnlineUpdatable_Cd, TA_Base_Core::Write);
        }
        catch (const TA_Base_Core::DatabaseException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "The database connection we were given must be invalid");
            showMsgBox(IDS_UE_010017, databaseString);
            FUNCTION_EXIT;
            return;
        }

        CDialog::OnOK();

        FUNCTION_EXIT;
    }

    void DatabaseLoginDlg::onChangeDatabase()
    {
        refresh();
    }

    void DatabaseLoginDlg::onChangePassword()
    {
        refresh();
    }

    void DatabaseLoginDlg::onChangeUsername()
    {
        refresh();
    }

    void DatabaseLoginDlg::refresh()
    {
        CString userString;
        m_user.GetWindowText(userString);
        m_user.EnableWindow(FALSE);

        CString passwordString;
        m_password.GetWindowText(passwordString);
        m_password.EnableWindow(FALSE);

        CString databaseString;
        m_database.GetWindowText(databaseString);

        // If one of the fields is empty then the Ok button should be disabled
        if (/*userString.IsEmpty() || passwordString.IsEmpty() || */databaseString.IsEmpty())
        {
            GetDlgItem(IDOK)->EnableWindow(FALSE);
        }
        else
        {
            GetDlgItem(IDOK)->EnableWindow(TRUE);
        }
    }
}
