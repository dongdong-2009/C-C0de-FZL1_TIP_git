/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditPATrainPage.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Facilitates the editing of Train PA step parameters.
  *
  */

#include "StdAfx.h"

#if defined (STEPTYPE_TRAIN_PUBLIC_ADDRESS)

#ifdef _MSC_VER
#pragma warning(disable: 4503 4786)
#endif

#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/TransActiveMessage.h"

#include "PlanManager.h"
#include "PlanStepEditPATrainPage.h"
#include "TrainPAStep.h"
#include "TrainStepParameter.h"
#include "PaMessageStepParameter.h"
#include "YesNoStepParameter.h"
#include	"TimeSpanStepParameter.h"
#include "core/data_access_interface/pa/src/PaTrainDvaMessageAccessFactory.h"
#include "core/data_access_interface/pa/src/IPaTrainDvaMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//train-group combo items

using namespace TA_Base_App;

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditPATrainPage dialog

static const CString TRAIN					= "单个列车";
static const CString IN_TRAINS			= "入站列车";
static const CString OUT_TRAINS			= "出站列车";
static const CString ALL_TRAINS			= "所有列车";


CPlanStepEditPATrainPage::CPlanStepEditPATrainPage(TrainPAStep* step) :
    CPlanStepEditParameterPage(CPlanStepEditPATrainPage::IDD, step),
	 m_messageParameter  (*step->getPAMessageParameter ()),
	 m_trainParameter		(*step->getTargetTrainParameter ())
{
	//{{AFX_DATA_INIT(CPlanStepEditPATrainPage)
	m_cyclic = FALSE;
	m_duration = 0;
	m_trainGroups = -1;
	m_message = _T("");
	m_trains = -1;
	//}}AFX_DATA_INIT
}
	 
CPlanStepEditPATrainPage::~CPlanStepEditPATrainPage()
{
}

void CPlanStepEditPATrainPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPlanStepEditPATrainPage)
	DDX_Control(pDX, IDC_PA_TRAINS_COMBO, m_trainsCombo);
	DDX_Control(pDX, IDC_PA_TRAIN_MSG_DLG_BTN, m_messageBtn);
	DDX_Control(pDX, IDC_PA_TRAIN_MESSAGE_TXT, m_messageTxt);
	DDX_Control(pDX, IDC_PA_TRAIN_GROUPES_COMBO, m_trainGroupsCombo);
	DDX_Control(pDX, IDC_DURATION_DTPICKER, m_durationDTPicker);
	DDX_Control(pDX, IDC_CYCLIC_CHECK, m_cyclicBtn);
	DDX_Check(pDX, IDC_CYCLIC_CHECK, m_cyclic);
	DDX_DateTimeCtrl(pDX, IDC_DURATION_DTPICKER, m_duration);
	DDX_CBIndex(pDX, IDC_PA_TRAIN_GROUPES_COMBO, m_trainGroups);
	DDX_Text(pDX, IDC_PA_TRAIN_MESSAGE_TXT, m_message);
	DDX_CBIndex(pDX, IDC_PA_TRAINS_COMBO, m_trains);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlanStepEditPATrainPage, CPlanStepEditParameterPage)
	//{{AFX_MSG_MAP(CPlanStepEditPATrainPage)
	ON_CBN_SELCHANGE(IDC_PA_TRAIN_GROUPES_COMBO, OnSelchangePaTrainGroupesCombo)
	ON_BN_CLICKED(IDC_PA_TRAIN_MSG_DLG_BTN, OnPaTrainMsgDlgBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditPATrainPage message handlers

BOOL CPlanStepEditPATrainPage::OnInitDialog ()
{
	if (!CPlanStepEditParameterPage::OnInitDialog ())
	{
		return FALSE;
	}
	
	PopulateTrainGroupsCombo ();
	
	
	// The duration date/time picker control will only be used for hours and minutes,
	// so set the format such that the date and the seconds are hidden.
	m_durationDTPicker.SetFormat("HH:mm");

	// Populate the train list. The contents will remain unchanged.
	try
	{//as a tmp fix for TD 14786 just hard code train IDs to range from 1-99
		CString sTrainId;
		for (int i = 1; i < 100; i++)
		{
			sTrainId.Format ("%.2d", i);  //td 14894
			int item (m_trainsCombo.AddString(sTrainId));
			m_trainsCombo.SetItemData(item, i);
		}
	}
	catch (const TA_Base_Core::TransactiveException& ex)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());    
		
		TA_Base_Bus::TransActiveMessage userMsg;
		userMsg.showMsgBox(IDS_UE_210109);  
	}
	
	return TRUE;  
}

void  CPlanStepEditPATrainPage::DisplayParameters()
{
	
	TrainPAStep* step = dynamic_cast<TrainPAStep*>(GetPlanStep());
	TA_ASSERT(step != NULL, "Incorrect step type for the Train PA step page");

    CWaitCursor waitCursor;
   
    // Initialise the destination train section.
    TA_Base_Core::TrainDestination train(step->getTargetTrainParameter()->getDestination());

    switch (train.trainGroup)
    {
        case TA_Base_Core::TG_INBOUND_TRAINS:
           m_trainGroupsCombo.SetCurSel (m_trainGroupsCombo.FindStringExact (0, IN_TRAINS));
			  m_trainsCombo.EnableWindow (FALSE);
			  break;

        case TA_Base_Core::TG_OUTBOUND_TRAINS:
            m_trainGroupsCombo.SetCurSel (m_trainGroupsCombo.FindStringExact (0, OUT_TRAINS));
				m_trainsCombo.EnableWindow (FALSE);
            break;

        case TA_Base_Core::TG_ALL_TRAINS:
            m_trainGroupsCombo.SetCurSel (m_trainGroupsCombo.FindStringExact (0, ALL_TRAINS));
				m_trainsCombo.EnableWindow (FALSE);
            break;
        
        default:
        {
            m_trainGroupsCombo.SetCurSel (m_trainGroupsCombo.FindStringExact (0, TRAIN));
				m_trainsCombo.EnableWindow (TRUE);
            if (train.trainId != TA_Base_Core::TIS_UNDEFINED_TRAIN_ID)
            {
                std::ostringstream trainIdStr;
                trainIdStr << train.trainId;

                // Try to locate the train ID in the train combo.
                int item(m_trainsCombo.FindStringExact(-1, trainIdStr.str().c_str()));

                if (item == CB_ERR)
                {
                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << trainIdStr.str();
                    userMsg.showMsgBox(IDS_UE_210105);
                }
                else
                {
                   m_trainsCombo.SetCurSel (item);
                }
            }
        }
    }
	 
    // Initialise message and cyclic
	 unsigned long messageId = m_messageParameter.getPAMessageId ();
	 CString messageName;
	 if (messageId > 0)
	 {
		 TA_Base_Core::IPaTrainDvaMessage* paMessage = TA_Base_Core::PaTrainDvaMessageAccessFactory::getInstance().getPaTrainDvaMessage(messageId, false);
		 messageName = paMessage->getLabel().c_str ();
	 }
	 else
	 {
		 messageName = NO_MESSAGE.c_str();
	 }

    m_messageTxt.SetWindowText(messageName);
    
	 m_cyclicBtn.SetCheck (step->getCyclicParameter()->getAffirmative ());

    //Initialise duration. Use any valid date with the picker control, it will be suppressed anyway.
	 
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
	 //    m_durationDTPicker.SetTime(durationAsTime); 

	 COleDateTimeSpan duration(0, 0, initialDuration, 0);
	 COleDateTimeSpan maxDuration(0, 23, 59, 0);
	 TA_ASSERT(duration.GetDays() == 0, "Invalid message duration");
	 COleDateTime durationAsTime(COleDateTime::GetCurrentTime());
	 COleDateTime maxDurationTime(COleDateTime::GetCurrentTime());
	 durationAsTime.SetTime(duration.GetHours(), duration.GetMinutes(), 0);
	 maxDurationTime.SetTime(maxDuration.GetHours(), maxDuration.GetMinutes(), 0);
	 m_durationDTPicker.SetRange(&durationAsTime, &maxDurationTime);
	 m_durationDTPicker.SetTime(durationAsTime); 
	 // TD 15722
}
	
void CPlanStepEditPATrainPage::UpdateParameters()
{
	CWaitCursor waitCursor;
	
	TrainPAStep* step = dynamic_cast<TrainPAStep*>(GetPlanStep());
	TA_ASSERT(step != NULL, "Incorrect step type for the PA Train step page");
	
	TrainStepParameter* trainParameter = step->getTargetTrainParameter();
	TA_ASSERT(trainParameter != NULL, "Train parameter not set");
	
	PaMessageStepParameter* messageParameter = step->getPAMessageParameter();
	TA_ASSERT(messageParameter != NULL, "Message parameter not set");
	
	YesNoStepParameter* cyclicParameter = step->getCyclicParameter();  
	TA_ASSERT(cyclicParameter != NULL, "Priority parameter not set");
	
	TimeSpanStepParameter* durationParameter = step->getDurationParameter();
	TA_ASSERT(durationParameter != NULL, "Duration parameter not set");
	
	
	// Update the train destination parameter
	trainParameter->setTrainId(TA_Base_Core::TIS_UNDEFINED_TRAIN_ID);
	trainParameter->setTrainGroup(TA_Base_Core::TG_UNDEFINED);
	
	int curSel = m_trainGroupsCombo.GetCurSel ();
	CString curText;
	m_trainGroupsCombo.GetLBText (curSel, curText);
	
	if (curText.Compare (IN_TRAINS) == 0)
	{
		trainParameter->setTrainGroup(TA_Base_Core::TG_INBOUND_TRAINS);
	}
	else if (curText.Compare (OUT_TRAINS) == 0)
	{
		trainParameter->setTrainGroup(TA_Base_Core::TG_OUTBOUND_TRAINS);
	}
	else if (curText.Compare (ALL_TRAINS) == 0)
	{
		trainParameter->setTrainGroup(TA_Base_Core::TG_ALL_TRAINS);
	}
	else
	{
		TA_ASSERT(curText.Compare (TRAIN) == 0, "Invalid train ID radio check state");
		
		int selectedTrain(m_trainsCombo.GetCurSel());
		unsigned short trainId(selectedTrain == CB_ERR? TA_Base_Core::TIS_UNDEFINED_TRAIN_ID
						: static_cast<unsigned short>(m_trainsCombo.GetItemData(selectedTrain)));
		trainParameter->setTrainId(trainId);
	}
	
	// Update the message parameter.
	// the lib version and id don't appear to be applicable here
	// as the Train message interface does not provide functions to handle them
	messageParameter->setPaMessageId(m_messageParameter.getPAMessageId ());
	messageParameter->setPAMessageLibVersion (m_messageParameter.getPAMessageLibVersion ());

	// Update the cyclic parameter.
	cyclicParameter->setAffirmative ((BST_CHECKED == m_cyclicBtn.GetCheck ()));
	
	// Update the duration parameter.
	COleDateTime durationAsTime;
	m_durationDTPicker.GetTime(durationAsTime);
	
	COleDateTimeSpan duration(0, durationAsTime.GetHour(), durationAsTime.GetMinute(), 0);
	durationParameter->setValue(static_cast<unsigned long>(duration.GetTotalMinutes()));
	durationParameter->hideValue(durationParameter->getNumericValue() == 0); 
	
}

void CPlanStepEditPATrainPage::OnSelchangePaTrainGroupesCombo() 
{
	if (m_trainGroupsCombo.GetCurSel () == m_trainGroupsCombo.FindStringExact (0, TRAIN))
	{
		m_trainsCombo.EnableWindow (TRUE);
		m_trainsCombo.SetCurSel (0);
	}
	else
	{
		m_trainsCombo.SetCurSel (-1);
		m_trainsCombo.EnableWindow (FALSE);
	}

}

void CPlanStepEditPATrainPage::PopulateTrainGroupsCombo () 
{	
	int index;
	index = m_trainGroupsCombo.AddString (TRAIN);
	m_trainGroupsCombo.AddString (IN_TRAINS);
	m_trainGroupsCombo.AddString (OUT_TRAINS);
	m_trainGroupsCombo.AddString (ALL_TRAINS);
	
	m_trainGroupsCombo.SetCurSel (index);

}

void CPlanStepEditPATrainPage::OnPaTrainMsgDlgBtn() 
{
	CPaMessagesDlg messagesDlg (PA_TRAIN_MESSAGE_TYPE, m_messageParameter, false, 0);   
	
	if (IDOK == messagesDlg.DoModal())
	{
		CString message = messagesDlg.GetSelectedMessage ();
		m_messageTxt.SetWindowText (message);
	}

	UpdateData ();
}

#endif //defined (STEPTYPE_TRAIN_PUBLIC_ADDRESS)
