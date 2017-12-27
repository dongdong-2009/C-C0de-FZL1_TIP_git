/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_list_control/src/SuppressAlarmDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 17:50:12 $
  * Last modified by:  $Author: Noel $
  *
  */
// SuppressAlarmDialog.cpp : implementation file
//

#include "stdafx.h"
#include "alarmlistctrl.h"
#include "SuppressAlarmDialog.h"
#include "CSuppressAlarmConfirmDlg.h"

#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"
#include "core/data_access_interface/alarm_rule/src/IAlarmRuleData.h"
#include "core/data_access_interface/alarm_rule/src/SuppressionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AlarmRuleAccessFactory.h"
#include "core/data_access_interface/src/AlarmTypeData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/AlarmActionException.h"

#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ConfigUpdate_MessageTypes.h"

#include "bus/generic_gui/src/TransActiveMessage.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{

    /////////////////////////////////////////////////////////////////////////////
    // CSuppressAlarmDialog dialog


    CSuppressAlarmDialog::CSuppressAlarmDialog(boost::shared_ptr<TA_Base_Bus::AlarmDataItem> alarm, std::string createdBySessionKey,  unsigned long createdByProfileKey, std::string createdByOperatorName, CWnd* pParent /*=NULL*/)
	    : CDialog(CSuppressAlarmDialog::IDD, pParent),
          m_alarmDetails(alarm),
          m_successful(true),
          m_createdBySessionKey(createdBySessionKey),
		  m_createdByProfileKey(createdByProfileKey),		   
		 m_createdByOperatorName(createdByOperatorName)
    {
	    //{{AFX_DATA_INIT(CSuppressAlarmDialog)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
    }


    void CSuppressAlarmDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(CSuppressAlarmDialog)
		    // NOTE: the ClassWizard will add DDX and DDV calls here
	    //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(CSuppressAlarmDialog, CDialog)
	    //{{AFX_MSG_MAP(CSuppressAlarmDialog)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CSuppressAlarmDialog message handlers

    BOOL CSuppressAlarmDialog::OnInitDialog() 
    {
	    CDialog::OnInitDialog();

		m_font.CreatePointFont(100,"宋体");
		SetFont(&m_font);
				
        setUpDialogTitle();
	    
	    // By default, check the entity and alarm type radio button
        CheckDlgButton(IDC_RADIO_ALARM_TYPE, BST_CHECKED);
        CheckDlgButton(IDC_RADIO_ALARM_DESCRIPTION, BST_UNCHECKED);
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }

    void CSuppressAlarmDialog::OnCancel() 
    {
	    // TODO: Add extra cleanup here
	    
	    CDialog::OnCancel();
    }

    void CSuppressAlarmDialog::OnOK() 
    {
	    // TODO: Add extra validation here
        inhibitAlarm();
	    
	    CDialog::OnOK();
    }

    bool CSuppressAlarmDialog::wasSuccessful()
    {
        return m_successful;
    }

	void CSuppressAlarmDialog::inhibitAlarmByType()
	{
		FUNCTION_ENTRY("inhibitAlarmByType");

        // Get out the required alarm data
        //TA_Base_Core::IAlarmTypeData* alarmTypeData = TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAlarmType( messageTypeKey, false );
    
	    unsigned long entityKey = m_alarmDetails->assocEntityKey;
        //unsigned long alarmTypeKey = 0;
        unsigned long entityTypeKey = 0;

        TA_Base_Core::IAlarmTypeData* typeData = 0;
		typeData = TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAlarmTypeByKey(m_alarmDetails->messageTypeKey, false);
		//alarmTypeKey = typeData->getKey();
        // need to convert the alarmTypeKey into a string
	    //std::ostringstream atkStream;
	    //atkStream << alarmTypeKey;

		std::string alarmAssetName( m_alarmDetails->assetName );
		std::string alarmDescription( m_alarmDetails->alarmDescription);
		std::string alarmValue( m_alarmDetails->alarmValue );

		// confirm the selection and inhibition
		CSuppressAlarmConfirmDlg confirmDlg;
		// string up the confirmation description
		//std::string confirmDesc("Inhibit by TYPE :\n");
		std::string confirmDesc("根据类型抑制 :\n");
		// confirmDesc += "Asset Name: " + alarmAssetName;
		//confirmDesc += "of Alarm Type: " + typeData->getName();
		confirmDesc += "警报类型: " + typeData->getName();
		confirmDlg.setConfirmationText(confirmDesc);
		delete typeData;

		int outcome = confirmDlg.DoModal();
		if (outcome == IDCANCEL) 
		{
			if (m_successful == false) m_successful = true;
			return;
		}
		// otherwise continue as the OK button is clicked

        // create the rule based on the alarm's data...
        TA_Base_Core::IAlarmRuleData* suppressionRule = TA_Base_Core::AlarmRuleAccessFactory::getInstance().createSuppressionRule(
            entityTypeKey,
            entityKey,
			m_alarmDetails->messageTypeKey,
            "submit",
            m_createdBySessionKey,
			alarmAssetName,
			alarmDescription,
			alarmValue
            );

        TA_Base_Core::SuppressionRuleData* suppressionDescRule=dynamic_cast<TA_Base_Core::SuppressionRuleData*> (suppressionRule);
        suppressionDescRule->setOperatorName(m_createdByOperatorName);
		suppressionDescRule->setProfileKey(m_createdByProfileKey);
        // and save to the database...
        bool successful = false;
        std::string reason( "" );
        try
        {
            suppressionRule->applyChanges();
		    sendConfigUpdate(suppressionRule);
            successful = true;
        }
        catch( TA_Base_Core::DatabaseException& dbe )
        {
            reason = dbe.what();
			//showErrorMsg(reason, "type");
			showErrorMsg(reason, "类型");
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", reason.c_str() );
			return;
        }
        catch( TA_Base_Core::DataException& de )
        {
            reason = de.what();
			//showErrorMsg(reason, "type");
			showErrorMsg(reason, "类型");
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", reason.c_str() );
			return;
        }
        catch( TA_Base_Core::DataConfigurationException& dce )
        {
            reason = dce.what();
			//showErrorMsg(reason, "type");
			showErrorMsg(reason, "类型");
            LOG_EXCEPTION_CATCH( SourceInfo, "DataConfigurationException", reason.c_str() );
			return;
        }
        catch( ... )
        {
			//showErrorMsg("", "type");
			showErrorMsg("", "类型");
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unknown exception occurred while saving suppression rule." );
			return;
        }

		// the new rule has been successfully applied/saved
		// send the config update
        //sendConfigUpdate(suppressionRule);
		
		FUNCTION_EXIT;
	}

	void CSuppressAlarmDialog::inhibitAlarmByDescription()
	{
		FUNCTION_ENTRY("inhibitAlarmByDescription");

        // Get out the required alarm data
        //TA_Base_Core::IAlarmTypeData* alarmTypeData = TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAlarmType( messageTypeKey, false );
    
	    unsigned long entityKey = m_alarmDetails->assocEntityKey;
        //unsigned long alarmTypeKey = 0;
        unsigned long entityTypeKey = 0;

        //TA_Base_Core::IAlarmTypeData* typeData = 0;
		//typeData = TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAlarmType(messageTypeKey, false);
		//alarmTypeKey = typeData->getKey();
        // need to convert the alarmTypeKey into a string
	    //std::ostringstream atkStream;
	    //atkStream << alarmTypeKey;

		std::string alarmAssetName( m_alarmDetails->assetName );
		std::string alarmValue( m_alarmDetails->alarmValue );
		std::string alarmDescription( m_alarmDetails->alarmDescription );

		// confirm the selection and inhibition
		CSuppressAlarmConfirmDlg confirmDlg;
		// string up the confirmation description
		//std::string confirmDesc("Inhibit by DESCRIPTION :\n");
		std::string confirmDesc("根据描述抑制 :\n");
		// confirmDesc += "Asset/Entity Name: " + alarmAssetName;
		// confirmDesc += "of Alarm Type: " + typeData->getName();
		//confirmDesc += "of Description: " + alarmDescription;
		confirmDesc += "描述: " + alarmDescription;
		confirmDlg.setConfirmationText(confirmDesc);
		
		//delete typeData;

		int outcome = confirmDlg.DoModal();
		if (outcome == IDCANCEL) 
		{
			if (m_successful == false) m_successful = true;
			return;
		}
		// otherwise continue as the OK button is clicked

        // create the rule based on the alarm's data...
        TA_Base_Core::IAlarmRuleData* suppressionRule = TA_Base_Core::AlarmRuleAccessFactory::getInstance().createSuppressionByDescriptionRule(
            entityTypeKey,
            entityKey,
            m_alarmDetails->messageTypeKey,
            "submit",
			alarmDescription,
            m_createdBySessionKey,
			alarmAssetName,
			alarmValue
            );
		TA_Base_Core::SuppressionRuleData* suppressionDescRule=dynamic_cast<TA_Base_Core::SuppressionRuleData*>(suppressionRule);
        suppressionDescRule->setOperatorName(m_createdByOperatorName);
		suppressionDescRule->setProfileKey(m_createdByProfileKey);

        // and save to the database...
        bool successful = false;
        std::string reason( "" );
        try
        {
            suppressionRule->applyChanges();
		    sendConfigUpdate(suppressionRule);
            successful = true;
        }
        catch( TA_Base_Core::DatabaseException& dbe )
        {
            reason = dbe.what();
			//showErrorMsg(reason, "description");
			showErrorMsg(reason, "描述");
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", reason.c_str() );
			return;
        }
        catch( TA_Base_Core::DataException& de )
        {
            reason = de.what();
			//showErrorMsg(reason, "description");
			showErrorMsg(reason, "描述");
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", reason.c_str() );
			return;
        }
        catch( TA_Base_Core::DataConfigurationException& dce )
        {
            reason = dce.what();
			//showErrorMsg(reason, "description");
			showErrorMsg(reason, "描述");
            LOG_EXCEPTION_CATCH( SourceInfo, "DataConfigurationException", reason.c_str() );
			return;
        }
        catch( ... )
        {
			//showErrorMsg("", "description");
			showErrorMsg("", "描述");
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Unknown exception occurred while saving suppression rule." );
			return;
        }

		// the new rule has been successfully applied/saved
		// send the config update
        //sendConfigUpdate(suppressionRule);
		
		FUNCTION_EXIT;
	}

	void CSuppressAlarmDialog::showErrorMsg(std::string reason, std::string type)
	{
        // if the rule couldn't be save
        // let the user know why :)
		CString errorMsg( "" );
		if( reason == "" )
		{
			// Log the error message
			//errorMsg += "An unknown error occurred preventing the inhibit-alarm-by-";
			errorMsg += "发生未知错误,导致";
			errorMsg += type.c_str();
			//errorMsg += " rule from being saved";
			errorMsg += "抑制不能保存";
		}
		else
		{
			// Log a message to user that there may be duplication
			//errorMsg += "An error occurred preventing the inhibit-alarm-by-";
			errorMsg += "发生错误,导致";
			errorMsg += type.c_str();
			//errorMsg += " rule from being saved.\nCause: ";
			errorMsg += "抑制不能保存. 原因: ";
			errorMsg += reason.c_str();
		}

		// No need to double log the error
		// LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error: %s", errorMsg.c_str() );
		TA_Base_Bus::TransActiveMessage userMsg;
		userMsg << errorMsg;
		CString errMsg = userMsg.constructMessage(IDS_UE_050130);
		AfxMessageBox(errMsg, MB_OK);
	}

    void CSuppressAlarmDialog::inhibitAlarm()
    {
        FUNCTION_ENTRY("inhibitAlarm");

        if(IsDlgButtonChecked(IDC_RADIO_ALARM_TYPE) == BST_CHECKED)
        {
			inhibitAlarmByType();
        }
		else if (IsDlgButtonChecked(IDC_RADIO_ALARM_DESCRIPTION) == BST_CHECKED)
		{
			inhibitAlarmByDescription();
		}
   
	    FUNCTION_EXIT;
    }

    void CSuppressAlarmDialog::sendConfigUpdate(TA_Base_Core::IAlarmRuleData* suppressionRule)
    {
	    FUNCTION_ENTRY("sendConfigUpdate");
 
        // First we iterate through and build up a list of changes. We are only interested in
        // which attributes have changed, not the values they have changed from and to.
	    std::stringstream entityKey;
        entityKey << suppressionRule->getKey();
	    std::vector<std::string> changes;
        changes.push_back(entityKey.str());

	    // Get the key
	    unsigned long keyOfChangedData = suppressionRule->getKey();

	    // Send the message
	    TA_Base_Core::ConfigUpdateMessageSender* configMessageSender = 0;
        configMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getConfigUpdateMessageSender( TA_Base_Core::ConfigUpdate::Context );
        TA_ASSERT(configMessageSender != NULL, "The config update message sender received is NULL");
	    configMessageSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigAlarmRule,
                                                       keyOfChangedData,
                                                       TA_Base_Core::Create,
                                                       changes);
	    delete configMessageSender;
	    configMessageSender = 0;
    
	    FUNCTION_EXIT;
    }

    void CSuppressAlarmDialog::setUpDialogTitle()
    {
        // Set the title of the dialog
        CString dialogTitle("");
		GetWindowText(dialogTitle);
		//CString replaceString("[AssetName].[AlarmType]");
		CString replaceString("[设备名称].[警告类型]");
		int assetIndex = dialogTitle.Find(replaceString, 0);
        CString alarmDetail = CString(m_alarmDetails->assetName.c_str()) + ".";

        // Set the alarm type in the dialog
        TA_Base_Core::IAlarmTypeData* iAlarmTypeData = NULL;
        std::string alarmType("");
        try
        {
            iAlarmTypeData = TA_Base_Core::AlarmTypeAccessFactory::getInstance().getAlarmTypeByKey(m_alarmDetails->messageTypeKey, false);
            alarmType = iAlarmTypeData->getName();
        }
        catch(const TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
        }
        catch(const TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "An unexpected exception was caught while retrieving alarm type.");
        }

        if(iAlarmTypeData != NULL)
        {
            delete iAlarmTypeData;
        }

		alarmDetail += alarmType.c_str();
		dialogTitle.Replace(replaceString, alarmDetail);
		//dialogTitle.Insert(assetIndex, alarmDetail);
        SetWindowText(dialogTitle);
    }
}
