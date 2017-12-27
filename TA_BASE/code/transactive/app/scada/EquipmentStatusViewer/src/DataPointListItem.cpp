/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/DataPointListItem.cpp $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * A DataPoint item to be displayed in (or filtered from) the EquipmentStatusListCtrl.
  * This class contains the required datapoint proxy.
  *
  */

#include "stdafx.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/TAAssert.h"
#include "equipmentstatusviewer.h"
#include "DataPointListItem.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"

#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
//TD16889 Mintao 
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/DateFormat.h" 
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"

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

    DataPointListItem::DataPointListItem(unsigned long entitykey, HWND hwndToNotify)
        : EquipmentStatusListItem(entitykey, hwndToNotify), m_dataNode(NULL)
    {
        TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy ( entitykey, *this, m_dataPoint);

        loadParentDataNode();
    }

    DataPointListItem::~DataPointListItem()
    {
        if (m_dataNode != NULL)
        {
            delete m_dataNode;
            m_dataNode = NULL;
        }
    }

    void DataPointListItem::loadParentDataNode()
    {
        try
        {
            TA_Base_Core::IEntityData* data = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_dataPoint->getParentKey());
             m_dataNode = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(data);
             TA_ASSERT(m_dataNode != NULL, "Failed to read the datanode information for the datapoint");            
        }
        catch (...)
        {
             TA_ASSERT(false, "Failed to create the datanode for the datapoint");            
        }
    }


    CString DataPointListItem::buildStatusText()
    {
		int numInhibits = 0;
		CString statusTxt("");

        try
		{
			if ( m_dataPoint->isScanInhibited())
			{
				statusTxt = FilterString[SCAN_FILTERSTRING]; //  "Scan"
				numInhibits++;
			}
		}
		catch (...)
		{
		}

		try
		{
			if (m_dataPoint->isAlarmInhibited())
			{
				if (numInhibits == 0)
					statusTxt = FilterString[ALARM_FILTERSTRING]; //  "Alarm";
				else
					statusTxt += FilterString[SEPARATOR_FILTERSTRING] + FilterString[ALARM_FILTERSTRING]; //  ", Alarm";
				numInhibits++;
			}
		}
		catch (...)
		{
		}

		try
		{
			if (m_dataPoint->isMmsAlarmInhibited())
			{
				if (numInhibits == 0)
					statusTxt = FilterString[MMS_FILTERSTRING]; //  "MMS";
				else
					statusTxt += FilterString[SEPARATOR_FILTERSTRING] + FilterString[MMS_FILTERSTRING]; //  ", MMS";
				numInhibits++;
			}
		}
		catch (...)
		{
		}

		try
		{
			// if the data point is an output
			if ( true == m_dataPoint->isWriteable() )
			{
				// check if its control is inhibited
				if (m_dataPoint->isControlInhibited())
				{
					statusTxt = FilterString[CONTROL_FILTERSTRING]; //  "Control";
					numInhibits++;
				};
			}
		}
		catch (const TA_Base_Core::ScadaProxyException&)
		{
		}
		catch (...)
		{
		}

		if (numInhibits == 0)
		{
			statusTxt = "";
		}
		else
		{
			statusTxt += FilterString[INHIBITED_FILTERSTRING]; //  " Inhibited";
		}

		try
		{
			if (m_dataPoint->isOverrideEnabled())
			{
			    if (numInhibits == 0)
				    statusTxt = FilterString[OVERRIDDEN_FILTERSTRING]; //  "Overridden";
			    else
				    statusTxt += FilterString[SEPARATOR_FILTERSTRING] + FilterString[OVERRIDDEN_FILTERSTRING]; //  ", Overridden";
			}
		}
		catch (...)
		{
		}

		return statusTxt;
    }


    void DataPointListItem::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
    {
        TA_Base_Core::ThreadGuard guard( m_lock );
        EquipmentStatusListItem::processEntityUpdateEvent(getEntityKey(), updateType);
    }


    CString DataPointListItem::buildNewColumnText( TA_Base_App::EslColumns column )
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
                    //  Each subsequent line may throw a ScadaProxyException, so build a default first
                    newText = m_dataNode->getAssetName().c_str();
                    break;
                }
            case DESCRIPTION_COLUMN:
                {
                    //  DESCRIPTION default is blank
                    newText = m_dataNode->getLabel().c_str();
                    break;
                }
            case DATAPOINT_COLUMN:
                {
                    newText="...";  //  DATAPOINT default if the next line throws an exception
                    newText = m_dataPoint->getDescription().c_str();
                    break;
                }
            case TYPE_COLUMN:
                {
                    newText="...";   //  TYPE default
                    newText = m_dataPoint->getShortNameOfType().c_str();
                    break;
                }
            case VALUE_COLUMN:
                {
                    //  VALUE default is blank
                    newText = m_dataPoint->getValueAsString().c_str();
                    break;
                }
            case QUALITY_COLUMN:
                {
                    newText = m_dataPoint->getQualityAsString();
                    break;
                }
            case STATUS_COLUMN:
                {
                    newText = buildStatusText();
                    break;
                }
            case MODIFIED_BY_COLUMN:
                {
                    if (!getColumnText(STATUS_COLUMN).IsEmpty() )
                    {
                        //TD16889 Mintao 
                        std::string sessionidstr = m_dataPoint->getLastStatusChangeSessionId();
                        if (!sessionidstr.empty())
                        {
                            try
                            {
                                newText = 
                                    TA_Base_Core::OperatorAccessFactory::getInstance().getOperatorFromSession( 
                                    m_dataPoint->getLastStatusChangeSessionId(), false)->getName().c_str();
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
                        m_timeStamp = m_dataPoint->getLastStatusChangeTimestamp();
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
            default:
                {
                    //  Requested column probably uses a field shared between DataNodes and DataPoints
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


    int DataPointListItem::buildNewColumnIcon( TA_Base_App::EslColumns column )
    {
        //  Currently the only icon displayed in the EquipmentStatusViewer is 
        //  the "quality" icon displayed in the DataPoints Value column.

        if ( VALUE_COLUMN != column)
        {
            return EquipmentStatusListItem::buildNewColumnIcon( column);
        }

		try
		{
			if ( m_dataPoint->isInAlarmState())
			{
				return ESL_RED;
			}
		}
		catch (TA_Base_Core::ScadaProxyException& )
		{
			// Alarm state not ready
		}

		try
		{
			switch (m_dataPoint->getStatus() & TA_Base_Bus::QUALITY_MASK)
			{
			case TA_Base_Bus::QUALITY_GOOD:
				if (m_dataPoint->getStatus() == TA_Base_Bus::QUALITY_GOOD_LOCAL_OVERRIDE)
				{
					return ESL_ORANGE;
				}
				if (m_dataPoint->getStatus() == TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON)
				{
					return ESL_GREEN;
				}

				return -1;

			case TA_Base_Bus::QUALITY_UNCERTAIN:
			case TA_Base_Bus::QUALITY_BAD:
				return ESL_BLUE;

			default:
				return -1;

			}
		}
		catch (TA_Base_Core::ScadaProxyException& )
		{
            //  Datapoint status not ready
			return ESL_BLUE;
		}
    }


    void DataPointListItem::CheckFilters( FilterFlagsAndStrings const& filters)
    {
        if ( true == filters[REPORT_TYPE_FILTER_OFFSET].filterInUse )
        {
            if ( ! ( (REPORT_TYPE_DATAPOINT == filters[REPORT_TYPE_FILTER_OFFSET].filterString) ||
                     (REPORT_TYPE_ALL       == filters[REPORT_TYPE_FILTER_OFFSET].filterString) ) )
            {
                m_isFiltered = true;
                return;
            }
        }

        EquipmentStatusListItem::CheckFilters(filters);
    }

    unsigned long DataPointListItem::getItemDataNodeEntityKey()
    {
        return m_dataNode->getKey();
        
    }

    std::string DataPointListItem::getItemDataNodeEntityName()
    {
        return m_dataNode->getName();
    }

    
} // TA_Base_App

