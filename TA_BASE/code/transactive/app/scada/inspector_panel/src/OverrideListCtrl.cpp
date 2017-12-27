/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/OverrideListCtrl.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/07/02 16:25:32 $
  * Last modified by:  $Author: jinmin.huang $
  * 
  * This class manages the list display used to show the basic information in
  * the CInformationDialog class.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "app/scada/Inspector_Panel/src/stdafx.h"

#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/Equipment.h"
#include "app/scada/inspector_panel/src/InspectorPanelDlg.h"
#include "app/scada/inspector_panel/src/ConfirmOverrideDialog.h" 
#include "app/scada/inspector_panel/src/OverrideListCtrl.h"
#include "app/scada/Inspector_Panel/src/AccessControlHelper.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"

#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/types/InspectorPanelAudit_MessageTypes.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
	enum columns
	{
		DUMMY_COLUMN				= 0,
		DESCRIPTION_COLUMN			= 1,
		DATAPOINT_TYPE_COLUMN		= 2,
		OVERRIDE_ENABLE_COLUMN		= 3,
		OVERRIDE_VALUE_COLUMN		= 4,
		CURRENT_VALUE_COLUMN		= 5
	};

	/////////////////////////////////////////////////////////////////////////////
	// COverrideListCtrl

	COverrideListCtrl::COverrideListCtrl() 
	:
	CAbstractDataPointListCtrl(),
	m_dataPointsDisplayedOnScreen ( false )
	{
		FUNCTION_ENTRY ("COverrideListCtrl()");

		// We will manage the cell requests
		SetManager(this);

		FUNCTION_EXIT;
	}

	COverrideListCtrl::~COverrideListCtrl()
	{
		FUNCTION_ENTRY ("~COverrideListCtrl()");
		FUNCTION_EXIT;
	}

	/* SetEquipment
	 *
	 * This function will set the name of the equipment that the data points are to be displayed for.It can 
	 * be called before or after the windows control is on the screen.
	 */

	// Helper functions

	void COverrideListCtrl::setupList()
	{
		FUNCTION_ENTRY ("setupList()");

		SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES | CAbstractDataPointListCtrl::LVS_EX_LABELTIP_XXX);

		// Headings
   		this->InsertColumn(DUMMY_COLUMN,				"Dummy");
		//this->InsertColumn(DESCRIPTION_COLUMN,			"Description");
		//this->InsertColumn(DATAPOINT_TYPE_COLUMN,		"Type");
		//this->InsertColumn(OVERRIDE_ENABLE_COLUMN,		"Ovr");
		//this->InsertColumn(OVERRIDE_VALUE_COLUMN,		"Override Value");
		//this->InsertColumn(CURRENT_VALUE_COLUMN,		"Current Value"); // FAT TES# 228

		this->InsertColumn(DESCRIPTION_COLUMN,			"描述");
		this->InsertColumn(DATAPOINT_TYPE_COLUMN,		"类型",LVCFMT_CENTER);
		this->InsertColumn(OVERRIDE_ENABLE_COLUMN,		"超驰",LVCFMT_CENTER);
		this->InsertColumn(OVERRIDE_VALUE_COLUMN,		"超驰值");
		this->InsertColumn(CURRENT_VALUE_COLUMN,		"当前值"); // FAT TES# 228

		// Using a 'dummy' column for index 0 prevents
		// MFC from indenting the text by the image size for the
		// 'real' first column
		this->SetColumnWidth(DUMMY_COLUMN,				0);
		this->SetColumnWidth(DESCRIPTION_COLUMN,		180);
		this->SetColumnWidth(DATAPOINT_TYPE_COLUMN,		45);
		this->SetColumnWidth(OVERRIDE_ENABLE_COLUMN,	45);
		this->SetColumnWidth(OVERRIDE_VALUE_COLUMN,		90);
		this->SetColumnWidth(CURRENT_VALUE_COLUMN,		90);

		FUNCTION_EXIT;
	}

	void COverrideListCtrl::updateDataPointDisplay(int iPoint, int nItem, ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("updateDataPointDisplay()");

		switch (updateType)
		{
		case ValueStateUpdate:
        case AlarmSummaryUpdate://limin++, TD20740
        case AckSummaryUpdate:  //limin++, TD20740
        case QualitySummaryUpdate:
			displayItem(nItem);
			break;

		default:
			break;
		}

		FUNCTION_EXIT;
	}

	// By using the preprocessed index vector, reduce linear search and ensure only those
	// datapoints had been sorted and config valid were displayed.	
	void COverrideListCtrl::displayDataPoints()
	{
		FUNCTION_ENTRY ("displayDataPoints()");

		int validItem = 0;
		int difference = 0;
		int totalItem = GetItemCount();

		// No equipment defined yet.
		if (m_equipment==NULL) 
			return;

		int				nItem = 0;
		std::string		value;
		unsigned int	iPoint;

		std::vector<TA_Base_Bus::DataPointProxySmartPtr*>& dataPoints = m_equipment->getChildDataPointVector();

		// Get the preprocessed indexes with the required config.
		std::vector<unsigned long>& vectorIndex = m_equipment->getNotWriteableIndex();

		if (0 == vectorIndex.size())
		{
			DeleteAllItems();
			return;
		}

		// Only display the datapoints with index in the vector
		for (unsigned i=0;i<vectorIndex.size();i++)
		{
			iPoint = vectorIndex.at(i);

			try
			{		
				validItem++;

    			// Description (Equipment Attributes)
				if( validItem > GetItemCount() )
				{
                    std::string description = (*dataPoints[iPoint])->getDescription();
                    nItem = insertItemTextImage(iPoint, validItem-1, description, false);
				}
				else
				{
					nItem = validItem-1;
				}
				SetItemData(nItem, iPoint);

				displayItem(nItem, REFRESH_ALL);								

				// set the flag to indicate data points are displayed on screen
				m_dataPointsDisplayedOnScreen = true;
			
			}
			catch (TA_Base_Core::ScadaProxyException &e)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "%s", e.what());
			}
		}
	
		// Delete invalid item
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

	void COverrideListCtrl::refreshDataPoints()
	{
		FUNCTION_ENTRY ("refreshDataPoints()");

		// No equipment defined yet.
		if (m_equipment==NULL) 
			return;
		
		displayDataPoints();	

		FUNCTION_EXIT;
	}

	// ICellManager Implementation
	ICellManager::CellType COverrideListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		FUNCTION_ENTRY ("GetCellType()");

		// Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)
		// or No access to this dialog
		if (item<0 || !m_accessAllowed)	
		{
			return CT_NOTEDITABLE;
		}

		TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(GetItemData(item));

		try
		{
			switch (subItem)
			{
			case OVERRIDE_ENABLE_COLUMN:
				if (dataPoint->isWriteable() == false)
				{
					return (isCellEnabled (item, subItem) ? CT_CHECKBOX : CT_CHECKBOX_DISABLED);
				}
				return CT_NOTEDITABLE;

			case OVERRIDE_VALUE_COLUMN:
				if (dataPoint->isWriteable() == false)
				{
					if (true == dataPoint->isOverrideEnabled())
					{
						if (dataPoint->isBoolean() || dataPoint->isDerived())
						{
							return (isCellEnabled (item, subItem) ? CT_SELECTION : CT_NOTEDITABLE);
						}

						if (dataPoint->isNumber())
						{
							return (isCellEnabled (item, subItem) ? CT_NUMBER : CT_NOTEDITABLE);
						}
					}
				}
				return CT_NOTEDITABLE;

			default:
				return CT_NOTEDITABLE;
			}
		}
		catch (TA_Base_Core::ScadaProxyException &e)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "%s", e.what());
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
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Unknown Exception in CControlListCtrl::GetCellData");
		}		

		FUNCTION_EXIT;

		return CT_NOTEDITABLE;
	}


    unsigned long COverrideListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        return 0;
    }


	ICellManager::CellData COverrideListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
	{
		FUNCTION_ENTRY ("GetCellData()");

		TA_ASSERT (m_equipment!=NULL,"Equipment not defined");

		ICellManager::CellData   data;

		if (subItem == OVERRIDE_VALUE_COLUMN)
		{
			TA_Base_Bus::DataPointProxySmartPtr &dataPoint = m_equipment->getChildDataPoint(GetItemData(item));

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
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Unknown Exception in CControlListCtrl::GetCellData");
			}
		}

		FUNCTION_EXIT;

		return data;
	}

    CString COverrideListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return "";
    }

    char COverrideListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return 0;
    }

	bool COverrideListCtrl::inhibitOverrideConfirm(const int inhibitOverrideItem, TA_Base_Bus::DataPointProxySmartPtr& dataPoint, bool enable)
	{
		FUNCTION_ENTRY ("inhibitOverrideConfirm()");

        CString actionName, actionType;
        actionType.LoadString(inhibitOverrideItem);
		
		CString dpDescription = dataPoint->getDescription().c_str();
		int pos = dpDescription.Find ( "%", 0 );
		if ( -1 != pos )
		{
			dpDescription.Insert ( pos, "%" );
		}

        AfxFormatString2(actionName, enable?IDS_ENABLE_DATAPOINT:IDS_DISABLE_DATAPOINT, actionType, dpDescription );

        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003, "监控面板");        
        FUNCTION_EXIT;
		return (selectedButton == IDYES);
	}

	bool COverrideListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
	{
		FUNCTION_ENTRY ("UpdateCellValue()");

		TA_ASSERT (m_equipment!=NULL,"Equipment not defined.");

		std::string  strValue = value;
		std::string message;
		bool cellValueUpdated = false;

		// Get the data point array position
		UINT iPoint = listCtrl->GetItemData(item);
		TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(iPoint);

		std::string overrideValue ( "" );
		std::stringstream statusLine;
		statusLine << dataPoint->getDescription() << ": ";

		try
		{
			// only applicable to input data points
			if (dataPoint->isWriteable() == false)
			{
				switch (subItem)
				{
					case OVERRIDE_ENABLE_COLUMN:
					{
						// if the override state is different
						if ( (strValue == "Yes") != dataPoint->isOverrideEnabled() )
						{
							// if the checkbox is checked
							if (strValue == "Yes")
							{
								// confirm operator access on this data point
								bool isSetForcedPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
									dataPoint->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_SET_FORCED_STATE);

								// if the current operator profile does not have this access
								if (!isSetForcedPermitted)
								{
									//statusLine << "No Access.";
									statusLine << "无超驰权限.";
									break;
								}

								// else if the operator profile has the access
								else
								{
									// get confirmation from the operator
									CConfirmOverrideDialog dlg(NULL, iPoint, m_equipment);

									// if acknowledged to go ahead
									if (dlg.DoModal() == IDOK)
									{
										// get the value entered by the operator
										overrideValue = dlg.getOverrideValue();

										//
										// perform sanity checks
										//

										// if the value entered by the operator is empty 
										if ( overrideValue == "" )
										{
											// show text in Status box
											//  [3/14/2010 benxun]
											//statusLine << "Override Failed: entered value is empty.";
											statusLine << "超驰失败，原因是输入值为空.";
											//  [3/14/2010 benxun]
											std::string* statusLineStr = new std::string(statusLine.str());
											AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
											break;
										}

										// if this data point is an analogue, i.e not a boolean, not a text nor a derived,
										// do range checking
										if ( ( false == dataPoint->isBoolean() ) &&
											 ( false == dataPoint->isText() ) &&
											 ( false == dataPoint->isDerived() ) )
										{
											double doubleValue = 0;
											std::istringstream valueStream ( overrideValue );

											// if the value entered by the operator is purely a numerical string
											if ( valueStream >> doubleValue )
     										{
												// if the eng unit is defined
												if ( "" != dataPoint->getUnit() )
												{
													// if the value entered by the operator is outside
													// the range
													if ( ( doubleValue < dataPoint->getLimit ( true ) ) ||	// low eng limit
														 ( doubleValue > dataPoint->getLimit ( false ) ) )	// high eng limit
													{
														// show text in Status box
														//  [3/14/2010 benxun]
														//statusLine << "Override Failed: entered value is not within range.";
														statusLine << "超驰失败，原因是输入值不在有效范围内.";
														//  [3/14/2010 benxun]
														std::string* statusLineStr = new std::string(statusLine.str());
														AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

														std::ostringstream message2;
														message2 << dataPoint->getDescription() << ": ";
														//  [3/14/2010 benxun]
														//message2 << "Valid range is from ";
														message2 << "有效范围从 ";														
														//message2 << dataPoint->getLimit(true) << " to ";
														message2 << dataPoint->getLimit(true) << " 到 ";
														//  [3/14/2010 benxun]
														//message2 << dataPoint->getLimit(false) << " ";

														DWORD Limit = dataPoint->getLimit(false);//jiangshengguang++ 4.29497e+009 to 4294967295
														message2 << Limit << " ";

														message2 << dataPoint->getUnit() << ".";													
														std::string* statusLineStr2 = new std::string(message2.str());
														AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr2), 0);
									
														break;
													}
												}
											}

											// if the value entered by the operator is an alpha-numerical string
											else
											{
												// show text in Status box
												//  [3/14/2010 benxun]
												//statusLine << "Override Failed: Value " << "\"" << overrideValue << "\" is not a valid number.";
												statusLine << "超驰失败失败，原因是值 " << "\"" << overrideValue << "\" 不是一个有效数字.";
												//  [3/14/2010 benxun]
												std::string* statusLineStr = new std::string(statusLine.str());
												AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
												
												break;
											}
										}

										//
										// override the data point now
										//
										
										// TD 4607: replaced 2 calls "setOverrideEnableStatus" and "setValue" with
										//			1 call "setManuallyOverrideValue"
										std::string sessionId ( TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID) );
										dataPoint->setManuallyOverrideValue ( overrideValue, sessionId );

										// show text in Status box
										//  [3/14/2010 benxun]
										//statusLine << "Override \"" << overrideValue << "\" Successful.";
										statusLine << "超驰 \"" << overrideValue << "\" 成功.";
										//  [3/14/2010 benxun]
										std::string* statusLineStr = new std::string(statusLine.str());
										AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

										// set the flag to indicate that screen animations need to be updated
										cellValueUpdated = true;
									}
								}
							}

							// if the checkbox is unchecked
							else
							{
								// confirm operator access on this data point
								bool isReSetForcedPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
									dataPoint->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_RESET_FORCED_STATE);						

								// if the current operator profile does not have this access
								if (!isReSetForcedPermitted)
								{
									// show text in Status box
									//  [3/14/2010 benxun]
									//statusLine << "No Access.";
									statusLine << "无超驰权限.";
									//  [3/14/2010 benxun]
									break;
								}

								// else if the operator profile has the access
								else
								{
									// get confirmation from the operator
									if ( inhibitOverrideConfirm ( IDS_OVERRIDE, dataPoint, (strValue == "Yes") ) )
									{
										//
										// undo override of the data point now
										//

										std::string sessionId ( TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID) );
										dataPoint->setOverrideEnableStatus ( false, sessionId );

										// show text in Status box
										//  [3/14/2010 benxun]
										//statusLine << "Override OFF Successful.";
										statusLine << "超驰取消成功.";
										//  [3/14/2010 benxun]
										std::string* statusLineStr = new std::string(statusLine.str());
										AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

										// set the flag to indicate that screen animations need to be updated
										cellValueUpdated = true;
									}
								}
							}
						}

						break;
					}

					case OVERRIDE_VALUE_COLUMN:
					{
						if (true == dataPoint->isOverrideEnabled())
						{
							// confirm operator access on this data point
							bool isSetForcedPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
								dataPoint->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_SET_FORCED_STATE);

							// if the current operator profile does not have this access
							if (!isSetForcedPermitted)
							{
								//statusLine << "No Access.";
								statusLine << "无超驰权限.";
								break;
							}

							// else if the operator profile has the access
							else
							{
								std::string currentValue = dataPoint->getValueAsString();

								CString strCurrentValue = currentValue.c_str();
								if (strCurrentValue == value)
								{
									break;
								}
								// get confirmation from the operator
								CConfirmOverrideDialog dlg(NULL, iPoint, m_equipment, value);

								// if acknowledged to go ahead
								if (dlg.DoModal() == IDOK)
								{
									// get the value entered by the operator
									overrideValue = dlg.getOverrideValue();

									//
									// perform sanity checks
									//

										// if the value entered by the operator is empty 
										if ( overrideValue == "" )
										{
											// show text in Status box
											//  [3/14/2010 benxun]
											//statusLine << "Override Failed: entered value is empty.";
											statusLine << "超驰失败，原因是输入值为空.";
											//  [3/14/2010 benxun]
											std::string* statusLineStr = new std::string(statusLine.str());
											AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
											break;
										}

									// if this data point is an analogue, i.e not a boolean, not a text nor a derived,
									// do range checking
									if ( ( false == dataPoint->isBoolean() ) &&
										( false == dataPoint->isText() ) &&
										( false == dataPoint->isDerived() ) )
									{
										double doubleValue = 0;
										std::istringstream valueStream ( overrideValue );

											// if the value entered by the operator is purely a numerical string
											if ( valueStream >> doubleValue )
											{
												// if the eng unit is defined
												if ( "" != dataPoint->getUnit() )
												{
													// if the value entered by the operator is outside
													// the range
													if ( ( doubleValue < dataPoint->getLimit ( true ) ) ||	// low eng limit
														( doubleValue > dataPoint->getLimit ( false ) ) )	// high eng limit
													{
														// show text in Status box
														//  [3/14/2010 benxun]
														//statusLine << "Override Failed: entered value is not within range.";
														statusLine << "超驰失败，原因是输入值不在有效范围内.";
														//  [3/14/2010 benxun]
														std::string* statusLineStr = new std::string(statusLine.str());
														AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

														std::ostringstream message2;
														message2 << dataPoint->getDescription() << ": ";
														//  [3/14/2010 benxun]
														//message2 << "Valid range is from ";
														message2 << "有效范围是从 ";
														//message2 << dataPoint->getLimit(true) << " to ";
														message2 << dataPoint->getLimit(true) << " 到 ";
														//  [3/14/2010 benxun]
														//message2 << dataPoint->getLimit(false) << " ";

													DWORD Limit = dataPoint->getLimit(false);//jiangshengguang++ 4.29497e+009 to 4294967295
													message2 << Limit << " ";

													message2 << dataPoint->getUnit() << ".";													
													std::string* statusLineStr2 = new std::string(message2.str());
													AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr2), 0);

													break;
												}
											}
										}

											// if the value entered by the operator is an alpha-numerical string
											else
											{
												// show text in Status box
												//  [3/14/2010 benxun]
												//statusLine << "Override Failed: Value " << "\"" << overrideValue << "\" is not a valid number.";
												statusLine << "超驰失败，原因是值 " << "\"" << overrideValue << "\" 不是一个有效数字.";
												//  [3/14/2010 benxun]
												std::string* statusLineStr = new std::string(statusLine.str());
												AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

											break;
										}
									}

									//
									// override the data point now
									//

									// TD 4607: replaced 2 calls "setOverrideEnableStatus" and "setValue" with
									//			1 call "setManuallyOverrideValue"
									std::string sessionId ( TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID) );
									dataPoint->setManuallyOverrideValue ( overrideValue, sessionId );

										// show text in Status box
										//  [3/14/2010 benxun]
										//statusLine << "Override \"" << overrideValue << "\" Successful.";
										statusLine << "超驰 \"" << overrideValue << "\" 成功.";
										//  [3/14/2010 benxun]
										std::string* statusLineStr = new std::string(statusLine.str());
										AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);

									// set the flag to indicate that screen animations need to be updated
									cellValueUpdated = true;
								}
							}
						}

						break;
					}

					default:
					{
						break;
					}				
				}
				// end of switch statement
			}
		}
		catch ( TA_Base_Core::ScadaProxyException & e )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"UpdateCellValue() - %s", e.what());

			// show error message in Status box

			// if the checkbox is checked
			if (strValue == "Yes")
			{
				//  [3/14/2010 benxun]
				//statusLine << "Override \"" << overrideValue << "\" Failed.";
				statusLine << "超驰 \"" << overrideValue << "\" 失败.";
				//  [3/14/2010 benxun]
			}
			// if the checkbox is unchecked
			else
			{
				//  [3/14/2010 benxun]
				//statusLine << "Override OFF Failed.";
				statusLine << "超驰取消失败.";
				//  [3/14/2010 benxun]
			}
			std::string* statusLineStr = new std::string(statusLine.str());
			AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
		}
		catch (...)
		{
			std::string message = "Error (unknown) when overriding DataPoint \"";
			message += dataPoint->getDescription() + "\".";

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"UpdateCellValue() - %s", message.c_str());

			// show error message in Status box

			// if the checkbox is checked
			if (strValue == "Yes")
			{
				//  [3/14/2010 benxun]
				//statusLine << "Override \"" << overrideValue << "\" Failed.";
				statusLine << "超驰 \"" << overrideValue << "\" 失败.";
				//  [3/14/2010 benxun]
			}
			// if the checkbox is unchecked
			else
			{
				//  [3/14/2010 benxun]
				//statusLine << "Override OFF Failed.";
				statusLine << "超驰取消失败.";
				//  [3/14/2010 benxun]
			}
			std::string* statusLineStr = new std::string(statusLine.str());
			AfxGetApp()->m_pMainWnd->PostMessage(UPDATE_STATUS_WINDOW_MESSAGE, reinterpret_cast<UINT>(statusLineStr), 0);
		}

		// if the flag indicates that screen animation needs to be updated
		if ( true == cellValueUpdated )
		{
			// TD#3351 - force re-query of datapoint to get overriden value, in case override fails
			// TES #115 - force the 'Value' column to be refreshed
			displayItem ( item, REFRESH_ALL );
		}

		return cellValueUpdated;

		FUNCTION_EXIT;
	}

	void COverrideListCtrl::displayItem(int nItem, DWORD refreshType)
	{
		FUNCTION_ENTRY ("displayItem()");

		TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(GetItemData(nItem));

		std::string  value;

		if( (dataPoint->isConfigValid() == true) )
		{
			if( (dataPoint->isWriteable() == false) && (dataPoint->getDisplayOrder() >= 0) )
			{
				// If a read only datapoint, able to refresh all fields
				// Otherwise we only refresh items specified as argument
				refreshType |= REFRESH_ALL;
			}

			if (REFRESH_DESCRIPTION & refreshType)
			{
				// Description (Equipment Attributes)
				value = dataPoint->getDescription();
				setItemTextImage(nItem, DESCRIPTION_COLUMN, value, false);
			}

			if (REFRESH_TYPE & refreshType)
			{
				// Type of the data point
				value = dataPoint->getShortNameOfType();
				setItemTextImage(nItem, DATAPOINT_TYPE_COLUMN, value, false);
			}

			if (REFRESH_OVERRIDE & refreshType)
			{
				bool isChecked, isEnabled;
				// Override Enable
				try
				{
					isChecked = dataPoint->isOverrideEnabled();
					isEnabled = isCellEnabled(nItem, OVERRIDE_ENABLE_COLUMN);
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// Not always available
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"displayItem() - %s", e.what());

					isChecked = false;
					isEnabled = false;
				}

				showCheckIcon(nItem, OVERRIDE_ENABLE_COLUMN, isChecked, isEnabled);
			}

			std::string currentValue = getCurrentValueAsString(dataPoint);

			if (REFRESH_OVERRIDE_VALUE & refreshType)
			{
				try
				{
					// Override Value same as current value
					if (dataPoint->isOverrideEnabled())
					{
						// wenching++ (TD3647): Overrided value should follow the display format configured, 
						//                      remove the default display format

						SetItemText(nItem, OVERRIDE_VALUE_COLUMN, currentValue.c_str());
					}
					else
					{
						SetItemText(nItem, OVERRIDE_VALUE_COLUMN, "");
					}				
				}
				catch (TA_Base_Core::ScadaProxyException &e)
				{
					// Not always available
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"displayItem() - %s", e.what());

					SetItemText(nItem, OVERRIDE_VALUE_COLUMN, "");
				}	
			}

			// TES#1034: Always update the Current Value column 
			try
			{				
				// Status of data point (Current Value)
				if (dataPoint->isOverrideEnabled())
				{
					value = dataPoint->getFieldValue();
					value = value + " ";
					value = value + dataPoint->getUnit();

					setItemTextImage(nItem, CURRENT_VALUE_COLUMN, value, true);
				}
				else
				{
					// No override, may just display current value
					setItemTextImage(nItem, CURRENT_VALUE_COLUMN, currentValue, true);
				}
			} 
			catch (TA_Base_Core::ScadaProxyException &e)
			{
				// Not always available
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"displayItem() - %s", e.what());
				setItemTextImage(nItem, CURRENT_VALUE_COLUMN, TA_Base_Bus::QUALITY_BAD_VALUE_UNKNOWN_STRING, true);
			}
			catch(TA_Base_Core::TransactiveException &ex)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"displayItem() - %s", ex.what());
				setItemTextImage(nItem, CURRENT_VALUE_COLUMN, TA_Base_Bus::QUALITY_BAD_VALUE_UNKNOWN_STRING, true);
			}
			catch(...)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"displayItem() - catch unknown exception");
				setItemTextImage(nItem, CURRENT_VALUE_COLUMN, TA_Base_Bus::QUALITY_BAD_VALUE_UNKNOWN_STRING, true);
			}
		}


		FUNCTION_EXIT;
	}


	bool COverrideListCtrl::isCellEnabled (int nItem, int nSubItem)
	{
		FUNCTION_ENTRY ("isCellEnabled()");

		// Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)
		if (nItem<0 || !m_accessAllowed)
			return false;

		TA_Base_Bus::DataPointProxySmartPtr& dataPoint = m_equipment->getChildDataPoint(GetItemData(nItem));

		try
		{
			// only applicable to input data points
			if (dataPoint->isWriteable() == false)
			{
				switch (nSubItem)
				{
					case OVERRIDE_ENABLE_COLUMN:
					{
						bool isSetForcedPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
							dataPoint->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_SET_FORCED_STATE);

						bool isReSetForcedPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
							dataPoint->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_RESET_FORCED_STATE);						

						// if the operator profile has access to both actions
						if ( ( true == isSetForcedPermitted ) &&
							 ( true == isReSetForcedPermitted ) )
						{
							return true;
						}

						// if the operator profile has no access to both actions
						else if ( ( false == isSetForcedPermitted ) &&
								  ( false == isReSetForcedPermitted ) )
						{
							return false;
						}

						// if the operator profile has access to one of the actions
						else
						{
							// if the operator profile has access to override the field value
							// but not to reset the override value
							if ( ( true == isSetForcedPermitted ) &&
								 ( false == isReSetForcedPermitted ) )
							{
								// if the data point has not been overridden yet
								if ( false == dataPoint->isOverrideEnabled() )
								{
									// enable the cell so that operator can
									// override the field value
									return true;
								}
								// else if the data point has been overridden
								else
								{
									// disable the cell so that the operator cannot reset 
									// the override value
									return false;
								}
							}

							// if the operator profile has access to reset the override value
							// but not to override the field value
							if ( ( false == isSetForcedPermitted ) &&
								 ( true == isReSetForcedPermitted ) )
							{
								// if the data point has not been overridden yet
								if ( false == dataPoint->isOverrideEnabled() )
								{
									// disable the cell so that the operator cannot
									// override the field value
									return false;
								}
								// else if the data point has been overridden
								else
								{
									// enable the cell so that operator can reset the
									// override value
									return true;
								}
							}

							// if none of the above conditions
							else
							{
								return false;
							}
						}
					}

					case OVERRIDE_VALUE_COLUMN:
					{
						bool isSetForcedPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
							dataPoint->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_SET_FORCED_STATE);

						bool isReSetForcedPermitted = TA_Base_App::AccessControlHelper::getInstance()->IsActionPermitted(
							dataPoint->getEntityKey(), TA_Base_App::AccessControlHelper::DATA_POINT_RESET_FORCED_STATE);						

						// if the operator profile has access to both actions
						if ( ( true == isSetForcedPermitted ) &&
							( true == isReSetForcedPermitted ) )
						{
							return true;
						}

						// if the operator profile has no access to both actions
						else if ( ( false == isSetForcedPermitted ) &&
							( false == isReSetForcedPermitted ) )
						{
							return false;
						}

						// if the operator profile has access to one of the actions
						else
						{
							// if the operator profile has access to override the field value
							// but not to reset the override value
							if ( ( true == isSetForcedPermitted ) &&
								( false == isReSetForcedPermitted ) )
							{
								// if the data point has not been overridden yet
								if ( false == dataPoint->isOverrideEnabled() )
								{
									// enable the cell so that operator can
									// override the field value
									return true;
								}
								// else if the data point has been overridden
								else
								{
									// disable the cell so that the operator cannot reset 
									// the override value
									return false;
								}
							}

							// if the operator profile has access to reset the override value
							// but not to override the field value
							if ( ( false == isSetForcedPermitted ) &&
								( true == isReSetForcedPermitted ) )
							{
								// if the data point has not been overridden yet
								if ( false == dataPoint->isOverrideEnabled() )
								{
									// disable the cell so that the operator cannot
									// override the field value
									return false;
								}
								// else if the data point has been overridden
								else
								{
									// enable the cell so that operator can reset the
									// override value
									return true;
								}
							}

							// if none of the above conditions
							else
							{
								return false;
							}
						}
					}

					default:
					{
						return false;
					}						
				}
			}
		}
		catch (TA_Base_Core::ScadaProxyException &e)
		{
			// Not always available
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"isCellEnabled() - %s", e.what());
		}		

		return false;

		FUNCTION_EXIT;
	}

	bool COverrideListCtrl::isAnyDataPointsDisplayedOnScreen() const
	{
		return m_equipment->hasInformationPoint();
	}

	//TD15438, override the function to get a chance to adjust columns width.
	BOOL COverrideListCtrl::SetWindowPos( const CWnd* pWndInsertAfter, int x, int y, int cx, int cy, UINT nFlags )
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
				
				int columnWidth = GetColumnWidth(CURRENT_VALUE_COLUMN);
				int vsrollWidth = GetSystemMetrics(SM_CXVSCROLL);
				
				//the minimum width is 100
				if (columnWidth - vsrollWidth > 100)
				{
					SetColumnWidth(CURRENT_VALUE_COLUMN, columnWidth - vsrollWidth);
				}
			}
		}
		
		return isSucceeded;
		
	}
	

}
