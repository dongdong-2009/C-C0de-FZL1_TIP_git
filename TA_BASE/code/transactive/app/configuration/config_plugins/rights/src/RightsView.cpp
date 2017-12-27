/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/rights/src/RightsView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This is the main view for configuring rights. This is the view that will display all rights information
  * and allow it to be configured.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/rights/src/stdafx.h"
#include "app/configuration/config_plugins/rights/src/RightsView.h"
#include "app/configuration/config_plugins/rights/src/RightsComponent.h"
#include "app/configuration/config_plugins/rights/src/RightsDatabaseAccessor.h"
#include "app/configuration/configuration_editor/src/IProgressStatusBar.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/HelperResource.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/data_access_interface/src/ConfigRights.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace
{
    void translateToChinese(TA_Base_App::ChangeDialog::ItemsForSelection& items)
    {
        std::map<std::string, std::string> translation;
        translation.insert(std::make_pair("Normal", "正常"));
        translation.insert(std::make_pair("Delegated", "代理"));
        translation.insert(std::make_pair("Degraded", "降级"));

        for (TA_Base_App::ChangeDialog::ItemsForSelection::iterator it = items.begin(); it != items.end(); ++it)
        {
            it->second = translation[it->second];
        }
    }
}

namespace TA_Base_App
{
    // These are the control Id's to give the grids we create. These id's are the two last id's in the
    // range specified for this component.
    const int RightsView::SUBSYSTEM_GRID_ID = 10998;
    const int RightsView::APPLICATION_GRID_ID = 10999;

    // This is the maximum number of items we should show in a list in an error message
    const int RightsView::MAX_NUMBER_OF_ITEMS_TO_LIST = 10;

    const CString RightsView::CHANGE_SUBSYSTEM_STATE_DIALOG_TITLE = "更改子系统状态";

    IMPLEMENT_DYNCREATE(RightsView, CFormView)

    RightsView::RightsView()
        : m_component(NULL),
          m_subsystemStateKey(0),
          CFormView(RightsView::IDD)
    {
    }

    RightsView::~RightsView()
    {
    }

    void RightsView::DoDataExchange(CDataExchange* pDX)
    {
        CFormView::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(RightsView)
        DDX_Control(pDX, IDC_SUBSYSTEM_LIST, m_subsystemList);
        DDX_Control(pDX, IDC_APPLICATION_LIST, m_applicationList);
        DDX_Control(pDX, IDC_SUBSYSTEM_STATE_EDIT, m_subsystemState);
        //}}AFX_DATA_MAP
    }

    BEGIN_MESSAGE_MAP(RightsView, CFormView)
        //{{AFX_MSG_MAP(RightsView)
        ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
        ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
        ON_BN_CLICKED(IDC_CHANGE_SUBSYSTEM_STATE, onChangeSubsystemState)
        ON_NOTIFY(TCN_SELCHANGE, IDC_SUBSYSTEM_APP_TAB, onSelchangeSubsystemAppTab)
        ON_WM_SIZE()
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void RightsView::OnInitialUpdate()
    {
        CFormView::OnInitialUpdate();
    }

    void RightsView::setComponentInterface(RightsComponent* component)
    {
        TA_ASSERT(component != NULL, "The component passed in was NULL");

        m_component = component;

        loadSubsystemState();

        static_cast<CTabCtrl*>(GetDlgItem(IDC_SUBSYSTEM_APP_TAB))->InsertItem(0, "子系统");
        static_cast<CTabCtrl*>(GetDlgItem(IDC_SUBSYSTEM_APP_TAB))->InsertItem(1, "应用程序");

        loadItems();

        // hide the application grid - only do this the first time
        m_applicationGrid.ShowWindow(SW_HIDE);
        m_subsystemGrid.BringWindowToTop();
    }

    void RightsView::refreshButtonAvailability(bool isAvailable)
    {
        GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(isAvailable);
        GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(isAvailable);
    }

    void RightsView::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        TA_ASSERT(m_component != NULL, "The RightsComponent must be set in this class before this method can be called");

        // Retrieve the progress bar and display it on the status bar
        CProgressCtrl& progress = m_component->getStatusBar()->displayProgressBar();
        progress.SetStep(1);

        CRect position;

        if (m_subsystemGrid.m_hWnd == NULL)
        {
            // Use our dummy static control to retrieve the positioning for the grid and then destroy it

            GetDlgItem(IDC_SUBSYSTEM_LIST_POSITION)->GetWindowRect(&position);
            GetDlgItem(IDC_SUBSYSTEM_LIST_POSITION)->DestroyWindow();

            // Now convert to client position rather than on the whole screen
            ScreenToClient(&position);

            // Now create the subsystem grid at the position given.
            m_subsystemGrid.Create(position, this, SUBSYSTEM_GRID_ID, WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER);

            //m_subsystemGrid.SetWindowPos()
        }

        try
        {
            m_subsystemGrid.loadItems(m_component->getDatabaseAccessor(), RightsDatabaseAccessor::SUBSYSTEM, progress, m_subsystemStateKey);

            if (m_applicationGrid.m_hWnd == NULL)
            {
                // Now create the subsystem grid at the position given.
                m_applicationGrid.Create(position, this, APPLICATION_GRID_ID, WS_CHILD | WS_TABSTOP | WS_VISIBLE | WS_BORDER);
            }

            m_applicationGrid.loadItems(m_component->getDatabaseAccessor(), RightsDatabaseAccessor::APPLICATION, progress, m_subsystemStateKey);

            m_component->getStatusBar()->hideProgressBar();

            /* This comment is removing the code that enforces that each subsystem only has a duty set for one central
               profile. If this is reversed this should be added back in. The removal relates to PW2621.

                        isValidConfiguration("loaded correctly");
            */
            refreshButtonAvailability(false); // Disable buttons as no changes have been made yet
        }
        catch (const TA_Base_Core::DatabaseException&)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            m_component->getStatusBar()->hideProgressBar();
            FUNCTION_EXIT;
            return;
        }
        catch (const TA_Base_Core::DataException&)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);

            m_component->getStatusBar()->hideProgressBar();
            FUNCTION_EXIT;
            return;
        }

        FUNCTION_EXIT;
    }

    void RightsView::loadSubsystemState()
    {
        FUNCTION_ENTRY("loadSubsystemStates");

        // Use our dummy static control to retrieve the positioning for the grid and then destroy it
        CRect position;

        GetDlgItem(IDC_SUBSYSTEM_STATE_EDIT)->GetWindowRect(&position);
        GetDlgItem(IDC_SUBSYSTEM_STATE_EDIT)->DestroyWindow();

        // Now convert to client position rather than on the whole screen
        ScreenToClient(&position);

        // Now create the subsystem grid at the position given.
        m_subsystemState.CreateEx(WS_EX_CLIENTEDGE, _T("EDIT"), "0", ES_LEFT | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_READONLY,
                                  position, this, IDC_SUBSYSTEM_STATE_EDIT);
        m_subsystemState.SetFont(GetFont());

        std::map<unsigned long, std::string> subsystemStates = m_component->getDatabaseAccessor()->getAllSubsystemStates();
        translateToChinese(subsystemStates);
        std::map<unsigned long, std::string>::iterator iter = subsystemStates.begin();
        m_subsystemState.SetWindowText(iter->second.c_str());
        m_subsystemStateKey = iter->first;

        FUNCTION_EXIT;
    }

    void RightsView::onButtonApply()
    {
        FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        TA_ASSERT(m_component != NULL, "The component stored is NULL");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        /* This comment is removing the code that enforces that each subsystem only has a duty set for one central
           profile. If this is reversed this should be added back in. The removal relates to PW2621.
                if (!isValidConfiguration("applied"))
                {
                    FUNCTION_EXIT;
                    return;
                }
        */
        try
        {
            // Set up the message details that remain the same for every right we apply
            MessageSender::UpdateDetails updateDetails;
            updateDetails.isNewItem = false;
            updateDetails.isDelete = false;
            updateDetails.configUpdateMessage = MessageSender::SECURITY;

            CProgressCtrl& progress = m_component->getStatusBar()->displayProgressBar();

            // Retrieve the first lot of rights
            RightsDatabaseAccessor::LoadedRights rights = m_component->getDatabaseAccessor()->getAllRights();

            progress.SetStep(1);
            progress.SetRange(0, rights.size());

            for (RightsDatabaseAccessor::LoadedRights::iterator iter = rights.begin(); iter != rights.end(); ++iter)
            {
                TA_ASSERT(iter->second != NULL, "One of the rights given to us from the RightsDatabaseAccessor is NULL");

                // IF this particular set of rights has not changed then we continue onto the next one
                if (!iter->second->hasChanged())
                {
                    progress.StepIt();
                    continue;
                }

                updateDetails.nameOfChangedItem = iter->second->getFormattedName();
                updateDetails.changes = iter->second->getAllItemChanges();
                updateDetails.keyOfChangedItem = iter->second->getProfileKey();

                // Set the current subsystem state key
                iter->second->setSubsystemState(m_subsystemStateKey);

                // Apply the changes to the database
                iter->second->applyChanges();

                m_component->getMessageSender().sendMessagesForTheChange(updateDetails);

                progress.StepIt();
            }

            refreshButtonAvailability(false); // Disable buttons as all changes have just been applied
        }
        catch (const TA_Base_Core::DataConfigurationException& exception)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataConfigurationException", "");

            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(IDS_FIELD_MISSING);

            std::vector<std::string> missing = exception.getMissingFields();

            for (size_t i = 0; i < missing.size(); ++i)
            {
                reason += missing[i].c_str();
                reason += "\n";
            }

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason.GetBuffer(reason.GetLength());
            userMsg.showMsgBox(IDS_UE_030060);
        }
        catch (const TA_Base_Core::DatabaseException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not apply the item changes");

            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(IDS_DATABASE_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason.GetBuffer(reason.GetLength());
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }
        catch (const TA_Base_Core::DataException& exception)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not apply the item changes");

            unsigned int errorId;

            switch (exception.getFailType())
            {
            case (TA_Base_Core::DataException::NOT_UNIQUE):
            {
                errorId = IDS_FIELD_NOT_UNIQUE;
                break;
            }

            case (TA_Base_Core::DataException::WRONG_TYPE):
            {
                errorId = IDS_FIELD_INVALID;
                break;
            }

            case (TA_Base_Core::DataException::MISSING_MANDATORY):
            {
                errorId = IDS_FIELD_MISSING;
                break;
            }

            default:
            {
                errorId = IDS_UNKNOWN_ERROR;
                break;
            }
            }

            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(errorId);
            reason += exception.getWhichData().c_str();
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason.GetBuffer(reason.GetLength());
            userMsg.showMsgBox(IDS_UE_030060);
        }

        m_component->getStatusBar()->hideProgressBar();

        FUNCTION_EXIT;
    }

    void RightsView::onButtonReset()
    {
        FUNCTION_ENTRY("onButtonReset");

        std::vector<std::string> rightsNotApplied; // This is not actually needed but we must pass it to the method call below

        if (m_component->getDatabaseAccessor()->areCurrentChangesPending(rightsNotApplied))
        {
            CString actionName;
            actionName.LoadString(IDS_RESET);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT response = userMsg.showMsgBox(IDS_UW_010003);

            if (response == IDNO)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "User cancelled the reset");
                FUNCTION_EXIT;
                return;
            }
        }

        TA_ASSERT(m_component != NULL, "Component object stored is NULL");

        // Invalidate and re-populate the details. This will ensure all data is reloaded from the database.
        m_component->getDatabaseAccessor()->invalidateData();

        CProgressCtrl& progress = m_component->getStatusBar()->displayProgressBar();

        // Now tell the grids to refresh their data

        /* This comment is removing the code that enforces that each subsystem only has a duty set for one central
           profile. If this is reversed this should be added back in. The removal relates to PW2621.
                if (!isValidConfiguration("loaded correctly"))
                {
                    FUNCTION_EXIT;
                    return;
                }
        */
        try
        {
            m_subsystemGrid.loadItems(m_component->getDatabaseAccessor(), RightsDatabaseAccessor::SUBSYSTEM, progress, m_subsystemStateKey);
            m_applicationGrid.loadItems(m_component->getDatabaseAccessor(), RightsDatabaseAccessor::APPLICATION, progress, m_subsystemStateKey);
        }
        catch (const TA_Base_Core::DatabaseException&)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            m_component->getStatusBar()->hideProgressBar();
            FUNCTION_EXIT;
            return;
        }
        catch (const TA_Base_Core::DataException&)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);

            m_component->getStatusBar()->hideProgressBar();
            FUNCTION_EXIT;
            return;
        }

        refreshButtonAvailability(false); // Disable buttons as all changes have just been applied

        m_component->getStatusBar()->hideProgressBar();

        FUNCTION_EXIT;
    }

    void RightsView::onChangeSubsystemState()
    {
        FUNCTION_ENTRY("OnSelchangeSubsystemStatesCombo");

        // Retrieve the current alarm type this rule is using
        CString currentSubsystemState("");
        m_subsystemState.GetWindowText(currentSubsystemState);

        TA_ASSERT(m_component->getDatabaseAccessor() != NULL, "Database accessor is NULL");

        try
        {
            ChangeDialog::ItemsForSelection items = m_component->getDatabaseAccessor()->getAllSubsystemStates();
            translateToChinese(items);

            // Now construct the dialog and then wait for the user to
            // finish with it
            ChangeDialog dlg(items, CHANGE_SUBSYSTEM_STATE_DIALOG_TITLE, currentSubsystemState, this);
            int response = dlg.DoModal();

            if (response != IDOK)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                            "User cancelled the selection of a new subsystem state");
                FUNCTION_EXIT;
                return;
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                        "User selected a new subsystem state which is %s",
                        dlg.getSelectedName());

            m_subsystemStateKey = dlg.getSelectedKey();

            // Now update the subsystem state the details view is showing
            m_subsystemState.SetWindowText(dlg.getSelectedName());

            // reload the rights
            loadItems();
        }
        catch (const TA_Base_Core::DataException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve subsystem state details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch (const TA_Base_Core::DatabaseException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve subsystem state details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }

    bool RightsView::isValidConfiguration(const CString& task)
    {
        FUNCTION_ENTRY("isValidConfiguration");

        CString errorMessage, item;
        errorMessage.LoadString(IDS_MORE_THAN_ONE_CENTRAL_PROFILE);
        item.LoadString(IDS_ITEMS_PREVENTING_ACTION);
        errorMessage += _T("\n") + item;
        int numberOfListedSubsystems = 0;

        std::map<std::string, unsigned long> allSubsystems = m_component->getDatabaseAccessor()->getAllSubsystems(RightsDatabaseAccessor::ALL);

        std::vector<unsigned long> invalidSubsystems;

        if (!m_subsystemGrid.isConfigurationValid(invalidSubsystems))
        {
            // THe configuration is invalid so we must determine what subsystems are invalid so we can display
            // a nice error message to the user.
            for (std::vector<unsigned long>::iterator iter = invalidSubsystems.begin(); iter != invalidSubsystems.end(); ++iter)
            {
                // Search for a match so we can get the name
                std::map<std::string, unsigned long>::iterator subsystem;
                std::map<std::string, unsigned long>::iterator matching = allSubsystems.end();

                for (subsystem = allSubsystems.begin(); subsystem != allSubsystems.end(); ++subsystem)
                {
                    if (subsystem->second == *iter)
                    {
                        matching = subsystem;
                        break;
                    }
                }

                if (matching != allSubsystems.end())
                {
                    if (numberOfListedSubsystems < MAX_NUMBER_OF_ITEMS_TO_LIST)
                    {
                        errorMessage += matching->first.c_str();
                        errorMessage += _T("\n");
                        ++numberOfListedSubsystems;
                    }
                    else
                    {
                        // Set the number of listed subsystems to a large number so we can tell that we had
                        // more than the limit
                        numberOfListedSubsystems = MAX_NUMBER_OF_ITEMS_TO_LIST + 1;
                        break;
                    }
                }
            }
        }

        if (numberOfListedSubsystems > MAX_NUMBER_OF_ITEMS_TO_LIST)
        {
            CString more;
            more.LoadString(IDS_MORE);
            errorMessage += more;
        }

        if (numberOfListedSubsystems > 0)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << task << errorMessage;
            userMsg.showMsgBox(IDS_UE_030060);
            FUNCTION_EXIT;
            return false;
        }

        FUNCTION_EXIT;
        return true;
    }

    void RightsView::onSelchangeSubsystemAppTab(NMHDR* pNMHDR, LRESULT* pResult)
    {
        if (static_cast<CTabCtrl*>(GetDlgItem(IDC_SUBSYSTEM_APP_TAB))->GetCurSel() == 0)
        {
            // subsystem mode
            m_applicationGrid.ShowWindow(SW_HIDE);
            m_subsystemGrid.ShowWindow(SW_SHOW);
            m_subsystemGrid.BringWindowToTop();

            GetDlgItem(IDC_SUBSYSTEM_STATE_STATIC)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_SUBSYSTEM_STATE_EDIT)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_CHANGE_SUBSYSTEM_STATE)->ShowWindow(SW_SHOW);
        }
        else if (static_cast<CTabCtrl*>(GetDlgItem(IDC_SUBSYSTEM_APP_TAB))->GetCurSel() == 1)
        {
            // app mode
            m_subsystemGrid.ShowWindow(SW_HIDE);
            m_applicationGrid.ShowWindow(SW_SHOW);
            m_applicationGrid.BringWindowToTop();

            GetDlgItem(IDC_SUBSYSTEM_STATE_STATIC)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_SUBSYSTEM_STATE_EDIT)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_CHANGE_SUBSYSTEM_STATE)->ShowWindow(SW_HIDE);
        }

        *pResult = 0;
    }


    void RightsView::OnSize(UINT nType, int cx, int cy)
    {
        CFormView::OnSize(nType, cx, cy);

        if ( m_subsystemGrid.GetSafeHwnd() )
        {
            CRect position;
            position.top = 35;
            position.bottom = cy - 10;
            position.left = 0;
            position.right = cx;
            m_subsystemGrid.MoveWindow(position);
            m_applicationGrid.MoveWindow(position);
        }
    }


    /////////////////////////////////////////////////////////////////////////////
    // RightsView diagnostics

#ifdef _DEBUG
    void RightsView::AssertValid() const
    {
        CFormView::AssertValid();
    }

    void RightsView::Dump(CDumpContext& dc) const
    {
        CFormView::Dump(dc);
    }
#endif //_DEBUG
}
