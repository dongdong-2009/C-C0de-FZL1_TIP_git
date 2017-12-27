/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/DirectControlDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/11/18 14:31:43 $
  * Last modified by:  $Author: jinmin.huang $
  *
  */
// DirectControlDialog.cpp : implementation file
//

#include "app/scada/Inspector_Panel/src/stdafx.h"

#include "app/scada/inspector_panel/src/InspectorPanelDlg.h"
#include "app/scada/inspector_panel/src/Equipment.h"
#include "app/scada/inspector_panel/src/DirectControlDialog.h"
#include "app/scada/inspector_panel/src/AccessControlHelper.h"
#include "app/scada/inspector_panel/src/DataPointControlHelper.h"

#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_Base_App
{

    /////////////////////////////////////////////////////////////////////////////
    // CDirectControlDialog dialog


    CDirectControlDialog::CDirectControlDialog(CWnd* pParent /*=NULL*/)
	    : CDialog(CDirectControlDialog::IDD, pParent),
          m_dataPointKey(0),
          m_newValue(""),
          m_actionAllowed(false)
    {
	    //{{AFX_DATA_INIT(CDirectControlDialog)
	    //}}AFX_DATA_INIT
    }

    CDirectControlDialog::~CDirectControlDialog()
    {
        m_equipment = NULL;
    }


    void CDirectControlDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(CDirectControlDialog)
	DDX_Control(pDX, IDCONFIRM, m_confirmButton);
	    DDX_Control(pDX, IDC_NEW_VALUE, m_newValueLabel);
	    DDX_Control(pDX, IDC_EQUIPMENT, m_equipmentLabel);
	    DDX_Control(pDX, IDC_DATA_POINT, m_dataPointLabel);
	//}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(CDirectControlDialog, CDialog)
	    //{{AFX_MSG_MAP(CDirectControlDialog)
	    ON_BN_CLICKED(IDCONFIRM, OnConfirm)
        ON_MESSAGE(UPDATE_LABEL_IN_MAIN_THREAD_MESSAGE, OnUpdateLabelInMainThread)

	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CDirectControlDialog message handlers



    void CDirectControlDialog::resetDialog()
    {
        setEquipment(NULL);

        m_dataPointKey = 0;
        m_newValue = "";

        setDataPointLabelInMainThread("");
        setNewValueLabelInMainThread("");
    }

    void CDirectControlDialog::setDataPoint(unsigned long entityKey)
    {
		TA_ASSERT(m_equipment!=NULL, "Equipment not defined");
        if (entityKey != 0 && entityKey != m_dataPointKey)
        {
		    m_dataPointKey = entityKey;
        	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "set datapoint key to [%u]", entityKey);
			std::string description = "";
			try
			{
				// Setup datapoint proxy
				TA_Base_Bus::DataPointProxySmartPtr& dataPoint = 
								m_equipment->getChildDataPointFromEntityKey(m_dataPointKey);
				description = dataPoint->getDescription();
			}
			catch( ... )
			{
			}
			setDataPointLabelInMainThread(description);  
        	updatePermissions();
        }
    }

    void CDirectControlDialog::setNewValue(const std::string& newValue)
    {
        // Setup datapoint proxy
        setNewValueLabelInMainThread(newValue);  

        m_newValue = newValue;
		//Remove character &
		m_newValue.erase(std::remove_if(m_newValue.begin(),m_newValue.end(), 
				std::bind2nd(std::equal_to<char>(), '&')), 
			m_newValue.end());

        updatePermissions();
    }

	void CDirectControlDialog::setEquipment(CEquipment* equipment)
	{
		FUNCTION_ENTRY ("setEquipment()");

		if(equipment == NULL)
		{
			//this->resetDialog();
			return;
		}

		if(m_equipment != equipment)
		{
			m_equipment = equipment;
	        std::string equipmentLabel("");
	        try
			{
				equipmentLabel = m_equipment->getDataNode()->getAssetName();
				equipmentLabel += "\r\n";
				equipmentLabel += m_equipment->getDataNode()->getLabel();
			}
			catch(ScadaProxyException&)
			{
			}
			// Update the displayed control
			setEquipmentLabelInMainThread(equipmentLabel);

			std::string controlDataPointName = TA_Base_Core::RunParams::getInstance().get(EQUIPMENT_RPARAM);
			unsigned long controlEntityKey = equipment->getChildKeyByName(controlDataPointName);
			this->setDataPoint(controlEntityKey); 	
		}
	}


	void CDirectControlDialog::updatePermissions(bool skipAccessCheck)
	{
        // AccessControlHelper has been recently updated in InpectorPanelDlg

        if ( !m_equipment || 
             !m_dataPointKey || 
             m_newValue.size()==0)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"[16173] !m_equipment || !m_dataPointKey || m_newValue.size()==0");
			if (!m_equipment)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"[16173] !m_equipment");
			}
			if (!m_dataPointKey)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"[16173] !m_dataPointKey");
			}
			if (m_newValue.size()==0)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"[16173] m_newValue.size()==0");
			}
            setActionAllowed(false);
            return;
        }

        try
        {
            TA_Base_Bus::DataPointProxySmartPtr& dataPoint = 
                m_equipment->getChildDataPointFromEntityKey(m_dataPointKey);
        
            // In the same manner as ControlListCtrl, we update the datapoint write 
            // permission based on the current control inhibit state.

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"[16173] dataPoint->getEntityKey(): %lu ",
				dataPoint->getEntityKey());

            bool isDPWriteAllowed = AccessControlHelper::getInstance()->IsActionPermitted(
													    dataPoint->getEntityKey(), 
													    AccessControlHelper::DATA_POINT_WRITE);

			if (isDPWriteAllowed == false)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"[16173] isDPWriteAllowed == false");
			}

           // bool isControlInhibited = dataPoint->isControlInhibited();
		   // CL-20561, for change of proxy library, 2009-2-3
            bool isControlInhibited = false;			
			bool bSyn=false;
			long tryCount = 0;
			while(!bSyn && (tryCount++ < 40))
			{
				bSyn = true;
				try
				{
					isControlInhibited = dataPoint->isControlInhibited();
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					bSyn = false;
					Sleep(500);
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, 			
						TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());
				}
			}

			if (isControlInhibited == true)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"[16173] isControlInhibited == true");
			}

            setActionAllowed(isDPWriteAllowed && !isControlInhibited);
        }
        catch(...)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"[16173] Exception occurred");
            setActionAllowed(false);
        }
	}

    void CDirectControlDialog::setActionAllowed(bool isEnabled)
    {
        m_actionAllowed = isEnabled;
        m_confirmButton.EnableWindow(isEnabled);
        //::PostMessage(m_confirmButton, WM_ENABLE, static_cast<WPARAM>(isEnabled), 0);
    }

    void CDirectControlDialog::setEquipmentLabelInMainThread(const std::string& label)
    {
        std::string* temporalString = new std::string(label);
        PostMessage(
            UPDATE_LABEL_IN_MAIN_THREAD_MESSAGE,
            reinterpret_cast<UINT>(&m_equipmentLabel),
            reinterpret_cast<LONG>(temporalString));
    }
    void CDirectControlDialog::setDataPointLabelInMainThread(const std::string& label)
    {
        std::string* temporalString = new std::string(label);
        PostMessage(
            UPDATE_LABEL_IN_MAIN_THREAD_MESSAGE,
            reinterpret_cast<UINT>(&m_dataPointLabel),
            reinterpret_cast<LONG>(temporalString));
    }
    void CDirectControlDialog::setNewValueLabelInMainThread(const std::string& label)
    {
        std::string* temporalString = new std::string(label);
        PostMessage(
            UPDATE_LABEL_IN_MAIN_THREAD_MESSAGE,
            reinterpret_cast<UINT>(&m_newValueLabel),
            reinterpret_cast<LONG>(temporalString));
    }

    LRESULT CDirectControlDialog::OnUpdateLabelInMainThread(UINT wParam, LONG lParam)
    {
        std::string* temporalString = reinterpret_cast<std::string*>(lParam);
        CWnd* desiredLabel = reinterpret_cast<CWnd*>(wParam);

        if (temporalString)
        {
            if (desiredLabel)
            {
                desiredLabel->SetWindowText(temporalString->c_str());
            }
            delete temporalString;
        }
        return 0;
    }

    void CDirectControlDialog::OnConfirm() 
    {
        updatePermissions();

        // Assume this will work as we have just recently used the dataPointProxy to 
        // enable action permissions.  
        TA_Base_Bus::DataPointProxySmartPtr& dataPoint = 
            m_equipment->getChildDataPointFromEntityKey(m_dataPointKey);


        if ( !m_actionAllowed )
        {           
            // This should not happen unless a control inhibit has been recently imposed
            // since the last updatePermissions(), or the rights have changed.

			std::stringstream message;
			message << dataPoint->getDescription() << ": 没有权限.";

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message.str().c_str() );

			std::string* statusLineStr = new std::string(message.str());
			AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
			//CInspectorPanelDlg::addStatusLine ( message.str().c_str() );

            // Note that the Confirm Button has been disabled already
            return;
        }


		try
		{
			if ( dataPoint->isControlInhibited() )
			{
				std::stringstream message;
				//  [3/14/2010 benxun]
				//message << dataPoint->getDescription() << ": Control Inhibited.";
				message << dataPoint->getDescription() << ": 控制被禁止.";
				//  [3/14/2010 benxun]

				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message.str().c_str() );

				std::string* statusLineStr = new std::string(message.str());
				AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
				//CInspectorPanelDlg::addStatusLine ( message.str().c_str() );

				return;
			}


			if (!DataPointControlHelper::isControlValueValid(dataPoint, m_newValue))
			{
				// Error messages already displayed in helper
				return;
			}

			std::stringstream message;
			message << dataPoint->getDescription() << ": Updating value to \"" << m_newValue << "\".";

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message.str().c_str() );

			//CInspectorPanelDlg::addStatusLine ( message.str().c_str() );

			dataPoint->setValue(m_newValue, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));

			std::stringstream message2;
			message2 << dataPoint->getDescription() << ": 已将值设成 \"" << m_newValue << "\".";

			std::string* statusLineStr = new std::string(message2.str());
			AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message2.str().c_str() );

			//CInspectorPanelDlg::addStatusLine ( message2.str().c_str() );
		}
		catch (TA_Base_Core::ScadaProxyException &e)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what() );

			std::stringstream message;
			message << dataPoint->getDescription() << ": 将值设成 \"" << m_newValue << "\"失败，由于代理异常.";
			std::string* statusLineStr = new std::string(message.str());
			AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
		}
		catch (...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "%s", "Unknown Exception" );

			std::stringstream message;
			message << dataPoint->getDescription() << ": 将值设成 \"" << m_newValue << "\"失败，由于未知异常.";
			std::string* statusLineStr = new std::string(message.str());
			AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
		}
	    
    }

	//TD15945 Direct Control should post the status message when sending control (Not by CControllist)
	void CDirectControlDialog::updateControlState(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		if (TA_Base_Bus::ControlStateUpdate!=updateType || entityKey!=m_dataPointKey || m_equipment==NULL)
		{
			return;	
		}

		try
		{
			TA_Base_Bus::DataPointProxySmartPtr& dataPoint = 
					m_equipment->getChildDataPointFromEntityKey(m_dataPointKey);

			TA_Base_Bus::EDataPointControlState controlState = noControl;
			controlState = dataPoint->getControlState();

			addControlStateToStatusLine(dataPoint, controlState);
		}
		catch (TA_Base_Core::ScadaProxyException &e)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what() );
		}
		catch ( ... )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "%s", "Unknown Exception" );
		}
	}

	std::string CDirectControlDialog::addControlStateToStatusLine ( TA_Base_Bus::DataPointProxySmartPtr & datapoint, TA_Base_Bus::EDataPointControlState state ) //wenching++ (TD14179)
	{
		FUNCTION_ENTRY ("addControlStateToStatusLine()");

		bool addStatusLine = false;
		std::string returnString = "";
		std::stringstream statusLine;
		std::string value = ""; 

		try
		{
			// only proceed if the data point is an output data point
			if ( true == datapoint->isWriteable() )
			{
				statusLine << datapoint->getDescription() << ": " << datapoint->getValueAsString() << " - ";

				switch ( state ) 
				{
					case written:
							//statusLine << "Sent.";
							statusLine << "已发送.";
							addStatusLine = true;
							returnString = DPT_CONTROL_WRITTEN;
							break;
					
					case pending:
							//statusLine << "Pending.";
							statusLine << "处理中.";
							addStatusLine = true;
							returnString = DPT_CONTROL_PENDING;
							break;

					case conditionalSuccess:
							//statusLine << "Successful.";
							statusLine << "成功.";
							addStatusLine = true;
							returnString = DPT_CONTROL_CONDITIONALSUCCESS;						
							break;

					case success:
							//statusLine << "Successful.";
							statusLine << "成功.";
							addStatusLine = true;
							returnString = DPT_CONTROL_SUCCESS;
							break;

					case failed:
							//statusLine << "Failed.";
							statusLine << "失败.";
							addStatusLine = true;
							returnString = DPT_CONTROL_FAILED;						
							break;

					case LCC_failed:
							//CL18913++ - tester can not understand the datapoint related to the lcc of the control datapoint can not be accessed
							//statusLine << "Failed due to ";
							//statusLine << "LCC failed due to ";
							statusLine << "执行前检验失败，原因是";
							//++CL18913
							statusLine << datapoint->getLaunchingConditionResultString();
							statusLine << ".";
							addStatusLine = true;
							returnString = DPT_CONTROL_LCC_FAILED;
							break;

					case RCC_failed:
							//CL19432++ - The AssetName of the related equipment must be added in the status message
							//statusLine << "Failed due to Bad Return Condition.";
							//statusLine << "Sending control to " << datapoint->getEntityName();
							statusLine << "发送控制命令到 " << datapoint->getEntityName();
							//statusLine << " failed due to Bad Return Condition.";
							statusLine << " 失败，原因是未通过结果校验.";
							//++CL19432
							addStatusLine = true;
							returnString = DPT_CONTROL_RCC_FAILED;					
							break;

					case comms_failed:
							//statusLine << "Failed due to Comms Error.";
						statusLine << "失败.原因是通信错误";
							addStatusLine = true;
							returnString = DPT_CONTROL_COMMS_FAILED;
							break;

					case noControl:
					default:
							returnString = DPT_CONTROL_NOCONTROL;
					
				}

				if ( true == addStatusLine )
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", statusLine.str().c_str() );

					std::string* statusLineStr = new std::string(statusLine.str());
					AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), datapoint->getLastStatusChangeTimestamp());
				}		
			}
		}
		catch (TA_Base_Core::ScadaProxyException)
		{
		}
		catch ( ... )
		{
		}

		FUNCTION_EXIT;

		return returnString;
	}

}


