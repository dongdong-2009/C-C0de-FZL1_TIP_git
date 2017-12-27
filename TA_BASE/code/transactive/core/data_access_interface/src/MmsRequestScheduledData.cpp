/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/src/MmsRequestScheduledData.cpp $
* @author:  Luo HuiRong
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2009/12/29 14:17:48 $
* Last modified by:  $Author: grace.koh $
* 
* It holds the data specific to a MmsRequestScheduledData
* entry in the database.
*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif // _MSC_VER


#include "MmsRequestScheduledData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{

	MmsRequestScheduledData::MmsRequestScheduledData(unsigned long key,
		ScheduledType scheduledType,
		unsigned long entityKey,         
		const std::string& equipmentID,
		const std::string& meterCode,
		const std::string& equipmentDescription,
		double dpValue,
		timeb planDate,             			
		timeb createDate ):
	     m_key(key),m_scheduledType(scheduledType) ,m_entityKey(entityKey),m_equipmentID(equipmentID.c_str()),
		  m_meterCode(meterCode.c_str()), m_equipmentDescription(equipmentDescription.c_str()),
		  m_dpValue(dpValue),m_planDate(planDate) ,m_createDate(createDate)
	{
		m_planDate.dstflag=-1;
		m_planDate.timezone=-1;

		m_createDate.dstflag=-1;
		m_createDate.timezone=-1;		
	}	 

	MmsRequestScheduledData::~MmsRequestScheduledData()
	{
		 
	}

    unsigned long MmsRequestScheduledData::getKey()
    {         
        return m_key;
    } 

	MmsRequestScheduledData::ScheduledType MmsRequestScheduledData::getScheduledType()
	{
		return m_scheduledType;
	}
 
	double MmsRequestScheduledData::getDpValue()
	{
		return m_dpValue;
	}

	unsigned long MmsRequestScheduledData::getEntityKey()
	{
		return m_entityKey;
	}
	 
	std::string MmsRequestScheduledData::getEquipmentID()
	{
		return m_equipmentID;
	}
 
	std::string MmsRequestScheduledData::getMeterCode()
	{
		return m_meterCode;
	}

	 
	std::string MmsRequestScheduledData::getEquipmentDescription()
	{
		return m_equipmentDescription;
	}
	 
	timeb MmsRequestScheduledData::getPlanDate()
	{
		return m_planDate;
	}

	timeb MmsRequestScheduledData::getCreateDate()
	{
		return m_createDate;
	}
  
} // closes TA_Base_Core

