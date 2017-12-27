/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_Review_Branch/TA_BASE/transactive/app/scada/inspector_panel/src/ControlListCtrl.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/10/21 12:46:03 $
  * Last modified by:  $Author: wanzl $
  * 
  * This class manages the list display used to show the basic control information in
  * the CControlDialog class.
  *
  */
 
#include "app/scada/inspector_panel/src/stdafx.h"

#include "app/scada/inspector_panel/src/AccessControlHelper.h"
#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/InspectorPanelDlg.h"
#include "app/scada/inspector_panel/src/Equipment.h"
#include "app/scada/inspector_panel/src/ConfirmCommandDialog.h"
#include "app/scada/inspector_panel/src/EOControlListCtrl.h"
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
    const COLORREF CDropDownControlListCtrl::COLOUR_ALARM = 0x000000FF;  // colour to be used for showing alarms
    const COLORREF CDropDownControlListCtrl::COLOUR_DISABLE = 0x00808080; // colour to be used for showing disabled data points
    const COLORREF CDropDownControlListCtrl::COLOUR_NORMAL = 0x00000000; // colour to be used for showing normal data points

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
	};

	CDropDownControlListCtrl::CDropDownControlListCtrl()
	:
	m_equipment(NULL),
	//CEditableListCtrl(true),
	m_initialState(false), //wenching++ (TD14179)
	m_dataPointsDisplayedOnScreen ( false )
	{
		FUNCTION_ENTRY ("CDropDownControlListCtrl()");

		// We will manage the cell requests
		SetManager(this);

		FUNCTION_EXIT;
	}

	CDropDownControlListCtrl::~CDropDownControlListCtrl()
	{
		FUNCTION_ENTRY ("~CDropDownControlListCtrl()");

		FUNCTION_EXIT;
	}

	/* SetEquipment
	 *
	 * This function will set the name of the equipment that the DataPoints are to be displayed for.It can 
	 * be called before or after the windows control is on the screen.
	 */
	void CDropDownControlListCtrl::setEquipment(CEquipment* equipment)
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

	BEGIN_MESSAGE_MAP(CDropDownControlListCtrl, CEditableListCtrl)
		//{{AFX_MSG_MAP(CDropDownControlListCtrl)
		ON_WM_CREATE()
		//}}AFX_MSG_MAP
    	ON_NOTIFY_REFLECT ( NM_CUSTOMDRAW, onCustomDrawList )
	END_MESSAGE_MAP()



	/////////////////////////////////////////////////////////////////////////////
	// CDataPointListCtrl message handlers

	int CDropDownControlListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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


	void CDropDownControlListCtrl::PreSubclassWindow() 
	{
		FUNCTION_ENTRY ("PreSubclassWindow()");

		CWnd::PreSubclassWindow();
		setupList();
		displayDataPoints();

		FUNCTION_EXIT;
	}


	// Helper functions

	void CDropDownControlListCtrl::setupList()
	{
		FUNCTION_ENTRY ("SetupList()");
		SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | 
			LVS_EX_SUBITEMIMAGES | LVS_EDITLABELS);
// 		SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | 
// 											  LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES | 
// 											  CAbstractDataPointListCtrl::LVS_EX_LABELTIP );
		// Headings
   		this->InsertColumn(EQUIPMENT_ATTRIBUTES_COLUMN,		"描述");
		this->InsertColumn(DATAPOINT_TYPE_COLUMN,			"类型",LVCFMT_CENTER);
		this->InsertColumn(CONTROL_STATE_COLUMN,			"状态");
   		this->InsertColumn(NEW_VALUE_COLUMN,				"操作");

		this->SetColumnWidth(EQUIPMENT_ATTRIBUTES_COLUMN,	180);
		this->SetColumnWidth(DATAPOINT_TYPE_COLUMN,			45);
		this->SetColumnWidth(CONTROL_STATE_COLUMN,			92);
		this->SetColumnWidth(NEW_VALUE_COLUMN,				130);

		FUNCTION_EXIT;
	}

	void CDropDownControlListCtrl::UpdateControlState()
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
	void CDropDownControlListCtrl::displayDataPoints()
	{
		FUNCTION_ENTRY ("DisplayDataPoints()");

		if (m_equipment==NULL)
		{
			FUNCTION_EXIT;
			return;
		}

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

				// Control State
				value = getControlStatusString((*dataPoints[iPoint])->getControlState());
				SetItemText(nItem, CONTROL_STATE_COLUMN, value.c_str());
			

				// Value - only displayed if the control state is displayed
				if (value != "")
				{	
					value = (*dataPoints[iPoint])->getValueAsString();	
				}
				SetItemText(nItem,NEW_VALUE_COLUMN,value.c_str());

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


	// ICellManager Implementation
	ICellManager::CellType CDropDownControlListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		FUNCTION_ENTRY ("GetCellType()");

		// Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)
		// Also check if there is any pending control command or Only value column is editable
		if (item<0 || subItem!=NEW_VALUE_COLUMN) 
		{
			FUNCTION_EXIT;
			return CT_NOTEDITABLE;
		}

		// Get the DataPoint array position
		int iPoint = listCtrl->GetItemData(item);
		TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(iPoint);
		DataNodeProxySmartPtr& dataNode = m_equipment->getDataNode();

        try
        {
		    // If pending, disable the cell to prevent the next control to be sent
		    if (pending == dataPoint->getControlState())
		    {
			    FUNCTION_EXIT;
			    return CT_NOTEDITABLE;
		    }
        }
		catch (TA_Base_Core::ScadaProxyException &e)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Failed to get ControlState %s", e.what());
		}

		// Check for rights
		std::map<unsigned int, bool>::iterator it = m_setValueActionAllowed.find(iPoint);

		if (it != m_setValueActionAllowed.end() && !(*it).second)
		{
			std::stringstream message;
			message << dataPoint->getDescription() << ": No Access.";

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message.str().c_str() );

			FUNCTION_EXIT;
			return CT_NOTEDITABLE;
		}

		try
		{
			if (!dataPoint->isControlInhibited())
			{
				// Check if boolean, text or combo
				if (dataPoint->isBoolean() || dataPoint->isDerived())
				{
					FUNCTION_EXIT;
					return CT_SELECTION;
				}
				else if (dataPoint->isNumber())
				{
					FUNCTION_EXIT;
					return CT_NUMBER;
				}
				else if (dataPoint->isText())
				{
					FUNCTION_EXIT;
					return CT_TEXT;
				}
			}
			else
			{
				std::stringstream message;
				message << dataPoint->getDescription() << ": Control Inhibited.";

				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", message.str().c_str() );

				std::string* statusLineStr = new std::string(message.str());
				AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

				FUNCTION_EXIT;
				return CT_NOTEDITABLE;
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
		return CT_NOTEDITABLE;
	};


    unsigned long CDropDownControlListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        return 0;
    }


	ICellManager::CellData CDropDownControlListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		FUNCTION_ENTRY ("GetCellData()");

		TA_ASSERT (m_equipment!=NULL,"CDropDownControlListCtrl::GetCellData() - Equipment not defined");

		ICellManager::CellData   data;

		if (subItem == NEW_VALUE_COLUMN)
		{
			TA_Base_Bus::DataPointProxySmartPtr &dataPoint = m_equipment->getChildDataPoint(listCtrl->GetItemData(item));

			try
			{
				if (dataPoint->isBoolean())
				{
					std::string label = "";

					try
					{
						label = dataPoint->getBooleanLabel(true);
						data.push_back(label);
					}
			        catch (TA_Base_Core::ScadaProxyException &e)
			        {
				        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());
			        }

					try
					{
						label = dataPoint->getBooleanLabel(false);
						data.push_back(label);
					}
			        catch (TA_Base_Core::ScadaProxyException &e)
			        {
				        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());
			        }
				}
				else if (dataPoint->isDerived())
				{
					try
					{
						DpvEnumLabelsMap derivedStates =
							dataPoint->getDerivedStatesAsMap();

						DpvEnumLabelsMap::iterator it;

						for (it = derivedStates.begin(); it != derivedStates.end(); it++)
						{
							data.push_back((*it).second);
						}
					}
					catch (TA_Base_Core::ScadaProxyException &e)
			        {
                        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());
                    }
                }
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
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Unknown Exception in CDropDownControlListCtrl::GetCellData");
			}
		}

		FUNCTION_EXIT;

		return data;
	}

    CString CDropDownControlListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return "";
    }

    char CDropDownControlListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return 0;
    }

	bool CDropDownControlListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
	{
		FUNCTION_ENTRY ("UpdateCellValue()");

        bool updateListCell = false;
		try 
		{

		    UINT iPoint = listCtrl->GetItemData(item);
		    std::string  strValue = value;

            if (strValue.empty())
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Control value is empty, not processing!");
                return updateListCell;
            }
		    
		    TA_ASSERT (m_equipment!=NULL,"CDropDownControlListCtrl::UpdateCellValue() - Equipment not defined.");

		    // Get the DataPoint array position
		    TA_Base_Bus::DataPointProxySmartPtr &dataPoint = m_equipment->getChildDataPoint(iPoint);
		    DataNodeProxySmartPtr& dataNode = m_equipment->getDataNode();


            if (!DataPointControlHelper::isControlValueValid(dataPoint, strValue))
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Control value is invalid!", strValue);

			    FUNCTION_EXIT;
                return false;
            }

		    CConfirmCommandDialog     dlg;

		    // Confirm update
		    dlg.m_equipment = dataNode->getAssetName().c_str();
		    dlg.m_equipment += "\r\n";
		    dlg.m_equipment += dataNode->getLabel().c_str();

		    dlg.m_dataPoint = dataPoint->getDescription().c_str();
		    dlg.m_newValue  = value;

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

				    updateListCell = true;
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
		catch (...)
		{
			AfxMessageBox( _T("updatecell exeption"));
		}
		FUNCTION_EXIT;
		return updateListCell;
	}

	void CDropDownControlListCtrl::updateDataPoint(unsigned long key, ScadaEntityUpdateType updateType)
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
        case AlarmSummaryUpdate://limin++, TD20740
        case AckSummaryUpdate:  //limin++, TD20740
		case InhibitStateUpdate:
		case QualitySummaryUpdate:
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

	void CDropDownControlListCtrl::updateDataNode(ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataNode()");

		// No datanode details here

		FUNCTION_EXIT;
	}

	void CDropDownControlListCtrl::updateDataPointDisplay ( int iPoint, int nItem, TA_Base_Bus::ScadaEntityUpdateType updateType )
	{
		FUNCTION_ENTRY ("UpdateDataPointDisplay()");

		if (m_equipment == NULL)
		{
			FUNCTION_EXIT;
			return;
		}

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

				// Value - if the control state is displayed, get the value
				// Value - only displayed if the control state is displayed
				if (value != "")
				{
					value = dataPoint->getValueAsString();
				}
				SetItemText(nItem,NEW_VALUE_COLUMN,value.c_str());
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

	std::string CDropDownControlListCtrl::getControlStatusString ( TA_Base_Bus::EDataPointControlState state ) 
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

	std::string CDropDownControlListCtrl::addControlStateToStatusLine ( TA_Base_Bus::DataPointProxySmartPtr & datapoint, TA_Base_Bus::EDataPointControlState state ) //wenching++ (TD14179)
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

	void CDropDownControlListCtrl::updatePermissions(bool skipAccessCheck)
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

    // PW2805 - display cells with customised colours 
    void CDropDownControlListCtrl::onCustomDrawList( NMHDR* pNMHDR, LRESULT* pResult )
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

	bool CDropDownControlListCtrl::isAnyDataPointsDisplayedOnScreen() const
	{
		return m_equipment->hasControlPoint();
	}

	//TD15438, override the function to get a chance to adjust columns width.
	BOOL CDropDownControlListCtrl::SetWindowPos( const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags )
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

	void CDropDownControlListCtrl::RefreshPermission( unsigned int iPoint )
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

}
