/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/PreConflictAckDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// PreConflictAckDlg.cpp : implementation file
//

#include "bus/alarm/alarm_list_control/src/stdafx.h"
#include "bus/alarm/alarm_list_control/src/alarmlistctrl.h"
#include "bus/alarm/alarm_list_control/src/PreConflictAckDlg.h"
#include "bus/alarm/alarm_list_control/src/LocationSelectionDialog.h"
#include "bus/alarm/alarm_list_control/src/TrainAgentAccess.h"

#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 azenitha

#include "boost/tokenizer.hpp"
#include "core/utilities/src/RunParams.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PreConflictAckDlg dialog


PreConflictAckDlg::PreConflictAckDlg(CWnd* pParent /*=NULL*/)
	: m_details(NULL),
	m_train1(0),
	m_train2(0),
	CDialog(PreConflictAckDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(PreConflictAckDlg)
	//}}AFX_DATA_INIT
}


void PreConflictAckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PreConflictAckDlg)
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDCANCEL, m_cancel);
	DDX_Control(pDX, IDC_TRAIN1RADIO, m_train1RadioButton);
	DDX_Control(pDX, IDC_TRAIN2RADIO, m_train2RadioButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PreConflictAckDlg, CDialog)
	//{{AFX_MSG_MAP(PreConflictAckDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PreConflictAckDlg message handlers

void PreConflictAckDlg::OnOK() 
{
    // Get the train to deactivate from the checkboxes
	int trainToDeactivate = 0;

	if(m_train1RadioButton.GetCheck() == 1)
	{
		trainToDeactivate = m_train1;
		//deactivate train 1;
	}
	else
	{
		trainToDeactivate = m_train2;
		//deactivate train 2;
	}
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "trainToDeactivate =%d",trainToDeactivate);//CL-20990++ TestLog
    // get the session id from the runparams
        std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
    try
    {
        // call setCarrierOff on the train agent (use TrainCctvAccess to get to the train agent)
        CORBA_CALL( TA_Base_Bus::TrainAgentAccess::getInstance().getTrainCctvAgent(), setCarrierOff, ( trainToDeactivate, session.c_str() ) );//limin
    }
    // catch any exceptions and display the error to the operator
    catch ( TA_Base_Core::TransactiveException& e )
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        CString temp = e.what();
        CString description = "Deactivate Train: " + temp;
        userMsg << description;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_140037);
    }
    catch ( TA_Base_Core::AccessDeniedException& )
    {
        TA_Base_Bus::TransActiveMessage userMsg;
        CString description = "Deactivate Train: Access Denied";
        userMsg << description;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_140037);
    }
    catch (...)
    {   
        TA_Base_Bus::TransActiveMessage userMsg;
        CString description = "Deactivate Train";
        userMsg << description;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_140037);
    }
    
    
    CDialog::OnOK();
}

BOOL PreConflictAckDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// lsx need change
//	std::string parameters = m_details->alarmParameterList;
	ConflictingTrains trains = GetAlarmParameters();
//	ConflictingTrains trains = decodeParameterString(parameters);

	std::string radio1Text = "Train: " + trains.offendingTrainString;
	std::string radio2Text = "Train: " + trains.owningTrainString;
	m_train1 = trains.offendingTrain;
	m_train2 = trains.owningTrain;

	m_train1RadioButton.SetWindowText(radio1Text.c_str());
	m_train2RadioButton.SetWindowText(radio2Text.c_str());

	m_train1RadioButton.SetCheck(true);
	m_train2RadioButton.SetCheck(false);

	m_train1RadioButton.EnableWindow(true);
	m_train2RadioButton.EnableWindow(true);

	m_ok.EnableWindow(true);
	m_cancel.EnableWindow(true);

	return TRUE;	// return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE

}

void PreConflictAckDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

int PreConflictAckDlg::DoModal(TA_Base_Bus::AlarmDataItem* details)
{
	m_details = details;
	return CDialog::DoModal();
}

ConflictingTrains PreConflictAckDlg::decodeParameterString(std::string parameterString)
{
	//paramter string in the form 
	//"OffendingTrain:x,OwningTrain:y,"
	//where x and y are numbers

	ConflictingTrains trains;

	std::vector<std::string> parts;
    
	typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

    boost::char_separator<char> sep( "," );
    tokenizer tokens(parameterString, sep);
    
    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
    {
		boost::char_separator<char> sep2( ":" );
		tokenizer tokens2((*tok_iter), sep2);

		tokenizer::iterator tok2_iter = tokens2.begin();
		
		if(*tok2_iter == "OffendingTrain")
		{
			tok2_iter++;
			if(tok2_iter != tokens2.end())
			{
				trains.offendingTrain = atoi((*tok2_iter).c_str());
				trains.offendingTrainString = (*tok2_iter).c_str();
			}
		}
		else if(*tok2_iter == "OwningTrain")
		{
			tok2_iter++;
			if(tok2_iter != tokens2.end())
			{
				trains.owningTrain = atoi((*tok2_iter).c_str());
				trains.owningTrainString = (*tok2_iter).c_str();
			}
		}
    }
    return trains;
}

//--xinyang*/
//CL-20990 xinyang++
ConflictingTrains PreConflictAckDlg::GetAlarmParameters()
{		
	// Load the message Type data
	ConflictingTrains trains;
	std::string strDescriptionModel = 
		TA_Base_Core::AlarmTypeTableAccessor::getInstance().GetAlarmDescriptionModel( m_details->messageTypeKey);

	std::vector<std::string> subStrings;
	typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
	
    boost::char_separator<char> sep( " " );
    tokenizer tokens(strDescriptionModel, sep);
	for (tokenizer::iterator it = tokens.begin();it != tokens.end();it++)
	{

			subStrings.push_back(*it);
	}

	long lPrePos1(0),lNextPos1(0),lPrePos2(0),lNextPos2(0),lPrePos1_1(0),lPrePos2_2(0);
	std::string strAlarmParameters("");
	std::string strCurrentAlarmDescription = m_details->alarmDescription;
	std::vector<std::string> strCrrentDescription;
	boost::char_separator<char> Crrent(" ");
	tokenizer Crrenttokens(strCurrentAlarmDescription,Crrent);
	for (tokenizer::iterator itr = Crrenttokens.begin();itr != Crrenttokens.end();itr++)
	{
			strCrrentDescription.push_back((*itr));
	}

	for (unsigned int i = 0; i< subStrings.size();i++)
	{
		if (subStrings[i] != strCrrentDescription[i])
		{
			lPrePos1 = strDescriptionModel.find("]",lNextPos1);
			lPrePos1_1 = strDescriptionModel.find("[",lNextPos1);

			lPrePos2 = strCurrentAlarmDescription.find(strCrrentDescription[i],lNextPos2);
			lPrePos2_2 = strCrrentDescription[i].length()+lPrePos2;
			
			if (strDescriptionModel.substr(lPrePos1_1+1,lPrePos1-1-lPrePos1_1) == "OffendingTrain")
			{
				trains.offendingTrain = atoi((strCurrentAlarmDescription.substr(lPrePos2,lPrePos2_2-lPrePos2)).c_str());
 				trains.offendingTrainString = (strCurrentAlarmDescription.substr(lPrePos2,lPrePos2_2-lPrePos2)).c_str();
				
			}
			else if (strDescriptionModel.substr(lPrePos1_1+1,lPrePos1-1-lPrePos1_1) ==  "OwningTrain")
			{
				trains.owningTrain = atoi((strCurrentAlarmDescription.substr(lPrePos2,lPrePos2_2-lPrePos2)).c_str());
 				trains.owningTrainString = (strCurrentAlarmDescription.substr(lPrePos2,lPrePos2_2-lPrePos2)).c_str();
			}
			lNextPos1 = lPrePos1+1;
		}
	}
	return trains;
}
//++xinyang
