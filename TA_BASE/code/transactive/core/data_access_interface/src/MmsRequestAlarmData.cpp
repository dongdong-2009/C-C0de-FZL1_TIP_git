/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/MmsRequestAlarmData.cpp $
* @author:  Luo HuiRong
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2009/12/29 14:17:48 $
* Last modified by:  $Author: grace.koh $
* 
* It holds the data specific to a MmsRequestAlarmData
* entry in the database.
*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif // _MSC_VER


#include "MmsRequestAlarmData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{

	MmsRequestAlarmData::MmsRequestAlarmData( unsigned long key,
		const std::string& alarmID,
		const timeb alarmDate, 
		unsigned long alarmSeverity,
		const std::string& equipmentID,
		const std::string& alarmFaultCode,
		const std::string& alarmDescription,
		const std::string& requestor,
		const std::string& problemReport,
		const timeb targetDate,             			
		bool isAutoTrigger ):
	     m_key(key), m_alarmID(alarmID.c_str()),m_alarmDate(alarmDate), m_alarmSeverity(alarmSeverity), m_equipmentID(equipmentID.c_str()),
		  m_alarmFaultCode(alarmFaultCode.c_str()), m_alarmDescription(alarmDescription.c_str()), m_requestor(requestor.c_str()),
		  m_problemReport(problemReport.c_str()), m_targetDate(targetDate), m_isAutoTrigger(isAutoTrigger)
	{
		m_alarmDate.dstflag=-1;
		m_alarmDate.timezone=-1;

		m_targetDate.dstflag=-1;
		m_targetDate.timezone=-1;

		
	}

	 

	MmsRequestAlarmData::~MmsRequestAlarmData()
	{
		 
	}

    unsigned long MmsRequestAlarmData::getKey()
    {         
        return m_key;
    } 
 
	std::string MmsRequestAlarmData::getAlarmID()
	{
		return m_alarmID;
	}
	 
	timeb MmsRequestAlarmData::getAlarmDate()
	{
		return m_alarmDate;
	}
	 
	unsigned long MmsRequestAlarmData::getAlarmSeverity()
	{
		return m_alarmSeverity;
	}

	 
	std::string MmsRequestAlarmData::getAlarmEquipmentID()
	{
		return m_equipmentID;
	}
 
	std::string MmsRequestAlarmData::getAlarmFaultCode()
	{
		return m_alarmFaultCode;
	}

	 
	std::string MmsRequestAlarmData::getAlarmDescription()
	{
		return m_alarmDescription;
	}

	 
	std::string MmsRequestAlarmData::getRequestor()
	{
		return m_requestor;
	}
	 
	std::string MmsRequestAlarmData::getProblemReport()
	{
		return m_problemReport;
	}

	 
	timeb MmsRequestAlarmData::getTargetDate()
	{
		return m_targetDate;
	}

 
	bool  MmsRequestAlarmData::getIsAutoTrigger()
	{
		return m_isAutoTrigger;
	}
} // closes TA_Base_Core

