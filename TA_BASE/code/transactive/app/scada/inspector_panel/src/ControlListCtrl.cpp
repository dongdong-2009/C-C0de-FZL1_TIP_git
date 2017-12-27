/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/ControlListCtrl.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #8 $
  *
  * Last modification: $DateTime: 2013/12/09 09:59:42 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This class manages the list display used to show the basic control information in
  * the CControlDialog class.
  *
  */
 
#include "app/scada/inspector_panel/src/stdafx.h"
 
#include "bus/scada/common_library/src/DpValue.h"
#include "app/scada/inspector_panel/src/AccessControlHelper.h"
#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/InspectorPanelDlg.h"
#include "app/scada/inspector_panel/src/Equipment.h"
#include "app/scada/inspector_panel/src/ConfirmCommandDialog.h"
#include "app/scada/inspector_panel/src/InputNewValueDialog.h"
#include "app/scada/inspector_panel/src/ControlListCtrl.h"
#include "app/scada/inspector_panel/src/DataPointControlHelper.h"
#include "app/scada/inspector_panel/src/DataPointListCtrl.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

namespace TA_Base_App
{
    const COLORREF CButtonControlListCtrl::COLOUR_ALARM = 0x000000FF;  // colour to be used for showing alarms
    const COLORREF CButtonControlListCtrl::COLOUR_DISABLE = 0x00808080; // colour to be used for showing disabled data points
    const COLORREF CButtonControlListCtrl::COLOUR_NORMAL = 0x00000000; // colour to be used for showing normal data points

    /////////////////////////////////////////////////////////////////////////////
	// CDataPointListCtrl

	//TD15649, simplified this class 
	//by just responsing to the control state events of the visiable output datapoints
	//instead of using triggering entity of all datapoints to refresh the control state of visiable output datapoints
	
	enum columns
	{
		EQUIPMENT_ATTRIBUTES_COLUMN	= 0,
		DATAPOINT_TYPE_COLUMN		= 1,
		CONTROL_STATE_COLUMN		= 2,
		NEW_VALUE_COLUMN			= 3,
		//UNITS_COLUMN				= 4
	};

	CButtonControlListCtrl::CButtonControlListCtrl()
	:
	m_initialState(false), //wenching++ (TD14179)
	m_dataPointsDisplayedOnScreen ( false ),
	m_CurrentOperateDataPoint(-1)
	{
		FUNCTION_ENTRY ("CButtonControlListCtrl()");

		// We will manage the cell requests
		FUNCTION_EXIT;
	}

	CButtonControlListCtrl::~CButtonControlListCtrl()
	{
		FUNCTION_ENTRY ("~CButtonControlListCtrl()");
		std::map<unsigned long,CControlListCellButton*>::iterator itr;
		for(itr = m_buttonMap.begin();itr!=m_buttonMap.end(); ++itr)
		{
			itr->second->DestroyWindow();
			delete itr->second;
			itr->second = NULL;
		}
		FUNCTION_EXIT;
	}

	/* SetEquipment
	 *
	 * This function will set the name of the equipment that the DataPoints are to be displayed for.It can 
	 * be called before or after the windows control is on the screen.
	 */
	void CButtonControlListCtrl::setEquipment(CEquipment* equipment)
	{
		FUNCTION_ENTRY ("setEquipment()");

		m_equipment = equipment;

		// Update the displayed control
		if (m_hWnd!=0)
		{
			displayDataPoints();
		}

		FUNCTION_EXIT;
	}

	BEGIN_MESSAGE_MAP(CButtonControlListCtrl, CListCtrl)
		//{{AFX_MSG_MAP(CButtonControlListCtrl)
		ON_WM_CREATE()
		//}}AFX_MSG_MAP
    	ON_NOTIFY_REFLECT ( NM_CUSTOMDRAW, onCustomDrawList )
	END_MESSAGE_MAP()



	/////////////////////////////////////////////////////////////////////////////
	// CDataPointListCtrl message handlers

	int CButtonControlListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
	{
		FUNCTION_ENTRY ("OnCreate()");

		if (CWnd::OnCreate(lpCreateStruct) == -1)
		{
			FUNCTION_EXIT;
			return -1;
		}

		setupList();
		displayDataPoints();
		
		FUNCTION_EXIT;
		return 0;
	}


	void CButtonControlListCtrl::PreSubclassWindow() 
	{
		FUNCTION_ENTRY ("PreSubclassWindow()");

		CWnd::PreSubclassWindow();
		setupList();
		displayDataPoints();

		FUNCTION_EXIT;
	}

	//get Control data point's control value from its related input data point.
	bool CButtonControlListCtrl::getControlPointValueWithRelatedDataPointKey(unsigned long key,CString& stringValue)
	{
		std::string strValue = "";
		bool booleanValue = true;
		int	 integerValue = 0;
		bool bCanGetControlValue = false;

		//If this input data point is not a related data point for a control data point, just return false;
		int iPoint = m_equipment->getControlPointIndex(key);
		if (iPoint < 0)
		{
			stringValue = strValue.c_str();
			return false;
		}

		TA_Base_Bus::DataPointProxySmartPtr &inputdataPoint = m_equipment->getChildDataPointFromEntityKey(key);
		TA_Base_Bus::DataPointProxySmartPtr &controldataPoint = m_equipment->getChildDataPoint(iPoint);
		TA_Base_Bus::DpValue dpValue = inputdataPoint->getValue();

		//If the input data point is a derived data point, we only need to set the on and off status.
		if (inputdataPoint->isDerived() && controldataPoint->isBoolean())
		{
			unsigned long nValue = dpValue.getEnum();
			if (nValue == 2)
			{
				strValue = controldataPoint->getBooleanLabel(false);
			}
			else if (nValue == 1)
			{
				strValue = controldataPoint->getBooleanLabel(true);
			}
		}
		else if(inputdataPoint->isBoolean() && controldataPoint->isBoolean())
		{
			strValue = controldataPoint->getBooleanLabel(!dpValue.getBoolean());	
		}
		else if(inputdataPoint->isDerived() && controldataPoint->isDerived())
		{
			unsigned long nValue = dpValue.getEnum();
			if (nValue == 2)
			{
				strValue = controldataPoint->getDerivedStateAsString(1);
			}
			else if (nValue == 1)
			{
				strValue = controldataPoint->getDerivedStateAsString(2);
			}
		}
		else if(inputdataPoint->isBoolean() && controldataPoint->isDerived())
		{
			if(dpValue.getBoolean())
			{
				strValue = controldataPoint->getDerivedStateAsString(1);
			}
			else
			{
				strValue = controldataPoint->getDerivedStateAsString(2);
			}
		}



		//If the Input data point is a boolean data point, we need to get the opposite value
		/*else if (inputdataPoint->isBoolean())
		{
			bool bCurrentInputValue = dpValue.getBoolean();
			bControlValue = !bCurrentInputValue;

			bCanGetControlValue = true;
		}

		if (bCanGetControlValue)
		{
			TA_Base_Bus::DataPointProxySmartPtr &controldataPoint = m_equipment->getChildDataPoint(iPoint);
			if(controldataPoint->isBoolean())
			{
				strValue = controldataPoint->getBooleanLabel(bControlValue);
			}
			else if(controldataPoint->isDerived())
			{
				int stateValue = bControlValue == true?1:0;
				strValue = controldataPoint->getDerivedStateAsString();
			}
		}
		*/
		stringValue = strValue.c_str();
		return true;
	}


	bool CButtonControlListCtrl::getControlPointValueWithPosition(UINT iPoint,CString& strValue)
	{
		bool bSetValue = false;
		CString strControlValue = "";
		std::vector<TA_Base_Bus::DataPointProxySmartPtr*>& dataPoints = m_equipment->getChildDataPointVector();
		unsigned long nRelatedInputDataPointKey = (*dataPoints[iPoint])->getRelatedInputDataPoint();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getControlPointValueWithPosition -- Control(%d) Related(%d)",(*dataPoints[iPoint])->getEntityKey(),nRelatedInputDataPointKey);

		if (nRelatedInputDataPointKey != 0)
		{
			bSetValue = getControlPointValueWithRelatedDataPointKey(nRelatedInputDataPointKey,strControlValue);
		}
		else
		{
			bSetValue = true;
			strControlValue = ((*dataPoints[iPoint])->getBooleanLabel(true)).c_str();
		}
		strValue = strControlValue;
		return bSetValue;
	}

	void CButtonControlListCtrl::processButtonClick(int item, CString stringValue)
	{
		FUNCTION_ENTRY ("processButtonClick()");
		try
		{
			std::string  strValue = stringValue;
			if (strValue.empty())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Control value is empty, not processing!");
			}	

			UINT iPoint = GetItemData(item);
			//m_CurrentOperateDataPoint = iPoint;
			TA_ASSERT (m_equipment!=NULL,"CButtonControlListCtrl::processButtonClick() - Equipment not defined.");

			// Get the DataPoint array position
			TA_Base_Bus::DataPointProxySmartPtr &dataPoint = m_equipment->getChildDataPoint(iPoint);
			DataNodeProxySmartPtr& dataNode = m_equipment->getDataNode();

			if (dataPoint->getPScadaFlag())
			{
				if (!DataPointControlHelper::isControlValueValid(dataPoint, strValue))
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Control value is invalid!", strValue);
					FUNCTION_EXIT;
					return;
				}

				CControlListCellButton* button = this->getButton(item);
				TA_ASSERT (button!=NULL,"button clicked,but cannt find the button");
				
				try
				{
					std::stringstream message;
					message << dataPoint->getDescription() << ": Updating value to \"" << strValue << "\".";

					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message.str().c_str() );

					button->setIsWaitingForConfirm(true);
					dataPoint->setControlValue(strValue, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),CONTROL_SELECT);
					

					std::stringstream message1;
					message1 << dataPoint->getDescription() << ": Updated value to \"" << strValue << "\".";

					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message1.str().c_str() );
				}	
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					button->setIsWaitingForConfirm(false);
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what() );
				}
				catch (TA_Base_Core::NotAvailableException &e)
				{
					button->setIsWaitingForConfirm(false);
					std::string text = "setValue() - " + std::string(e.reason);
					LOG_EXCEPTION_CATCH(SourceInfo, "NotAvailableException", text);

					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "setValue failed!");
				}
				catch (TA_Base_Core::BadParameterException& e)
				{
					button->setIsWaitingForConfirm(false);
					std::string text = "setValue() - " + std::string(e.reason);
					LOG_EXCEPTION_CATCH(SourceInfo, "BadParameterException", text);

					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "setValue failed!");
				}
				catch (TA_Base_Core::AccessDeniedException& e)
				{
					button->setIsWaitingForConfirm(false);
					this->showStatusMessage("发送选择命令失败，没有权限");
					
					std::string text = "setValue() - " + std::string(e.reason);
					LOG_EXCEPTION_CATCH(SourceInfo, "AccessDeniedException", text);

					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "setValue failed!");
				}
				catch ( const TA_Base_Core::ObjectResolutionException & ex )
				{
					button->setIsWaitingForConfirm(false);
					LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "setValue failed!");
				}
				catch ( const TA_Base_Core::OperationModeException & )
				{
					button->setIsWaitingForConfirm(false);
					LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
				}
				catch ( const CORBA::Exception & ex )
				{
					button->setIsWaitingForConfirm(false);
					LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
				}
				catch (...)
				{
					button->setIsWaitingForConfirm(false);
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", "Unknown Exception" );
				}
											
			}
			else
			{
				CInputNewValueDialog inputDlg;
				inputDlg.m_equipment = dataNode->getAssetName().c_str();
				inputDlg.m_equipment += "\r\n";
				inputDlg.m_equipment += dataNode->getLabel().c_str();
				inputDlg.m_dataPoint = dataPoint->getDescription().c_str();				
				inputDlg.m_unit  = dataPoint->getUnit().c_str();

				if (dataPoint->isNumber())
				{
					if (dataPoint->isDerived())
					{
						inputDlg.setValueList(dataPoint->getDerivedStatesAsMap());
						if (inputDlg.DoModal()==IDOK)
						{
							strValue = inputDlg.m_dropListValue;
						}
						else
						{
							return;
						}
					}
					else 
					{
						if (inputDlg.DoModal()==IDOK)
						{
							strValue = inputDlg.m_newValue;
						}
						else
						{
							return;
						}
					}
				}
				else if (dataPoint->isText())
				{
					inputDlg.m_isTextValue = true;
					if (inputDlg.DoModal()==IDOK)
					{
						strValue = inputDlg.m_newValue;
					}
					else
					{
						return;
					}
				}	

				CConfirmCommandDialog     dlg;

				// Confirm update
				dlg.m_equipment = dataNode->getAssetName().c_str();
				dlg.m_equipment += "\r\n";
				dlg.m_equipment += dataNode->getLabel().c_str();

				dlg.m_dataPoint = dataPoint->getDescription().c_str();
				dlg.m_newValue  = strValue.c_str();


				if (!DataPointControlHelper::isControlValueValid(dataPoint, strValue))
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Control value is invalid!", strValue);
					FUNCTION_EXIT;
					return;
				}

				if (dlg.DoModal()==IDOK)
				{
					try
					{

						std::stringstream message;
						message << dataPoint->getDescription() << ": Updating value to \"" << strValue << "\".";

						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message.str().c_str() );

						dataPoint->setValue(strValue, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));

						std::stringstream message1;
						message1 << dataPoint->getDescription() << ": Updated value to \"" << strValue << "\".";

						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message1.str().c_str() );

					}
					catch (TA_Base_Core::ScadaProxyException &e)
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what() );
					}
					catch (TA_Base_Core::NotAvailableException &e)
					{
						std::string text = "setValue() - " + std::string(e.reason);
						LOG_EXCEPTION_CATCH(SourceInfo, "NotAvailableException", text);

						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "setValue failed!");
					}
					catch (TA_Base_Core::BadParameterException& e)
					{
						std::string text = "setValue() - " + std::string(e.reason);
						LOG_EXCEPTION_CATCH(SourceInfo, "BadParameterException", text);

						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "setValue failed!");
					}
					catch (TA_Base_Core::AccessDeniedException& e)
					{
						this->showStatusMessage("设值失败，没有权限");
						
						std::string text = "setValue() - " + std::string(e.reason);
						LOG_EXCEPTION_CATCH(SourceInfo, "AccessDeniedException", text);

						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "setValue failed!");
					}
					catch ( const TA_Base_Core::ObjectResolutionException & ex )
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "setValue failed!");
					}
					catch ( const TA_Base_Core::OperationModeException & )
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
					}
					catch ( const CORBA::Exception & ex )
					{
						LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
					}
					catch (...)
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", "Unknown Exception" );
					}
				}
			}
		}
		catch (...)
		{
			AfxMessageBox( _T("updatecell exeption"));
		}
	}
	// Helper functions

	void CButtonControlListCtrl::setupList()
	{
		FUNCTION_ENTRY ("setupList()");

		SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | 
			 LVS_EX_SUBITEMIMAGES  );

		this->InsertColumn(EQUIPMENT_ATTRIBUTES_COLUMN,		"描述");
		this->InsertColumn(DATAPOINT_TYPE_COLUMN,			"类型",LVCFMT_CENTER);
		this->InsertColumn(CONTROL_STATE_COLUMN,			"状态");
		this->InsertColumn(NEW_VALUE_COLUMN,				"操作");


		this->SetColumnWidth(EQUIPMENT_ATTRIBUTES_COLUMN,	180);
		this->SetColumnWidth(DATAPOINT_TYPE_COLUMN,			45);
		this->SetColumnWidth(CONTROL_STATE_COLUMN,			98);
		this->SetColumnWidth(NEW_VALUE_COLUMN,				125);

		FUNCTION_EXIT;
	}

	void CButtonControlListCtrl::UpdateControlState()
	{
		std::vector<TA_Base_Bus::DataPointProxySmartPtr*>& dataPoints = m_equipment->getChildDataPointVector();
		std::vector<unsigned long>& vectorIndex = m_equipment->getWriteableIndex(); 

		// Only proceed if the control point all config valid
 		if ( dataPoints.size() > 0 && vectorIndex.size() > 0 )
		{
			// Create a triggering entity to entity map
			// Find the latest control status for the equipment as well as each control point
			int   maxIndex = -1;
			time_t timeStamp = 0L;
			CTime maxTimestamp(timeStamp);
			CTime curTimestamp(timeStamp);
			for(int i=0; i<vectorIndex.size(); i++)
			{
				int iPoint = vectorIndex.at(i);
				int latest = -1;
				
                try
                {
                    timeStamp = (time_t) (*dataPoints[iPoint])->getLastStatusChangeTimestamp();
                    curTimestamp = CTime(timeStamp);
                    
                    if( maxTimestamp < curTimestamp)
                    {
                        maxTimestamp = curTimestamp;
                        maxIndex = iPoint;
                    }
                }
                catch (TA_Base_Core::ScadaProxyException &e)
                {
                    //timestamp is not ready
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());
                }
			    catch ( const TA_Base_Core::ObjectResolutionException & ex )
			    {
				    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
			    }
			    catch ( const TA_Base_Core::OperationModeException & )
			    {
				    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
			    }
                catch ( const CORBA::Exception & ex )
			    {
				    LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
			    }
			    catch (...)
			    {
				    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", "Unknown Exception" );
			    }
            }
			
			try
			{
				// Show the latest control message in the status window
				if( maxIndex!=-1 && (*dataPoints[maxIndex])->isWriteable() )
				{
					addControlStateToStatusLine ( (*dataPoints[maxIndex]), (*dataPoints[maxIndex])->getControlState() );
				}
			}
			catch (TA_Base_Core::ScadaProxyException &e)
			{
				// Not always available
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());
			}
			catch ( const TA_Base_Core::ObjectResolutionException & ex )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
			}
			catch ( const TA_Base_Core::OperationModeException & )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
			}
            catch ( const CORBA::Exception & ex )
			{
				LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
			}
			catch (...)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", "Unknown Exception" );
			}
		}
	}

	// By using the preprocessed index vector, reduce linear search and ensure only those
	// datapoints had been sorted and config valid were displayed.
	void CButtonControlListCtrl::displayDataPoints()
	{
		FUNCTION_ENTRY ("displayDataPoints()");

		if (m_equipment==NULL)
		{
			FUNCTION_EXIT;
			return;
		}

		//m_setValueActionAllowed.clear(); 
		std::map<unsigned int, bool>::iterator it;

		int validItem = 0;
		int difference = 0;
		int totalItem = GetItemCount();

		int				nItem;
		std::string		value;
	
		std::vector<TA_Base_Bus::DataPointProxySmartPtr*>& dataPoints = m_equipment->getChildDataPointVector();
	
		// Get the preprocessed indexes with the required config.
		std::vector<unsigned long>& vectorIndex = m_equipment->getWriteableIndex(); 

		if (0 == vectorIndex.size())
		{
			DeleteAllItems();
			FUNCTION_EXIT;
			return;
		}

		if (m_initialState == false)
		{
			m_initialState = true;
			UpdateControlState();
		}

		// Only display the datapoints with index in the vector
		for (unsigned int i=0;i<vectorIndex.size();i++)
		{
			unsigned int iPoint = vectorIndex.at(i);

			try
			{
				RefreshPermission(iPoint);

				validItem++;

				// Description (Equipment Attributes)
				if( validItem > GetItemCount() )
				{
					nItem = this->InsertItem(validItem-1,(*dataPoints[iPoint])->getDescription().c_str());
				}
				else
				{
					nItem = validItem-1;
				}
				SetItemData(nItem,iPoint);
				SetItemText(nItem, EQUIPMENT_ATTRIBUTES_COLUMN, (*dataPoints[iPoint])->getDescription().c_str());

				// Type of the DataPoint
				value = (*dataPoints[iPoint])->getShortNameOfType();
				SetItemText(nItem, DATAPOINT_TYPE_COLUMN, value.c_str());

		
				//Need Display Select Button for IEC 104 Protocol Control
				CRect cellRect;
				GetSubItemRect(nItem, NEW_VALUE_COLUMN, LVIR_LABEL, cellRect);
				CString strControlValue;

				if ((*dataPoints[iPoint])->getPScadaFlag())
				{
					bool bSetValue = getControlPointValueWithPosition(iPoint,strControlValue);


					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Before create button Row %d ,buttonMap Size(%d)",nItem,m_buttonMap.size());
				
					CControlListCellButton *button = getButton(nItem);
					if (button == NULL)
					{
						button = new CControlListCellButton(this, cellRect, nItem, strControlValue + "选择", this);
						button->setValue(strControlValue);
						button->EnableWindow(false);
						m_buttonMap.insert(std::map<unsigned long,CControlListCellButton*> ::value_type(nItem, button));

					}
				}
				else
				{
					if ((*dataPoints[iPoint])->isBoolean())
					{
						std::string label = "";

						typedef std::vector<std::string> ValueData ;
						ValueData data;

						try
						{
							label = (*dataPoints[iPoint])->getBooleanLabel(true);

							if (label.size() != 0)
								data.push_back(label);

							label = (*dataPoints[iPoint])->getBooleanLabel(false);

							if (label.size() != 0)
								data.push_back(label);

						}
						catch (TA_Base_Core::ScadaProxyException &e)
						{
							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());
						}

						if (data.size() == 1)
						{
							CControlListCellButton *button = getButton(nItem*2);
							if (button == NULL)
							{
								strControlValue = data[0].c_str();

								button = new CControlListCellButton(this, cellRect, nItem, strControlValue , this);
								button->setValue(strControlValue);
								button->EnableWindow(false);
								m_buttonMap.insert(std::map<unsigned long,CControlListCellButton*> ::value_type(2*nItem, button));
							}

						}
						else if (data.size() == 2)
						{
							CRect buttonRectLeft(cellRect.left, cellRect.top, cellRect.right - cellRect.Width()/2, cellRect.bottom);
							CRect buttonRectRight(cellRect.left + cellRect.Width()/2, cellRect.top, cellRect.right , cellRect.bottom);

							CControlListCellButton *button = getButton(nItem*2);
							if (button == NULL)
							{
								strControlValue = data[0].c_str();

								button = new CControlListCellButton(this, buttonRectLeft, nItem, strControlValue , this);
								button->setValue(strControlValue);
								button->EnableWindow(false);
								m_buttonMap.insert(std::map<unsigned long,CControlListCellButton*> ::value_type(2*nItem, button));
							}

							button = getButton(nItem*2 + 1);
							if (button == NULL)
							{
								strControlValue = data[1].c_str();

								button = new CControlListCellButton(this, buttonRectRight, nItem, strControlValue , this);
								button->setValue(strControlValue);
								button->EnableWindow(false);
								m_buttonMap.insert(std::map<unsigned long,CControlListCellButton*> ::value_type(2*nItem + 1, button));
							}
						}
					}
			
					if ((*dataPoints[iPoint])->isNumber() || (*dataPoints[iPoint])->isText())
					{
						CControlListCellButton *button = getButton(nItem*2);
						if (button == NULL)
						{

							button = new CControlListCellButton(this, cellRect, nItem, "设置" , this);
							button->EnableWindow(false);
							m_buttonMap.insert(std::map<unsigned long,CControlListCellButton*> ::value_type(2*nItem, button));
						}
					}
				}
				RefreshButtonDisplay(iPoint,nItem);
				// Control State
				value = getControlStatusString((*dataPoints[iPoint])->getControlState());
				SetItemText(nItem, CONTROL_STATE_COLUMN, value.c_str());

				// set the flag to indicate data points are displayed on screen
				if ( false == m_dataPointsDisplayedOnScreen )
				{
					m_dataPointsDisplayedOnScreen = true;
				}
			}
			catch (TA_Base_Core::ScadaProxyException &e)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());
			}
			catch ( const TA_Base_Core::ObjectResolutionException & ex )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
			}
			catch ( const TA_Base_Core::OperationModeException & )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
			}
            catch ( const CORBA::Exception & ex )
			{
				LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
			}
			catch (...)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", "Unknown Exception" );
			}
		}

		if( validItem > 0 )
		{
			difference = totalItem - validItem;

			if ( difference > 0 )
			{
				for ( int i=0; i<difference; i++)
					DeleteItem(totalItem-1-i);
			}
		}
		else if( validItem == 0 && totalItem != 0 )
		{
			for ( int i=0; i<totalItem; i++)
				DeleteItem(i);
		}

		FUNCTION_EXIT;
	}	



	void CButtonControlListCtrl::updateDataPoint(unsigned long key, ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataPoint()");

		if (this->m_hWnd==0 || m_equipment==NULL)
		{
			FUNCTION_EXIT;
			return;
		}
		
		switch (updateType)
		{
		case ValueStateUpdate:
		case QualitySummaryUpdate:
			{
				UINT iPoint = m_equipment->getChildDataPointIndexFromEntityKey(key);
				TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(iPoint);

				if (dataPoint->getPScadaFlag())
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Receive Data Point %d Value change",key);

					CString strValue = "";
					bool bSetValue = getControlPointValueWithRelatedDataPointKey(key,strValue);

					if (bSetValue)
					{
						UINT iPoint = m_equipment->getControlPointIndex(key);

						for (int nItem=0; nItem < GetItemCount(); nItem++)
						{
							if (this->GetItemData(nItem) == iPoint)
							{
								std::vector<TA_Base_Bus::DataPointProxySmartPtr*>& dataPoints = m_equipment->getChildDataPointVector();
								bool bIsActionPermitted = AccessControlHelper::getInstance()->IsActionPermitted(
									(*dataPoints[iPoint])->getEntityKey(), 
									AccessControlHelper::DATA_POINT_WRITE);

								setButtonText( nItem, strValue, strValue + "选择" );

								RefreshButtonDisplay(iPoint,nItem);

								return;
							}
						}
					}
				}
			}
			break;
        case AlarmSummaryUpdate://limin++, TD20740
        case AckSummaryUpdate:  //limin++, TD20740
		case InhibitStateUpdate:
			{
				//  Find the DataPoint item with the key
				UINT iPoint = m_equipment->getChildDataPointIndexFromEntityKey(key);

				if (iPoint<m_equipment->getChildDataPointCount())
				{
					if (updateType == InhibitStateUpdate)
					{
						RefreshPermission(iPoint);
					}
					
					//  Find the DataPoint in the list view
					for (int nItem=0; nItem < GetItemCount(); nItem++)
					{
						if (this->GetItemData(nItem)==iPoint)
						{
							updateDataPointDisplay ( iPoint, nItem, updateType );

							FUNCTION_EXIT;
							return;
						}
					}
				}
			}
			break;

		case ControlStateUpdate:
			{
				UpdateControlState();

				//  Find the DataPoint item with the key
				UINT iPoint = m_equipment->getChildDataPointIndexFromEntityKey(key);

				//  Find the DataPoint in the list view
				for (int nItem=0; nItem < GetItemCount(); nItem++)
				{
					if (this->GetItemData(nItem)==iPoint)
					{
						updateDataPointDisplay ( iPoint, nItem, updateType );

						FUNCTION_EXIT;
						return;
					}
				}
			}
			break;

		default:
			break;
		}


		FUNCTION_EXIT;
	}

	void CButtonControlListCtrl::updateDataNode(ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataNode()");

		// No datanode details here

		FUNCTION_EXIT;
	}

	void CButtonControlListCtrl::updateDataPointDisplay ( int iPoint, int nItem, TA_Base_Bus::ScadaEntityUpdateType updateType )
	{
		FUNCTION_ENTRY ("updateDataPointDisplay()");

		if (m_equipment == NULL)
		{
			FUNCTION_EXIT;
			return;
		}

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Receive Item %d updateType %d",nItem,updateType );

		try
		{
			std::string			value;
			TA_Base_Bus::DataPointProxySmartPtr&	dataPoint = m_equipment->getChildDataPoint(iPoint);
			
			// Update the point
			if ( dataPoint->isWriteable() && !dataPoint->isControlInhibited() )
			{
				// Description
				value = dataPoint->getDescription();
				SetItemText(nItem,EQUIPMENT_ATTRIBUTES_COLUMN,value.c_str());

				// Type of the DataPoint
				value = dataPoint->getShortNameOfType();
				SetItemText(nItem, DATAPOINT_TYPE_COLUMN, value.c_str());

				// Control state
				value = "";
				value = getControlStatusString(dataPoint->getControlState());
				SetItemText(nItem, CONTROL_STATE_COLUMN, value.c_str());

				if (dataPoint->getPScadaFlag())
				{
					//displayDataPoints();	
					
					bool shouldCancelDlg = false;
					//Using for IEC104 Protocol control
					TA_Base_Bus::EDataPointControlState controlState = dataPoint->getControlState();
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Item %d current control state = %d",nItem,controlState );

					{
						switch(controlState)
						{
						case selectCmdWritten:
						case selectCmdConfirm:
							//case selectCmdConfirmTimeout:
						case execCmdWritten:
						//case execCmdConfirm:
							//case execCmdConfirmTimeout:
						case cancelCmdWritten:
							{
								std::map<unsigned long,CControlListCellButton*>::iterator iter  = m_buttonMap.find(nItem);
								if (iter != m_buttonMap.end())
								{			
									CControlListCellButton* pButton = iter->second;
									pButton->EnterProcess();
									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Enter In Process %d",controlState );

								} 
								RefreshButtonDisplay(iPoint,nItem);

							}
							break;

						//case execCmdSuccess:
						case operateCmdTimeout:
						case conditionalSuccess:
						case success:
						case failed:
						case selectCmdConfirmTimeout:
						case execCmdConfirmTimeout:
						case execCmdSuccessTimeout:
						case cancelCmdConfirmTimeout:
						case cancelCmdConfirm:
						case LCC_failed:
						case RCC_failed:
						case selectCmdCfmNegative:
						case execCmdCfmNegative:
						case cancelCmdCfmNegative:
						case deviceControlException:
							{
								shouldCancelDlg = true;
								std::map<unsigned long,CControlListCellButton*>::iterator iter  = m_buttonMap.find(nItem);
								if (iter != m_buttonMap.end())
								{			
									CControlListCellButton* pButton = iter->second;
									pButton->ExitProcess();
									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Exit In Process %d",controlState );
								} 
								RefreshButtonDisplay(iPoint,nItem);
							}
							break;
						default:
							//EnableButton(nItem,false);
							RefreshButtonDisplay(iPoint,nItem);
							break;
						}

						//If operator don't send the execute or cancel command to PSCADA Controller in a 
						//defined time,after this duration, PScadaAgent will check this timeout,and send a 
						//operateCmdTimeout control state change, we need close confirmcommandDialog. 
						//if (controlState == operateCmdTimeout)
						if (shouldCancelDlg)
						{
							HWND hwnd = ::FindWindow(NULL,_T("请执行或取消操作"));
							//::SendMessage(hwnd,WM_COMMAND,IDCANCEL,NULL);
							CControlListCellButton *pButton = this->getButton(nItem);
							if(NULL != pButton)
							{
								CWnd* hwnd = pButton->getConfirmDialogHandle();
								if(NULL != hwnd)
								{
									hwnd->SendMessage(WM_COMMAND,IDCANCEL,NULL);
									//::SendMessage(hwnd,WM_COMMAND,IDCANCEL,NULL);
									pButton->setConfirmDialogHandle(NULL);
								}
							}
						}

						if (controlState == selectCmdConfirm )
						{
							CControlListCellButton* pButton = this->getButton(nItem);
							TA_ASSERT(pButton!=NULL,"receive selectCmdConfirm, but cannt find a button");
							if(!pButton->getIsWaitingForConfirm())
							{
								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Ignored select command confirm message,because message not caused by current operator");
								return;
							}
							pButton->setIsWaitingForConfirm(false);
							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Receive select command confirm message" );
							DataNodeProxySmartPtr& dataNode = m_equipment->getDataNode();
							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "before new a CConfirmCommandDialog" );
							CConfirmCommandDialog confirmcommandDialog;
							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "after new a CConfirmCommandDialog" );
							confirmcommandDialog.m_equipment = dataNode->getAssetName().c_str();
							confirmcommandDialog.m_equipment += "\r\n";
							confirmcommandDialog.m_equipment += dataNode->getLabel().c_str();
							confirmcommandDialog.m_dataPoint = dataPoint->getDescription().c_str();
							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "confirmcommandDialog.m_equipment = %s",confirmcommandDialog.m_equipment );

							if (pButton->getIfExpandDialog())
							{
								return;
							}
							confirmcommandDialog.m_newValue  = pButton->getValue();
							pButton->expandDialog();
							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "confirmcommandDialog.m_newValue = %s",confirmcommandDialog.m_newValue);

							std::string strValue = confirmcommandDialog.m_newValue;
							confirmcommandDialog.m_changeWindowInfo = true;
							CWnd* pwnd = &confirmcommandDialog;
							pButton->setConfirmDialogHandle(pwnd);
							if (confirmcommandDialog.DoModal()==IDOK)
							{
								pButton->setConfirmDialogHandle(NULL);
								try
								{
									std::stringstream message;
									message << dataPoint->getDescription() << ": Updating value to \"" << strValue << "\".";

									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message.str().c_str() );

									dataPoint->setControlValue(strValue, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),CONTROL_EXECUTE);

									std::stringstream message1;
									message1 << dataPoint->getDescription() << ": Updated value to \"" << strValue << "\".";

									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message1.str().c_str() );

								}
								catch (TA_Base_Core::ScadaProxyException &e)
								{
									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what() );
								}
								catch (TA_Base_Core::NotAvailableException &e)
								{
									std::string text = "setValue() - " + std::string(e.reason);
									LOG_EXCEPTION_CATCH(SourceInfo, "NotAvailableException", text);

									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "setValue failed!");
								}
								catch (TA_Base_Core::BadParameterException& e)
								{
									std::string text = "setValue() - " + std::string(e.reason);
									LOG_EXCEPTION_CATCH(SourceInfo, "BadParameterException", text);

									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "setValue failed!");
								}
								catch (TA_Base_Core::AccessDeniedException& e)
								{
									this->showStatusMessage("发送执行命令失败，没有权限");
									
									std::string text = "setValue() - " + std::string(e.reason);
									LOG_EXCEPTION_CATCH(SourceInfo, "AccessDeniedException", text);

									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "setValue failed!");
								}
								catch ( const TA_Base_Core::ObjectResolutionException & ex )
								{
									LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "setValue failed!");
								}
								catch ( const TA_Base_Core::OperationModeException & )
								{
									LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
								}
								catch ( const CORBA::Exception & ex )
								{
									LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
								}
								catch (...)
								{
									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", "Unknown Exception" );
								}
							}
							else
							{
								pButton->setConfirmDialogHandle(NULL);
								try
								{
									std::stringstream message;
									message << dataPoint->getDescription() << ": Updating value to \"" << strValue << "\".";

									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message.str().c_str() );

									dataPoint->setControlValue(strValue, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),CONTROL_CANCEL);

									std::stringstream message1;
									message1 << dataPoint->getDescription() << ": Updated value to \"" << strValue << "\".";

									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message1.str().c_str() );

								}
								catch (TA_Base_Core::ScadaProxyException &e)
								{
									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what() );
								}
								catch (TA_Base_Core::NotAvailableException &e)
								{
									std::string text = "setValue() - " + std::string(e.reason);
									LOG_EXCEPTION_CATCH(SourceInfo, "NotAvailableException", text);

									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "setValue failed!");
								}
								catch (TA_Base_Core::BadParameterException& e)
								{
									std::string text = "setValue() - " + std::string(e.reason);
									LOG_EXCEPTION_CATCH(SourceInfo, "BadParameterException", text);

									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "setValue failed!");
								}
								catch (TA_Base_Core::AccessDeniedException& e)
								{
									this->showStatusMessage("发送撤销命令失败，没有权限");
									
									std::string text = "setValue() - " + std::string(e.reason);
									LOG_EXCEPTION_CATCH(SourceInfo, "AccessDeniedException", text);

									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "setValue failed!");
								}
								catch ( const TA_Base_Core::ObjectResolutionException & ex )
								{
									LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "setValue failed!");
								}
								catch ( const TA_Base_Core::OperationModeException & )
								{
									LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
								}
								catch ( const CORBA::Exception & ex )
								{
									LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
								}
								catch (...)
								{
									LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", "Unknown Exception" );
								}
							}

							pButton->unExpandDialog();
						}

					}


					// Units
					try
					{
						value = dataPoint->getUnit();
					}
					catch (TA_Base_Core::ScadaProxyException &e)
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());

						value = "";
					}
					//SetItemText(nItem,UNITS_COLUMN,value.c_str());				
				}
				else
				{
					RefreshButtonDisplay(iPoint,nItem);
				}
			}
			//if this datapoint is inhabit status, the button should be disabled.
			else if (dataPoint->isWriteable() && dataPoint->isControlInhibited())
			{
				RefreshButtonDisplay(iPoint,nItem);
				//EnableButton(nItem,false);
			}
		}
		catch (TA_Base_Core::ScadaProxyException &e)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());
		}
		catch ( const TA_Base_Core::ObjectResolutionException & ex )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
		}
		catch ( const TA_Base_Core::OperationModeException & )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
		}
        catch ( const CORBA::Exception & ex )
		{
			LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
		}
		catch (...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", "Unknown Exception" );
		}

		FUNCTION_EXIT;
	}

	std::string CButtonControlListCtrl::getControlStatusString ( TA_Base_Bus::EDataPointControlState state ) 
	{
		std::string returnString ( "" );

		try
		{	
			switch ( state )
			{
			case written:
				returnString = DPT_CONTROL_WRITTEN;
				break;

			case pending:
				returnString = DPT_CONTROL_PENDING;
				break;

			case conditionalSuccess:
				returnString = DPT_CONTROL_CONDITIONALSUCCESS;
				break;

			case success:
				returnString = DPT_CONTROL_SUCCESS;
				break;

			case failed:
				returnString = DPT_CONTROL_FAILED;
				break;

			case LCC_failed:
				returnString = DPT_CONTROL_LCC_FAILED;
				break;

			case RCC_failed:
				returnString = DPT_CONTROL_RCC_FAILED;
				break;

			case comms_failed:
				returnString = DPT_CONTROL_COMMS_FAILED;
				break;

			//Using for IEC104 Protocol control
			case selectCmdWritten:
				returnString = DPT_CONTROL_SELECT_SENT;
				break;

			case selectCmdConfirm:
				returnString = DPT_CONTROL_SELECT_CONFIRM;
				break;

			case selectCmdConfirmTimeout:
				returnString = DPT_CONTROL_SELECT_TIMEOUT;
				break;

			case operateCmdTimeout:
				returnString = DPT_CONTROL_OPERATOR_TIMEOUT;
				break;
				
			case execCmdWritten:
				returnString = DPT_CONTROL_EXECUTE_SENT;
				break;

			case execCmdConfirm:
				returnString = DPT_CONTROL_EXECUTE_CONFIRM;
				break;

			case execCmdConfirmTimeout:
				returnString = DPT_CONTROL_EXECUTE_TIMEOUT;
				break;

			case execCmdSuccess:
				returnString = DPT_CONTROL_EXECUTE_SUCCESS;
				break;

			case execCmdSuccessTimeout:
				returnString = DPT_CONTROL_EXECUTE_SUC_TIMEOUT;
				break;

			case cancelCmdWritten:
				returnString = DPT_CONTROL_CANCEL_SENT;
				break;

			case cancelCmdConfirm:
				returnString = DPT_CONTROL_CANCEL_CONFIRM;
				break;

			case cancelCmdConfirmTimeout:
				returnString = DPT_CONTROL_CANCEL_TIMEOUT;
				break;

			case noControl:
			default:
				returnString = DPT_CONTROL_NOCONTROL;
				break;
			}
		}
		catch (TA_Base_Core::ScadaProxyException& e)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());
		}
		catch ( ... )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"Unknown exception caught while convert the control state to string");
		}

		return returnString;
	}

	std::string CButtonControlListCtrl::addControlStateToStatusLine ( TA_Base_Bus::DataPointProxySmartPtr & datapoint, TA_Base_Bus::EDataPointControlState state ) //wenching++ (TD14179)
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
				//TD15649.
				statusLine << datapoint->getDescription() << ": " << datapoint->getValueAsString() << " - ";

				switch ( state ) 
				{
					case written:
						{
							//TD15649
							//statusLine << "Sent.";
							statusLine << "已发送.";
							addStatusLine = true;
							returnString = DPT_CONTROL_WRITTEN;
							break;
						}

					case pending:
						{
							//statusLine << "Pending.";
							statusLine << "处理中.";
							addStatusLine = true;
							returnString = DPT_CONTROL_PENDING;
							break;
						}

					case conditionalSuccess:
						{
							//statusLine << "Successful.";
							statusLine << "完成.";
							addStatusLine = true;
							returnString = DPT_CONTROL_CONDITIONALSUCCESS;						
							break;
						}

					case success:
						{
							//statusLine << "Successful.";
							statusLine << "成功.";
							addStatusLine = true;
							returnString = DPT_CONTROL_SUCCESS;
							break;
						}

					case failed:
						{
							//statusLine << "Failed.";
							statusLine << "失败.";
							addStatusLine = true;
							returnString = DPT_CONTROL_FAILED;						
							break;
						}

					case LCC_failed:
						{
							//statusLine << "Failed due to ";
							statusLine << "失败,原因是 ";
							statusLine << datapoint->getLaunchingConditionResultString();
							statusLine << ".";
							addStatusLine = true;
							returnString = DPT_CONTROL_LCC_FAILED;
							break;
						}

					case RCC_failed:
						{
							//CL19432++ - The AssetName of the related equipment must be added in the status message
							//statusLine << "Failed due to Bad Return Condition.";
							//statusLine << "Sending control to " << datapoint->getEntityName();
							//statusLine << " failed due to Bad Return Condition.";
							statusLine << "发送控制到 " << datapoint->getDescription();
							statusLine << " 失败.原因是未通过结果校验";
							//++CL19432
							addStatusLine = true;
							returnString = DPT_CONTROL_RCC_FAILED;				
							break;
						}

					case comms_failed:
						{
							//statusLine << "Failed due to Comms Error.";
							statusLine << "失败.原因是通信错误";
							addStatusLine = true;
							returnString = DPT_CONTROL_COMMS_FAILED;
							break;
						}

					//Using for IEC104 Protocol Control
					case selectCmdWritten:
						{
							//statusLine << "Sent Select Command to " << datapoint->getEntityName() << ".";
							statusLine << "发送选择命令到" << datapoint->getDescription() << ".";
							addStatusLine = true;
							returnString = DPT_CONTROL_SELECT_SENT;
							break;
						}

					case selectCmdConfirm:
						{
							//statusLine << "Recv Select Confirm from " << datapoint->getEntityName() << ".";
							statusLine << "从 " << datapoint->getDescription() << "接收选择确认.";
							addStatusLine = true;
							returnString = DPT_CONTROL_SELECT_CONFIRM;
							break;
						}

					case selectCmdConfirmTimeout:
						{
							//statusLine << "Select Confirm from " << datapoint->getEntityName() << " failed due to timeout.";
							statusLine << "从" << datapoint->getDescription() << " 接收选择确认失败，原因是超时.";
							addStatusLine = true;
							returnString = DPT_CONTROL_SELECT_CONFIRM;
							break;
						}

					case operateCmdTimeout:
						{
							//statusLine << "Operator timeout.";
							statusLine << "操作员超时.";
							addStatusLine = true;
							returnString = DPT_CONTROL_OPERATOR_TIMEOUT;
							break;
						}

					case execCmdWritten:
						{
							//statusLine << "Sent Execute Command to " << datapoint->getEntityName() << ".";
							statusLine << "发送执行命令到 " << datapoint->getDescription() << ".";
							addStatusLine = true;
							returnString = DPT_CONTROL_EXECUTE_SENT;
							break;
						}

					case execCmdConfirm:
						{
							//statusLine << "Recv Execute Confirm from " << datapoint->getEntityName() << ".";
							statusLine << "从" << datapoint->getDescription() << "接收执行确认.";
							addStatusLine = true;
							returnString = DPT_CONTROL_EXECUTE_CONFIRM;
							break;
						}

					case execCmdConfirmTimeout:
						{
							//statusLine << "Execute Confirm from " << datapoint->getEntityName() << " failed due to timeout.";
							statusLine << "从" << datapoint->getDescription() << "接收执行确认失败，原因是超时.";
							addStatusLine = true;
							returnString = DPT_CONTROL_EXECUTE_TIMEOUT;
							break;
						}
					case execCmdSuccess:
						{
							//statusLine << "Recv Execute Success from " << datapoint->getEntityName() << ".";
							statusLine << "从" << datapoint->getDescription() << "接收执行结束.";
							addStatusLine = true;
							returnString = DPT_CONTROL_EXECUTE_SUCCESS;
							break;
						}

					case execCmdSuccessTimeout:
						{
							//statusLine << "Execute Success from " << datapoint->getEntityName() << " failed due to timeout.";
							statusLine << "从 " << datapoint->getDescription() << "接收执行成功消息失败，原因是超时.";
							addStatusLine = true;
							returnString = DPT_CONTROL_EXECUTE_SUC_TIMEOUT;
							break;
						}
					case cancelCmdWritten:
						{
							//statusLine << "Sent Cancel Command to " << datapoint->getEntityName() << ".";
							statusLine << "发送取消命令到" << datapoint->getDescription() << ".";
							addStatusLine = true;
							returnString = DPT_CONTROL_CANCEL_SENT;
							break;
						}

					case cancelCmdConfirm:
						{
							//statusLine << "Recv Cancel Confirm from " << datapoint->getEntityName() << ".";
							statusLine << "从 " << datapoint->getDescription() << "接收取消确认消息.";
							addStatusLine = true;
							returnString = DPT_CONTROL_CANCEL_CONFIRM;
							break;
						}

					case cancelCmdConfirmTimeout:
						{
							//statusLine << "Cancel Confirm from " << datapoint->getEntityName() << " failed due to timeout.";
							statusLine << "从 " << datapoint->getDescription() << " 取消确认失败，原因是超时.";
							addStatusLine = true;
							returnString = DPT_CONTROL_CANCEL_TIMEOUT;
							break;
						}
					case selectCmdCfmNegative:
						{
							statusLine << "从 " << datapoint->getDescription() << " 选择确认失败， 原因是设备否定.";
							addStatusLine = true;
							returnString = DPT_CONTROL_FAILED;
							break;
						}
					case execCmdCfmNegative:
						{
							statusLine << "从 " << datapoint->getDescription() << " 执行确认失败， 原因是设备否定.";
							addStatusLine = true;
							returnString = DPT_CONTROL_FAILED;
							break;
						}
					case cancelCmdCfmNegative:
						{
							statusLine << "从 " << datapoint->getDescription() << " 取消确认失败，原因是设备否定.";
							addStatusLine = true;
							returnString = DPT_CONTROL_FAILED;
							break;
						}
					case deviceControlException:
						{
							statusLine << "从 " << datapoint->getDescription() << " 执行失败，原因是设备控制异常.";
							addStatusLine = true;
							returnString = DPT_CONTROL_FAILED;
							break;
						}
					case noControl:
					default:
						{
							returnString = DPT_CONTROL_NOCONTROL;
							break;
						}
				}

				if ( true == addStatusLine )
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", statusLine.str().c_str() );

					std::string* statusLineStr = new std::string(statusLine.str());
					AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), datapoint->getLastStatusChangeTimestamp());
				}		
			}
		}
		catch (TA_Base_Core::ScadaProxyException& e)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());
		}
		catch ( const TA_Base_Core::ObjectResolutionException & ex )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
		}
		catch ( const TA_Base_Core::OperationModeException & )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
		}
        catch ( const CORBA::Exception & ex )
		{
			LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
		}
		catch ( ... )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"Unknown exception caught while adding the control status to status box");
		}

		FUNCTION_EXIT;
		return returnString;
	}

	void CButtonControlListCtrl::updatePermissions(bool skipAccessCheck)
	{		
		if (NULL == m_equipment)
		{
			return;
		}

		std::vector<TA_Base_Bus::DataPointProxySmartPtr*>& dataPoints = m_equipment->getChildDataPointVector();
		std::map<unsigned int, bool>::iterator it;

		for (it = m_setValueActionAllowed.begin(); it != m_setValueActionAllowed.end(); it++)
		{
			bool isDPWriteAllowed = false;
			
			if (skipAccessCheck == false)
			{
				isDPWriteAllowed = AccessControlHelper::getInstance()->IsActionPermitted(
													(*dataPoints[(*it).first])->getEntityKey(), 
													AccessControlHelper::DATA_POINT_WRITE);
			}

			bool isControlInhibited = false;
			
			try
			{
				isControlInhibited = (*dataPoints[(*it).first])->isControlInhibited();
			}
			catch (...)
			{
				isControlInhibited = false;
			}

			// check if control inhibited. If so, cannot update value
			if (isControlInhibited)
			{
				isDPWriteAllowed = false;
			}

			(*it).second = isDPWriteAllowed; 
		}
		displayDataPoints();
		EnableWindow(true);
	}


	void CButtonControlListCtrl::RefreshPermission(unsigned int iPoint)
	{
		std::map<unsigned int, bool>::iterator it;

		TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(iPoint);

		bool isControlInhibited = false;

		try
		{
			isControlInhibited = dataPoint->isControlInhibited();
		}
		catch (...)
		{
			isControlInhibited = false;
		}


		TA_THREADGUARD(m_valueActionLock);

		it = m_setValueActionAllowed.find(iPoint);

		// if found
		if (it != m_setValueActionAllowed.end())
		{
			if (isControlInhibited)
				(*it).second = false; 
			else
			{
				(*it).second = AccessControlHelper::getInstance()->IsActionPermitted(
					dataPoint->getEntityKey(), 
					AccessControlHelper::DATA_POINT_WRITE);
			}
		}
		else
		{
			if (isControlInhibited)
				m_setValueActionAllowed.insert(std::map<unsigned int, bool>::value_type(iPoint, false));
			else
				m_setValueActionAllowed.insert(std::map<unsigned int, bool>::value_type(iPoint, AccessControlHelper::getInstance()->IsActionPermitted(
				dataPoint->getEntityKey(), 
				AccessControlHelper::DATA_POINT_WRITE)));
		}
	}


	void CButtonControlListCtrl::RefreshButtonDisplay(unsigned int iPoint,int nItem)
	{

		TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(iPoint);

		TA_THREADGUARD(m_valueActionLock);
		std::map<unsigned int, bool>::iterator it;
		it = m_setValueActionAllowed.find(iPoint);
		// if found
		if (it != m_setValueActionAllowed.end())
		{
			if (dataPoint->getPScadaFlag())
			{
				if (it->second)
				{
					CControlListCellButton *button = getButton(nItem);

					if (button->getValue() == "")
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "refresh button 1");
						EnableButton(nItem,false);
					}
					else if (button->isInProgress())
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "refresh button 2");
						EnableButton(nItem,false);
					}
					else
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "refresh button 3");
						EnableButton(nItem,true);
					}
				}
				else
				{
					EnableButton(nItem,false);
				}
			}
			else
			{
				if (it->second)
				{
					EnableButton(nItem*2,true);
					EnableButton(nItem*2 + 1,true);
				}
				else
				{
					EnableButton(nItem*2,false);
					EnableButton(nItem*2 + 1,false);
				}

			}

		}
	}
	void CButtonControlListCtrl::OnDrawCellButtons()
	{
		std::map<unsigned long,CControlListCellButton*>::iterator it = m_buttonMap.begin();
		for (;it != m_buttonMap.end();it++)
		{
			int nItem = it->second->getItem();
			CRect cellRect;
			GetSubItemRect(nItem, NEW_VALUE_COLUMN, LVIR_LABEL, cellRect);

			CRect   buttonRect; 
			it->second->GetWindowRect(&buttonRect);

			//todo bug buttonRec.Width =0 cellrect.width=0
			if ( (buttonRect.Width()*2) > cellRect.Width())// one Button
			{
				it->second->MoveWindow(cellRect);
			}
			else	// Two Button
			{
				if ( ((it->first)%2) == 0 )// 2*nItem
				{
					it->second->MoveWindow(cellRect.left,cellRect.top,cellRect.Width()/2,cellRect.Height());
				} 
				else // 2*nItem + 1
				{
					it->second->MoveWindow(cellRect.left+cellRect.Width()/2,cellRect.top,cellRect.Width()/2,cellRect.Height());
				}				
			}
		}
	}

    // PW2805 - display cells with customised colours 
    void CButtonControlListCtrl::onCustomDrawList( NMHDR* pNMHDR, LRESULT* pResult )
    {					
		NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

        // Take the default processing unless we set this to something else below.
        *pResult = CDRF_DODEFAULT;

		if (NULL != m_equipment)
		{
			std::vector<unsigned long>& vectorIndex = m_equipment->getWriteableIndex(); 
			if (0 == vectorIndex.size())
			{
				return;
			}
		}
		else
		{
			return;
		}

        // First thing - check the draw stage. If it's the control's prepaint
        // stage, then tell Windows we want messages for every item.
        if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
        {
            *pResult = CDRF_NOTIFYITEMDRAW;
			*pResult |= CDRF_NOTIFYPOSTPAINT;
        }
		else if (CDDS_POSTPAINT == pLVCD->nmcd.dwDrawStage)
		{
			OnDrawCellButtons();
			*pResult =  CDRF_DODEFAULT; 
		}
        else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
        {
            // This is the prepaint stage for an item. Here's where we set the
            // item's text color. Our return value will tell Windows to draw the
            // item itself, but it will use the new color we set here.
            // We'll cycle the colors through red, green, and light blue.

            // Tell Windows to paint the control itself.
			*pResult |= CDRF_DODEFAULT; 
            *pResult |= CDRF_NOTIFYSUBITEMDRAW; 
        }
		else if ( (CDDS_ITEMPREPAINT | CDDS_SUBITEM) == pLVCD->nmcd.dwDrawStage)
		{
			// This is the prepaint stage for a subitem. Here's where we set the
            // item's text and background colors. Our return value will tell 
            // Windows to draw the subitem itself, but it will use the new colors
            // we set here.

            // use the colour of the item
            pLVCD->clrText = COLOUR_ALARM;

			std::map<unsigned int, bool>::iterator it = 
				m_setValueActionAllowed.find(static_cast<unsigned int>(pLVCD->nmcd.lItemlParam));

			try
			{
	            if (it != m_setValueActionAllowed.end() && (*it).second)
	            {
	                // normal colour
	                pLVCD->clrText = COLOUR_NORMAL;   // black
	            }
	            else
	            {
	                pLVCD->clrText = COLOUR_DISABLE;  // grey  
	            }
			}
			catch(...)
			{
				pLVCD->clrText = COLOUR_DISABLE;  // grey
			}

            // if this is the state column and the datapoint is in failed state, display red instead
            if (CONTROL_STATE_COLUMN == pLVCD->iSubItem )
			{
				try
				{
					TA_Base_Bus::EDataPointControlState controlState = noControl;
					TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(static_cast<unsigned int>(pLVCD->nmcd.lItemlParam));
					controlState = dataPoint->getControlState();
				
					if ((controlState == LCC_failed) || 
						(controlState == RCC_failed) || 
						(controlState == comms_failed))
					{
						pLVCD->clrText = COLOUR_ALARM;    // red						
					}
				}
				catch (TA_Base_Core::ScadaProxyException& e)
				{
					// Not always available
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());
				}
			    catch ( const TA_Base_Core::ObjectResolutionException & ex )
			    {
				    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException" ,ex.what() );
			    }
			    catch ( const TA_Base_Core::OperationModeException & )
			    {
				    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
			    }
                catch ( const CORBA::Exception & ex )
			    {
				    LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
			    }
			    catch (...)
			    {
				    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", "Unknown Exception" );
			    }
			}

            // Tell Windows to paint the control itself.
            *pResult = CDRF_DODEFAULT;    
		}
    }

	bool CButtonControlListCtrl::isAnyDataPointsDisplayedOnScreen() const
	{
		return m_equipment->hasControlPoint();
	}

	//TD15438, override the function to get a chance to adjust columns width.
	BOOL CButtonControlListCtrl::SetWindowPos( const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags )
	{
		//Call the base class member first
		BOOL isSucceeded = CListCtrl::SetWindowPos(pWndInsertAfter, x, y, cx, cy, nFlags);
		
		//To adjust the column width if needed.
		if (isSucceeded)
		{
			CRect rectItem, rectHeader, rectList;
			GetItemRect(0, &rectItem, LVIR_BOUNDS);
			GetHeaderCtrl()->GetWindowRect(&rectHeader);
			GetClientRect(&rectList);
			
			//if horizontal and scroll bar appear, adjust a column to let horizontal scroll bar dispear
			if (rectList.Height() < rectItem.Height() * GetItemCount() + rectHeader.Height() &&
				rectList.Width() < rectItem.Width())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Column Width is adjusted: itemH=%d; itemNo=%d; itemW=%d; headH=%d; listH=%d; listW=%d", 
					rectItem.Height(), GetItemCount(), rectItem.Width(), rectHeader.Height(), rectList.Height(), rectList.Width());
				
				int columnWidth = GetColumnWidth(NEW_VALUE_COLUMN);
				int vsrollWidth = GetSystemMetrics(SM_CXVSCROLL);
				
				//the minimum width is 100
				if (columnWidth - vsrollWidth > 100)
				{
					SetColumnWidth(NEW_VALUE_COLUMN, columnWidth - vsrollWidth);
				}
			}
		}
		
		return isSucceeded;
	}

	void CButtonControlListCtrl::DestroyButton(int item)
	{

		std::map<unsigned long,CControlListCellButton*>::iterator iter  = m_buttonMap.find(item);
		if (iter != m_buttonMap.end())
		{			
			CControlListCellButton* pButton = iter->second;
			pButton->DestroyWindow();
		} 

	}

	void CButtonControlListCtrl::EnableButton(int item,bool bShow)
	{
		std::map<unsigned long,CControlListCellButton*>::iterator iter  = m_buttonMap.find(item);
		if (iter != m_buttonMap.end())
		{			
			CControlListCellButton* pButton = iter->second;
			pButton->EnableWindow(bShow);
		} 
	}

	
	CControlListCellButton* CButtonControlListCtrl::getButton(int item) 
	{
		std::map<unsigned long,CControlListCellButton*>::iterator iter  = m_buttonMap.find(item);
		if (iter != m_buttonMap.end())
		{			
			return iter->second;
		} 
		else
		{
			return NULL;
		}
	}

	void CButtonControlListCtrl::setButtonText(int item, CString strValue, CString strCaption)
	{
		std::map<unsigned long,CControlListCellButton*>::iterator iter  = m_buttonMap.find(item);
		if (iter != m_buttonMap.end())
		{			
			CControlListCellButton* pButton = iter->second;
			pButton->setValue(strValue);
			pButton->SetWindowText((LPCTSTR)strCaption);
		} 
	}

	void CButtonControlListCtrl::showStatusMessage(const std::string& strMessage)
	{
		std::string *statusString = new std::string(strMessage);
		AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, (WPARAM)statusString, 0);	
	}


	BOOL CButtonControlListCtrl::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
	{
		HD_NOTIFY   *pHDN = (HD_NOTIFY*)lParam;
		if((pHDN->hdr.code == HDN_BEGINTRACKW || pHDN->hdr.code == HDN_BEGINTRACKA) && pHDN->iItem == 3)
		{
			*pResult = TRUE;		
			return TRUE;      
		}
		return CListCtrl::OnNotify(wParam, lParam, pResult);
	}
}
