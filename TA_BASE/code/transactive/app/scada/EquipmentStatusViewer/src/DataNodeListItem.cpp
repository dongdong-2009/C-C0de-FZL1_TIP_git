/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/DataNodeListItem.cpp $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * A DataNode (Equipment) item to be displayed in (or filtered from) the EquipmentStatusListCtrl.
  * This class contains the required datanode proxy.
  *
  */

#include "stdafx.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/TAAssert.h"
#include "Equipmentstatusviewer.h"
#include "DataNodeListItem.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"

#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"

//TD16889 Mintao 
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/DateFormat.h" 
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace TA_Base_App
{
    
    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    DataNodeListItem::DataNodeListItem(unsigned long entitykey, HWND hwndToNotify)
        : EquipmentStatusListItem(entitykey, hwndToNotify)
    {
        //  This may throw a ScadaProxyException so be prepared to catch it.
        //  If we can't createDataNodeProxy() then try reallocating this DataNodeListItem later.
        TA_Base_Bus::ScadaProxyFactory::getInstance().createDataNodeProxy ( entitykey, *this, m_dataNode, false);

    }

    DataNodeListItem::DataNodeListItem(boost::shared_ptr<TA_Base_Core::DataNodeEntityData> entityData , HWND hwndToNotify)
                : EquipmentStatusListItem(entityData->getKey(), hwndToNotify)
    {
        //  This may throw a ScadaProxyException so be prepared to catch it.
        //  If we can't createDataNodeProxy() then try reallocating this DataNodeListItem later.
        TA_Base_Bus::ScadaProxyFactory::getInstance().createDataNodeProxy ( entityData, *this, m_dataNode, false);
    }

    DataNodeListItem::~DataNodeListItem()
    {
    }


    CString DataNodeListItem::buildNewColumnText( TA_Base_App::EslColumns column )
    {
        //  Get the new text for the requested column. By calling this function 
        //  for each column we need only one try-catch(ScadaProxyException) block.

        CString newText("");
        try
        {
            switch (column)
            {
            case ASSET_COLUMN:
                {
                    //just set the asset name
                    newText = m_dataNode->getAssetName().c_str();
                    break;
                }
            case DESCRIPTION_COLUMN:
                {
                    //  DESCRIPTION default is blank
                    newText = m_dataNode->getLabel().c_str();
                    break;
                }
            case STATUS_COLUMN:
                {
                    newText= buildSummaryText();
                    break;
                }
            case MODIFIED_BY_COLUMN:
                {
                    if (!getColumnText(STATUS_COLUMN).IsEmpty() )
                    {                        
                        //TD16889 Mintao 
                        std::string sessionidstr = m_dataNode->getLastStatusChangeSessionId();
                        if (!sessionidstr.empty())
                        {
                            try
                            {
                                newText = 
                                    TA_Base_Core::OperatorAccessFactory::getInstance().getOperatorFromSession( 
                                    m_dataNode->getLastStatusChangeSessionId(), false)->getName().c_str();
                                TA_Base_Core::ISession* thesession = TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionidstr);
                        
                                if (thesession != NULL)
                                {
                                    TA_Base_Core::IProfile* theprofile = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(thesession->getProfileKey());
                                    if (theprofile != NULL)
                                    {
                                        CString profilename;
                                        profilename.Format(" / %s", theprofile->getName().c_str());
                                        newText += profilename;
                                    }
                                    delete theprofile;

                                    TA_Base_Core::IConsole* theconsole = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsole(thesession->getConsoleKey());
                                    if (theconsole != NULL) 
                                    {
                                        CString locationname;
                                        locationname.Format(" / %s", theconsole->getLocationName().c_str());
                                        newText += locationname;
                                    }
                                    delete theconsole;
                                }
                                delete thesession;
                            }
                            catch (TA_Base_Core::DataException& ) 
                            {
                                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                                            "Failed to get session or profile data from database");                                
                            }
                        }
                    }
                    break;
                }
            case TIMESTAMP_COLUMN:
                {
                    if (!getColumnText(STATUS_COLUMN).IsEmpty() )
                    {                        
                        m_timeStamp = m_dataNode->getLastStatusChangeTimestamp();
                        if (0L != m_timeStamp)
                        {
                            CTime displayTime( m_timeStamp);
		                    TA_Base_Core::DateFormat dateFormat; 
		                    CString formatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat4).c_str(); 
                            newText = displayTime.Format(formatStr.operator LPCTSTR());
                        }
                    }
                    break;
                }

            case DATAPOINT_COLUMN:
            case TYPE_COLUMN:
            case VALUE_COLUMN:
            case QUALITY_COLUMN:
                {
                    //  Datanode items do not have values in these columns
                    break;
                }

            default:
                {
                    //  Requested column uses a field shared between DataNodes and DataPoints
                    newText = EquipmentStatusListItem::buildNewColumnText( column);
                }
            }
        }
        catch (TA_Base_Core::ScadaProxyException& )
        {
            //  The proxy data for this column is not ready yet
        }
        return newText;
    }


    CString DataNodeListItem::buildSummaryText() 
    {
		int numSummaries = 0;
		CString summaryTxt("");

        //firstly, check the tag state
		try
		{
            TA_Base_Bus::ETagState tagState = m_dataNode->getTaggingState();
            if (TA_Base_Bus::NotTagged != tagState)
            {
                summaryTxt = ( TA_Base_Bus::Tagged == tagState)?FilterString[TAGGED_FILTERSTRING]:FilterString[TAGGED_PTW_FILTERSTRING];
                summaryTxt += FilterString[STATUS_SEPARATOR_FILTERSTRING];
            }
        }
		catch (...)
		{
		}

		try
		{
			if ( m_dataNode->getControlInhibitSummary().value)
			{
				summaryTxt +=  FilterString[CONTROL_FILTERSTRING]; //  "Control";
				numSummaries++;
			};
		}
		catch (...)
		{
		}

		try
		{
			if (m_dataNode->getScanInhibitSummary().value)
			{
				if (numSummaries == 0)
					summaryTxt += FilterString[SCAN_FILTERSTRING]; //  "Scan"
				else
					summaryTxt += FilterString[SEPARATOR_FILTERSTRING] + FilterString[SCAN_FILTERSTRING]; //  ", Scan"
				numSummaries++;
			}
		}
		catch (...)
		{
		}

		try
		{
			if (m_dataNode->getAlarmInhibitSummary().value)
			{
				if (numSummaries == 0)
					summaryTxt += FilterString[ALARM_FILTERSTRING]; //  "Alarm";
				else
					summaryTxt += FilterString[SEPARATOR_FILTERSTRING] + FilterString[ALARM_FILTERSTRING]; // ", Alarm";
				numSummaries++;
			}
		}
		catch (...)
		{
		}

		try
		{
			if (m_dataNode->getMmsAlarmInhibitSummary().value)
			{
				if (numSummaries == 0)
					summaryTxt += FilterString[MMS_FILTERSTRING]; //  "MMS";
				else
					summaryTxt += FilterString[SEPARATOR_FILTERSTRING] + FilterString[MMS_FILTERSTRING]; //  ", MMS";
				numSummaries++;
			}
		}
		catch (...)
		{
		}

		if (numSummaries != 0)
			summaryTxt += FilterString[INHIBITED_FILTERSTRING] + FilterString[SUMMARY_FILTERSTRING]; //  " Inhibited(Summary)";

		try
		{
			if (m_dataNode->getOverrideSummary().value)
			{
		        if (numSummaries == 0)
			        summaryTxt += FilterString[OVERRIDDEN_FILTERSTRING] + FilterString[SUMMARY_FILTERSTRING]; //  "Overridden(Summary)"
		        else
			        summaryTxt += FilterString[STATUS_SEPARATOR_FILTERSTRING] + FilterString[OVERRIDDEN_FILTERSTRING] + FilterString[SUMMARY_FILTERSTRING]; //  ", Overridden(Summary)";
			}
		}
		catch (...)
		{
		}

		return summaryTxt;
	}

    void DataNodeListItem::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
    {
        TA_Base_Core::ThreadGuard guard( m_lock );

        //  Perform DataNode-specific updates then call the base class to post to UI thread
        try
        {
            if (m_dataNode->getChildDataPointEntities().size() == 0)
            {
                //  This DataNode has children so it should not be included in the report
                m_updateType = DELETE_ITEM;
            }
        }
        catch (TA_Base_Core::ScadaProxyException& )
        {
            //  Not ready yet
        }
        EquipmentStatusListItem::processEntityUpdateEvent( entityKey, updateType);

    }


    void DataNodeListItem::CheckFilters(FilterFlagsAndStrings const& filters)
    {
        if ( true == filters[REPORT_TYPE_FILTER_OFFSET].filterInUse )
        {
            if ( ! ( (REPORT_TYPE_EQUIPMENT == filters[REPORT_TYPE_FILTER_OFFSET].filterString) ||
                     (REPORT_TYPE_ALL       == filters[REPORT_TYPE_FILTER_OFFSET].filterString) ) )
            {
                m_isFiltered = true;
                return;
            }
        }

        EquipmentStatusListItem::CheckFilters(filters);
    }

    unsigned long DataNodeListItem::getItemDataNodeEntityKey()
    {
        return m_dataNode->getEntityKey();
    }

    std::string DataNodeListItem::getItemDataNodeEntityName()
    {
        return m_dataNode->getEntityName();
    }


} // TA_Base_App

