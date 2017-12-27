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


#include "MmsRequestSchematicData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{


	MmsRequestSchematicData::MmsRequestSchematicData(unsigned long key,
		unsigned long entityKey,      
		const std::string& equipmentID,	     
		const std::string& requestor,
		const std::string& problemReport,
		const timeb targetDate):
	     m_key(key), m_entityKey(entityKey) , m_equipmentID(equipmentID.c_str()),
		 m_requestor(requestor.c_str()),
		  m_problemReport(problemReport.c_str()), m_targetDate(targetDate) 
	{
 		m_targetDate.dstflag=-1;
		m_targetDate.timezone=-1;		
	}

	 

	MmsRequestSchematicData::~MmsRequestSchematicData()
	{
		 
	}

    unsigned long MmsRequestSchematicData::getKey()
    {         
        return m_key;
    } 
 
	unsigned long  MmsRequestSchematicData::getEntityKey()
	{
		return m_entityKey;
	}
	 
	 
	 
	std::string MmsRequestSchematicData::getEquipmentID()
	{
		return m_equipmentID;
	}
 
	 

	 
	std::string MmsRequestSchematicData::getRequestor()
	{
		return m_requestor;
	}
	 
	std::string MmsRequestSchematicData::getProblemReport()
	{
		return m_problemReport;
	}

	 
	timeb MmsRequestSchematicData::getTargetDate()
	{
		return m_targetDate;
	}

  
} // closes TA_Base_Core

