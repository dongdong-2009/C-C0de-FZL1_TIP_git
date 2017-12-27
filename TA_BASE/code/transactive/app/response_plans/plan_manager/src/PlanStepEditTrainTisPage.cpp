/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditTrainTisPage.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Facilitates the editing of Train TIS step parameters.
  *
  */

#include "StdAfx.h"

#if defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)

#ifdef _MSC_VER
#pragma warning(disable: 4503 4786)
#endif

#include <map>
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/tis_agent/IDL/src/TISMessageDataDefinitions.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/signs/tis_agent_access/src/TISAgentAccessFactory.h"
#include "PlanManager.h"
#include "PlanStepEditTrainTisPage.h"
#include "TrainTisStep.h"
#include "TrainStepParameter.h"
#include "TimeSpanStepParameter.h"
#include "TextualStepParameter.h"
#include "TisConfigAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    static const unsigned short UNDEFINED_LIBRARY_VERSION(0xFFFF);

    static const int CB_GLOBAL_SEARCH(-1);
    static const int CB_NO_SELECTION(-1);
}

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditTrainTisPage dialog


CPlanStepEditTrainTisPage::CPlanStepEditTrainTisPage(TrainTisStep* step) :
    CPlanStepEditParameterPage(CPlanStepEditTrainTisPage::IDD, step),
    m_trainLibraryVersion(UNDEFINED_LIBRARY_VERSION),
    m_messageParameter(*step->getMessageParameter()),
    m_priorityParameter(*step->getPriorityParameter())
{
	//{{AFX_DATA_INIT(CPlanStepEditTrainTisPage)
	//}}AFX_DATA_INIT
}


CPlanStepEditTrainTisPage::~CPlanStepEditTrainTisPage()
{
}


void CPlanStepEditTrainTisPage::EnableParameterControls(bool enable)
{
    FUNCTION_ENTRY("EnableParameterControls(bool enable)");

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    m_trainIdBtn.EnableWindow(enable);
    m_trainIdCombo.EnableWindow(enable);
    m_inboundTrainsBtn.EnableWindow(enable);
    m_outboundTrainsBtn.EnableWindow(enable);
    m_allTrainsBtn.EnableWindow(enable);

    m_messageEdit.EnableWindow(enable);
    m_pickMessageBtn.EnableWindow(enable);

    m_priorityCombo.EnableWindow(enable);
    m_durationPicker.EnableWindow(enable);

    FUNCTION_EXIT;
}


void CPlanStepEditTrainTisPage::DisplayParameters()
{
    FUNCTION_ENTRY("DisplayParameters()");

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

	TrainTisStep* step = dynamic_cast<TrainTisStep*>(GetPlanStep());
	TA_ASSERT(step != NULL, "Incorrect step type for the Train TIS step page");

    CWaitCursor waitCursor;
   
    // Initialise the destination train section.
    TA_Base_Core::TrainDestination train(step->getTrainParameter()->getDestination());

    switch (train.trainGroup)
    {
        case TA_Base_Core::TG_INBOUND_TRAINS:
            SetButtonState(m_inboundTrainsBtn, BST_CHECKED);
            break;

        case TA_Base_Core::TG_OUTBOUND_TRAINS:
            SetButtonState(m_outboundTrainsBtn, BST_CHECKED);
            break;

        case TA_Base_Core::TG_ALL_TRAINS:
            SetButtonState(m_allTrainsBtn, BST_CHECKED);
            break;
        
        default:
        {
            SetButtonState(m_trainIdBtn, BST_CHECKED);

            if (train.trainId != TA_Base_Core::TIS_UNDEFINED_TRAIN_ID)
            {
                std::ostringstream trainIdStr;
                trainIdStr << train.trainId;

                // Try to locate the train ID in the train combo.
                int item(m_trainIdCombo.FindStringExact(CB_GLOBAL_SEARCH, trainIdStr.str().c_str()));

                if (item == CB_ERR)
                {
                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << trainIdStr.str();
                    userMsg.showMsgBox(IDS_UE_210105);
                }
                else
                {
                    // This will call OnTrainComboSelectionChanged() to determine the current
                    // library version number used by this train.
                    SetComboSelection(m_trainIdCombo, item);
                }
            }
        }
    }

    // Initialise message and priority.
    m_messageEdit.SetWindowText(m_messageParameter.getMessageText().c_str());
    UpdatePriorityCombo();
    
    // Initialise duration. Use any valid date with the picker control, it will be suppressed anyway.
	 //as per TD 12382
	 //for new steps set default duration to 1 minute
	 unsigned long initialDuration = step->getDurationParameter()->getNumericValue();
	 if (initialDuration == 0)
		 initialDuration = 1;

	 // TD 15722
//    COleDateTimeSpan duration(0, 0, initialDuration, 0);
//    TA_ASSERT(duration.GetDays() == 0, "Invalid message duration");
//    COleDateTime durationAsTime(COleDateTime::GetCurrentTime());
//    durationAsTime.SetTime(duration.GetHours(), duration.GetMinutes(), 0);
//    m_durationPicker.SetTime(durationAsTime); 

	 COleDateTimeSpan duration(0, 0, initialDuration, 0);
	 COleDateTimeSpan maxDuration(0, 23, 59, 0);
	 TA_ASSERT(duration.GetDays() == 0, "Invalid message duration");
	 COleDateTime durationAsTime(COleDateTime::GetCurrentTime());
	 COleDateTime maxDurationTime(COleDateTime::GetCurrentTime());
	 durationAsTime.SetTime(duration.GetHours(), duration.GetMinutes(), 0);
	 maxDurationTime.SetTime(maxDuration.GetHours(), maxDuration.GetMinutes(), 0);
	 m_durationPicker.SetRange(&durationAsTime, &maxDurationTime);
	 m_durationPicker.SetTime(durationAsTime); 
	 // TD 15722

    FUNCTION_EXIT;
}


void CPlanStepEditTrainTisPage::UpdateParameters()
{
    FUNCTION_ENTRY("UpdateParameters()");

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

	TrainTisStep* step = dynamic_cast<TrainTisStep*>(GetPlanStep());
	TA_ASSERT(step != NULL, "Incorrect step type for the Train TIS step page");

    TrainStepParameter* trainParameter = step->getTrainParameter();
    TA_ASSERT(trainParameter != NULL, "Train parameter not set");
    
    TisMessageStepParameter* messageParameter = step->getMessageParameter();
    TA_ASSERT(messageParameter != NULL, "Message parameter not set");

    TisPriorityStepParameter* priorityParameter = step->getPriorityParameter();  
    TA_ASSERT(priorityParameter != NULL, "Priority parameter not set");

    TimeSpanStepParameter* durationParameter = step->getDurationParameter();
    TA_ASSERT(durationParameter != NULL, "Duration parameter not set");

    // Update the train destination parameter
    trainParameter->setTrainId(TA_Base_Core::TIS_UNDEFINED_TRAIN_ID);
    trainParameter->setTrainGroup(TA_Base_Core::TG_UNDEFINED);

    if (m_inboundTrainsBtn.GetCheck() == BST_CHECKED)
    {
        trainParameter->setTrainGroup(TA_Base_Core::TG_INBOUND_TRAINS);
    }
    else if (m_outboundTrainsBtn.GetCheck() == BST_CHECKED)
    {
        trainParameter->setTrainGroup(TA_Base_Core::TG_OUTBOUND_TRAINS);
    }
    else if (m_allTrainsBtn.GetCheck() == BST_CHECKED)
    {
        trainParameter->setTrainGroup(TA_Base_Core::TG_ALL_TRAINS);
    }
    else
    {
        TA_ASSERT(m_trainIdBtn.GetCheck() == BST_CHECKED, "Invalid train ID radio check state");

        int selectedTrain(m_trainIdCombo.GetCurSel());
        unsigned short trainId(selectedTrain == CB_ERR? TA_Base_Core::TIS_UNDEFINED_TRAIN_ID
                                                      : static_cast<unsigned short>(m_trainIdCombo.GetItemData(selectedTrain)));
        trainParameter->setTrainId(trainId);
    }

    // Update the message parameter.
    messageParameter->setMessage(m_messageParameter.getMessage());

    // Update the priority parameter.
    priorityParameter->setValue(m_priorityParameter.getNumericValue());
    priorityParameter->hideValue(m_priorityParameter.isHiddenValue());

    // Update the duration parameter.
    COleDateTime durationAsTime;
    m_durationPicker.GetTime(durationAsTime);

    COleDateTimeSpan duration(0, durationAsTime.GetHour(), durationAsTime.GetMinute(), 0);
    durationParameter->setValue(static_cast<unsigned long>(duration.GetTotalMinutes()));
    durationParameter->hideValue(durationParameter->getNumericValue() == 0);    

    FUNCTION_EXIT;
}


void CPlanStepEditTrainTisPage::UpdatePriorityCombo()
{
    FUNCTION_ENTRY("UpdatePriorityCombo()");

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    m_priorityCombo.ResetContent();
    m_priorityCombo.EnableWindow(m_messageParameter.getMessage().messageId != TA_Base_Core::TIS_UNDEFINED_MESSAGE_ID);

    if (m_priorityCombo.IsWindowEnabled())
    {
        // Populate the priority combo with priorities corresponding to the library section
        // associated with the current message.
        typedef std::vector<unsigned short> PriorityList;
        PriorityList priorities(m_priorityParameter.getTisPriorities(
            static_cast<TA_Base_Core::ELibrarySection>(m_messageParameter.getMessage().librarySection)));

        if (priorities.empty())
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_210115);
        }
        else
        {
            int currentPriorityIndex(CB_ERR);

            for (PriorityList::const_iterator pIt(priorities.begin()); pIt != priorities.end(); pIt++)
            {
                int item(m_priorityCombo.AddString(m_priorityParameter.getPriorityLabel(*pIt).c_str()));
                m_priorityCombo.SetItemData(item, *pIt);

                // See if the given priority matches the one currently configured
                // for this step. We'll want to select this item.
                if (*pIt == m_priorityParameter.getPriority())
                {
                    currentPriorityIndex = item;
                }
            }
           
            if (currentPriorityIndex == CB_ERR)
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << m_priorityParameter.getPriority();
                userMsg.showMsgBox(IDS_UE_210106);
            }
            else
            {
                SetComboSelection(m_priorityCombo, currentPriorityIndex);
            }            
        }
    }

    FUNCTION_EXIT;    
}


void CPlanStepEditTrainTisPage::SetButtonState(CButton& button, int checkState)
{
    FUNCTION_ENTRY("SetButtonState(CButton& button, int checkState)");

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    // Update the specified button's check state and ensure that the "button clicked" message
    // handler for that button is invoked.
    button.SetCheck(checkState);
    SendMessage(WM_COMMAND, MAKEWPARAM(button.GetDlgCtrlID(), BN_CLICKED), reinterpret_cast<LPARAM>(GetSafeHwnd()));

    FUNCTION_EXIT;
}


void CPlanStepEditTrainTisPage::SetComboSelection(CComboBox& combo, int item)
{
    FUNCTION_ENTRY("SetComboSelection(CComboBox& combo, int checkState)");

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    // Select the specified item in the combo box and ensure that the "selection changed" message
    // handler for that combo is invoked.
    combo.SetCurSel(item);
    SendMessage(WM_COMMAND, MAKEWPARAM(combo.GetDlgCtrlID(), CBN_SELCHANGE), reinterpret_cast<LPARAM>(GetSafeHwnd()));

    FUNCTION_EXIT;
}


void CPlanStepEditTrainTisPage::SetTrainLibraryVersion()
{
    FUNCTION_ENTRY("SetTrainLibraryVersion()");

    TA_ASSERT(::IsWindow(GetSafeHwnd()), "Dialog not initialised");

    // There is no point checking each train for its library version, the system will
    // only allow messages to be sent using the current ISCS version anyway
    // also, both the ISCS version and the train version may change AFTER the step is configured
    // so there is no point doing all this wasteful and meaningless checking now
    // just use the current ISCS version...

    CWaitCursor waitCursor;

    m_trainLibraryVersion = UNDEFINED_LIBRARY_VERSION;
    m_pickMessageBtn.EnableWindow(FALSE);

    try
    {    
        TA_Base_Bus::TISAgentAccessFactory::StisNamedObj& stisAgent =
            TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent();
        
        CORBA_CALL_RETURN( m_trainLibraryVersion, stisAgent, getCurrentTTISMessageLibraryVersion, () );//limin
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException",
                             "Failed to retrieve current TTIS message library version" );

        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg.showMsgBox(IDS_UE_210111);

        FUNCTION_EXIT;
        return;    
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception",
                             "Failed to retrieve current TTIS message library version" );

        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg.showMsgBox(IDS_UE_210111);

        FUNCTION_EXIT;
        return;
    }

    // Must have a valid library to be able to pick messages.
    if (m_trainLibraryVersion != UNDEFINED_LIBRARY_VERSION)
    {
        m_pickMessageBtn.EnableWindow(TRUE);

        // Make sure we're displaying a valid message for the given train library version.
        // If the current message is sourced from a different library, then advise the
        // operator to pick one from the correct message library.
        TA_Base_Core::TISMessage currentMessage(m_messageParameter.getMessage());

        if (currentMessage.messageId != TA_Base_Core::TIS_UNDEFINED_MESSAGE_ID &&
            currentMessage.libraryVersion != m_trainLibraryVersion)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UW_210030);
        }
    }

    FUNCTION_EXIT;
}


void CPlanStepEditTrainTisPage::DoDataExchange(CDataExchange* pDX)
{
	CPlanStepEditParameterPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlanStepEditTrainTisPage)
	DDX_Control(pDX, IDC_DURATION_PICKER, m_durationPicker);
	DDX_Control(pDX, IDC_MESSAGE_EDIT, m_messageEdit);
	DDX_Control(pDX, IDC_PRIORITY_COMBO, m_priorityCombo);
	DDX_Control(pDX, IDC_TRAIN_ID_COMBO, m_trainIdCombo);
	DDX_Control(pDX, IDC_TRAIN_ID_RADIO, m_trainIdBtn);
    DDX_Control(pDX, IDC_INBOUND_TRAINS_RADIO, m_inboundTrainsBtn);
    DDX_Control(pDX, IDC_OUTBOUND_TRAINS_RADIO, m_outboundTrainsBtn);
    DDX_Control(pDX, IDC_ALL_TRAINS_RADIO, m_allTrainsBtn);
	DDX_Control(pDX, IDC_PICK_MESSAGE_BUTTON, m_pickMessageBtn);
	//}}AFX_DATA_MAP
}




/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditTrainTisPage message handlers

BEGIN_MESSAGE_MAP(CPlanStepEditTrainTisPage, CPlanStepEditParameterPage)
	//{{AFX_MSG_MAP(CPlanStepEditTrainTisPage)
	ON_CBN_SELCHANGE(IDC_TRAIN_ID_COMBO, OnTrainComboSelectionChanged)
	ON_CBN_SELCHANGE(IDC_PRIORITY_COMBO, OnPriorityComboSelectionChanged)
	ON_BN_CLICKED(IDC_TRAIN_ID_RADIO, OnTrainIdRadio)
	ON_BN_CLICKED(IDC_INBOUND_TRAINS_RADIO, OnTrainGroupRadio)
	ON_BN_CLICKED(IDC_OUTBOUND_TRAINS_RADIO, OnTrainGroupRadio)
	ON_BN_CLICKED(IDC_ALL_TRAINS_RADIO, OnTrainGroupRadio)
	ON_BN_CLICKED(IDC_PICK_MESSAGE_BUTTON, OnPickMessageButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPlanStepEditTrainTisPage::OnInitDialog() 
{
    FUNCTION_ENTRY("OnInitDialog()");

	CPlanStepEditParameterPage::OnInitDialog();
 
    // Populate the train list. The contents will remain unchanged.
    try
    {
        CWaitCursor waitCursor;

        //as a tmp fix for TD 14786 just hard code train IDs to range from 1-99
        CString sTrainId;
        for (int i = 1; i < 100; i++)
        {
            sTrainId.Format ("%.2d", i);          //td 14894
            int item (m_trainIdCombo.AddString(sTrainId));
            m_trainIdCombo.SetItemData(item, i);
        }
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());    

        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg.showMsgBox(IDS_UE_210109);  
    }
	 catch (...)
	 {
		 LOG_EXCEPTION_CATCH(SourceInfo, " Unknown Exception", "");    
		 
		 TA_Base_Bus::TransActiveMessage userMsg;
		 userMsg.showMsgBox(IDS_UE_210109);  
	 }

    // The duration date/time picker control will only be used for hours and minutes,
    // so set the format such that the date and the seconds are hidden.
    m_durationPicker.SetFormat("HH:mm");
  	
    FUNCTION_EXIT;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CPlanStepEditTrainTisPage::OnTrainComboSelectionChanged() 
{
    FUNCTION_ENTRY("OnTrainComboSelectionChanged()");

    // If a valid train is selected (or a train group button is active), we need to determine
    //  the version of the message library in use and immediately inform the operator of any problems.
    SetTrainLibraryVersion();

    FUNCTION_EXIT;
}


void CPlanStepEditTrainTisPage::OnTrainIdRadio() 
{
    FUNCTION_ENTRY("OnTrainRadio()");

    // Tabbing through the controls a BN_CLICKED message is generated when m_trainIdBtn
    // receives focus. If this happens while any of the other train buttons is selected,
    // simply ignore the notification.
    if (m_inboundTrainsBtn.GetCheck() == BST_CHECKED ||
        m_outboundTrainsBtn.GetCheck() == BST_CHECKED ||
        m_allTrainsBtn.GetCheck() == BST_CHECKED)
    {
        FUNCTION_EXIT;
        return;
    }

    m_trainIdCombo.EnableWindow(TRUE);

    // Clear train selection. OnTrainComboSelectionChanged() will set the library version
    // for the target destination.
    SetComboSelection(m_trainIdCombo, CB_NO_SELECTION); 

    FUNCTION_EXIT;
}


void CPlanStepEditTrainTisPage::OnTrainGroupRadio() 
{
    FUNCTION_ENTRY("OnTrainGroupRadio()");

    // Tabbing through the controls a BN_CLICKED message is generated when a train group
    // radio button receives focus. If this happens while m_trainIdBtn is selected, simply
    // ignore the notification.
    if (m_trainIdBtn.GetCheck() == BST_CHECKED)
    {
        FUNCTION_EXIT;
        return;
    }

    m_trainIdCombo.EnableWindow(FALSE);

    // Clear train selection. OnTrainComboSelectionChanged() will set the library version
    // for the target destinations.
    SetComboSelection(m_trainIdCombo, CB_NO_SELECTION); 

    FUNCTION_EXIT;
}



void CPlanStepEditTrainTisPage::OnPickMessageButton() 
{
    FUNCTION_ENTRY("OnPickMessageButton()");
	
    // The destination train library version should be valid for message selection button to become enabled.
    TA_ASSERT(m_trainLibraryVersion != UNDEFINED_LIBRARY_VERSION, "Invalid destination library version");

    TisConfigAccess::PredefinedTisMessage selectedMessage;

    if (m_messageParameter.editParameter(m_trainLibraryVersion, selectedMessage))
    {
        TA_ASSERT(selectedMessage.messageTag != TA_Base_Core::TIS_UNDEFINED_MESSAGE_ID, "Invalid TIS message");

        TA_Base_Core::TISMessage newMessage;
        newMessage.libraryVersion = m_trainLibraryVersion;
        newMessage.librarySection = selectedMessage.librarySection;
        newMessage.messageId = selectedMessage.messageTag;

        m_messageParameter.setMessage(newMessage);
        m_priorityParameter.setValue(selectedMessage.priority);

        m_messageEdit.SetWindowText(m_messageParameter.getMessageText().c_str());
        UpdatePriorityCombo();
    }

    FUNCTION_EXIT;
}


void CPlanStepEditTrainTisPage::OnPriorityComboSelectionChanged() 
{
    int selectedPriority(m_priorityCombo.GetCurSel());
    
    if (selectedPriority == CB_ERR)
    {
        m_priorityParameter.hideValue(true);
    }
    else
    {
        m_priorityParameter.setValue(m_priorityCombo.GetItemData(selectedPriority));
        m_priorityParameter.hideValue(false);
    }
}

#endif // defined (STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM)

