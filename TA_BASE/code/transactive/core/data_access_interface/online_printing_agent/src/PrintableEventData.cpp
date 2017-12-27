/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/online_printing_agent/src/PrintableEventData.cpp $
 * @author:  Cameron Rochester
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * MmSchedulingData is an implementation of IMmSchedulingData. It holds the data specific to 
 * alarm plan associations.
 */



#include <sstream>
#include <string>

#include "core/data_access_interface/online_printing_agent/src/PrintableEventData.h"
#include "core/data_access_interface/online_printing_agent/src/PrintableEventHelper.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{


	PrintableEventData::PrintableEventData( unsigned long pkey,std::string printer,
                            std::string alarmflag, std::string createtime,
                            std::string severity, std::string value,
                            std::string asset,std::string description ,
                            std::string mms,std::string dss,
                            std::string avalanche,std::string operatorname,
                            std::string alarmcomment,std::string alm,
                            unsigned long locationkey)
							:m_helper(0)
	{
		FUNCTION_ENTRY("PrintableEventData");

		m_helper = new PrintableEventHelper( pkey, printer, alarmflag, createtime, severity,
			                                 value, asset, description, mms, dss, avalanche, 
											 operatorname, alarmcomment, alm, locationkey
										   );

		FUNCTION_EXIT;
	}


	PrintableEventData::~PrintableEventData()
	{
		FUNCTION_ENTRY("~PrintableEventData");

		if(0 != m_helper)
		{
			delete m_helper;
		}

		FUNCTION_EXIT;
	}
	


	unsigned long PrintableEventData::getPkey()
    {
        FUNCTION_ENTRY("getPkey");
		FUNCTION_EXIT;
		return m_helper->getPkey();

      
    }
	
	std::string PrintableEventData::getPrinter()
    {
        FUNCTION_ENTRY("getPrinter");
		 FUNCTION_EXIT;
		
        return m_helper->getPrinter();
       
    }
	
	std::string PrintableEventData::getAlarmflag()
    {
        FUNCTION_ENTRY("getAlarmflag");
		FUNCTION_EXIT;
		
        return m_helper->getAlarmflag();
  
        
    }
	
	std::string PrintableEventData::getCreatetime()
    {
        FUNCTION_ENTRY("getCreatetime");
		FUNCTION_EXIT;
        return m_helper->getCreatetime();

        
		
		
    }
	
	std::string PrintableEventData::getSeverity()
    {
        FUNCTION_ENTRY("getSeverity");
		FUNCTION_EXIT;		
        return m_helper->getSeverity();      
		
		
    }
	
	std::string PrintableEventData::getAsset()
    {
        FUNCTION_ENTRY("getAsset");

		FUNCTION_EXIT;
		
         return m_helper->getAsset();        
		
		
    }
	
	
	std::string PrintableEventData::getDescription()
    {
        FUNCTION_ENTRY("getDescription");

		 FUNCTION_EXIT;
		
         return m_helper->getDescription();
       
		
		
    }
	
	
	std::string PrintableEventData::getValue()
    {
        FUNCTION_ENTRY("getValue");

		FUNCTION_EXIT;
		
        return m_helper->getValue();       
		
		
    }
	
	
	std::string PrintableEventData::getAvalanche()
    {
        FUNCTION_ENTRY("getAvalanche");

		  FUNCTION_EXIT;
		
        return m_helper->getAvalanche();      
       
		
    }
	
	
	std::string PrintableEventData::getMMS()
    {
        FUNCTION_ENTRY("getMMS");
		FUNCTION_EXIT;	
        return m_helper->getMMS();   
        
		
    }
	
	
	std::string PrintableEventData::getDSS()
    {
        FUNCTION_ENTRY("getDSS");

		FUNCTION_EXIT;


		return m_helper->getDSS();		      
 
		
    }
	
	
	std::string PrintableEventData::getOperatorname()
    {
        FUNCTION_ENTRY("getOperatorname");

		FUNCTION_EXIT;
		
        return m_helper->getOperatorname();        

		
		
    }

	std::string PrintableEventData::getAlarm_Comment()
    {
        FUNCTION_ENTRY("getAlarm_Comment");

		FUNCTION_EXIT;
		
        return m_helper->getAlarm_Comment();     
 
		
		
    }


	std::string PrintableEventData::getAlm()
    {
        FUNCTION_ENTRY("getAlm");

		FUNCTION_EXIT;
		
        return m_helper->getAlm();	
        
       
    }


	unsigned long PrintableEventData::getLocation_Key()
    {
        FUNCTION_ENTRY("getLocation_Key");
		
		FUNCTION_EXIT;


        return m_helper->getLocation_Key();        
        
		
		
    }




	void PrintableEventData::invalidate()
	{
		FUNCTION_ENTRY("invalidate");

		m_helper->invalidate();

		FUNCTION_EXIT;
	}


} // closes TA_IRS_Core


