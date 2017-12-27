/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/src/CombinedEventItem.cpp $
 * @author:  Bradley Cleaver
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2009/12/03 15:46:08 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * CombinedEventItem is an implementation of the DisplayItem abstract class. It contains the data for one row in the list, and 
 * defines the standard ordering operator (<) for list sorting.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>

#include "app/soe/soe_event_viewer/src/stdafx.h"
#include "app/soe/soe_event_viewer/src/EventViewer.h"
#include "app/soe/soe_event_viewer/src/CombinedEventItem.h"
#include "app/soe/soe_event_viewer/src/CombinedEventFilter.h"
#include "core/data_access_interface/src/AlarmData.h"
#include "core/data_access_interface/src/IAlarmData.h"
#include "core/utilities/src/RunParams.h"

// NOTE: This inclusion of the CombinedEventFilter is simply to gain access to the statically declared variables. While
// this does lead to a cross-dependency (as AuditEventFilter is dependant on this class), that is preferable to 
// hard coding string variables throughout the application
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DateFormat.h" // TD12474 

#include <iomanip>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace
{
    const std::string DEFAULT_VALUE("");
	// Alarm Comments identifiers
    const std::string COMMENT_IDENTIFIER("C");
	// Alarm DSS Plan identifiers
	const std::string AUTO_PLAN_NOT_RUN_ID("A");
	const std::string SEMI_AUTO_PLAN_NOT_RUN_ID("S");
	const std::string PLAN_RUN_ID("R");
	const std::string PLAN_POSTPONED_ID("X");
	// Alarm Avalanche Status identifiers
	const std::string AVL_HEAD_ID("H");
	const std::string AVL_CHILD_ID("F");
	// Alarm Mms Status identifiers
	const std::string MMS_AUTO_ID("A");
	const std::string MMS_SEMI_ID("S");
	const std::string MMS_JR_SENT_ID("R");
	const std::string MMS_FAILED_ID("D");
	const std::string MMS_INHIBIT_ID("I");
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
 
CombinedEventItem::CombinedEventItem( TA_Base_Core::SOEEvent* const eventData )
{
	// source time: 设备时间
	std::string strTime = CTime(eventData->getSourceTime().time).Format("%Y/%m/%d %H:%M:%S");
	m_createTime = eventData->getSourceTime();

	char strmillsec[4];

	sprintf(strmillsec, "%03d", eventData->getSourceTime().millitm);

	strTime=strTime + "." + strmillsec;

    m_columnData.insert(std::map<std::string,std::string>::value_type
		(CombinedEventFilter::COLUMN_SOURCE_DATETIME,
		strTime
		) );

	// transactive time: 接收时间
	strTime = CTime(eventData->getTransactiveTime().time).Format("%Y/%m/%d %H:%M:%S");
	 
    char strmillsecTra[4];
	sprintf(strmillsecTra, "%03d", eventData->getTransactiveTime().millitm);

	strTime=strTime + "." + strmillsecTra;

    m_columnData.insert(std::map<std::string,std::string>::value_type
		(CombinedEventFilter::COLUMN_TRANSACTIVE_TIME,
		strTime
		) );
	
	// entity
	std::stringstream strEntity;
	strEntity<< eventData->getEntityKey();

 

	// asset
	m_columnData.insert(std::map<std::string,std::string>::value_type(
		CombinedEventFilter::COLUMN_ASSET,
		eventData->getAssetName()
		));
	
	// display value
    m_columnData.insert(std::map<std::string,std::string>::value_type
		(CombinedEventFilter::COLUMN_DISPLAY_VALUE,
		eventData->getDisplayValue()
		) );


	// location name
    m_columnData.insert(std::map<std::string,std::string>::value_type
		(CombinedEventFilter::COLUMN_LOCATION_NAME,
		eventData->getLocationName()
		) );

	// location name
	m_columnData.insert(std::map<std::string,std::string>::value_type
		(CombinedEventFilter::COLUMN_DESCRIPTION,
		eventData->getDescription()
		) );

	//li zhongyan add
	this->m_rPKey = eventData->getPKey();
}


CombinedEventItem::~CombinedEventItem()
{
}


const std::string& CombinedEventItem::getColumnData(const std::string& columnName)
{
    // Try to find the specified column in the data available for this item
	std::map<std::string,std::string>::iterator iter = m_columnData.find(columnName);

    if (iter != m_columnData.end())
    {
         
        // If it is found, return the string stored there
        return iter->second;
    }
    else
    {
        // If not return an empty string
        return DEFAULT_VALUE;
    }
}

 
const std::string& CombinedEventItem::getCompleteColumnData( const std::string& columnName )
{
    // Try to find the specified column in the data available for this item
    std::map<std::string,std::string>::iterator iter = m_columnData.find(columnName);

    if (iter != m_columnData.end())
    {
        // If it is found, return the string stored there
        return iter->second;
    }
    else
    {
        // If not return an empty string
        return DEFAULT_VALUE;
    }
}



bool CombinedEventItem::operator<(DisplayItem& rhs)
{

    // These are both string values, so use std::string.compare(), and store the result, where
    // result > 0 --> this item is lexicographically greater than the rhs item
    // result = 0 --> this item is lexicographically equal to the rhs item
    // result < 0 --> this item is lexicographically less than the rhs item
    int sortValue = 0;
    if ( CombinedEventFilter::COLUMN_SOURCE_DATETIME == s_sortColumn )
    {
        CombinedEventItem* pRhs = dynamic_cast<CombinedEventItem*>(&rhs);
        sortValue = m_createTime.time - pRhs->m_createTime.time;
        if (0 == sortValue)
        {
            // If the sort value was 0, it means the two times were the same, so want to sort on the milliseconds.
            sortValue = m_createTime.millitm - pRhs->m_createTime.millitm;
        }
    }
	else
	{
        std::string lhsData(getColumnData(s_sortColumn));
        std::string rhsData(rhs.getColumnData(s_sortColumn));
		//std::transform(lhsData.begin(), lhsData.end(), lhsData.begin(),toupper);
		//std::transform(rhsData.begin(), rhsData.end(), rhsData.begin(),toupper);
		sortValue = lhsData.compare(rhsData);
	}

    // Now compare the sort values to determine what we're doing.
    if (s_sortAscending)
    {
        // If the sort method is ascending, a result less than 0 is true (i.e. lower item first)
        if (0 > sortValue)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        // If the sort method is decending, a result greater than 0 is true (i.e. lower item last).
        if (0 < sortValue)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

void CombinedEventItem::setrPKey(double rPKey)
{
	this->m_rPKey = rPKey;
}

double CombinedEventItem::getrPKey(){
	return m_rPKey;
}

std::string CombinedEventItem::getCreateTimeStr()
{
	return m_strCreateTime;
}
