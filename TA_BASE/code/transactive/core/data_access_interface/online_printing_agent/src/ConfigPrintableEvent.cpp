/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/online_printing_agent/src/ConfigPrintableEvent.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Implements the IConfigScheduleData interface to allow manipulation
  * of a row of schedule data. Note that schedule data is stored in the
  * Job table, but is accessed using this separate class for convenience.
  *
  */


#include "core/data_access_interface/online_printing_agent/src/ConfigPrintableEvent.h"
#include "core/data_access_interface/online_printing_agent/src/PrintableEventHelper.h"
#include "core/data_access_interface/online_printing_agent/src/PrintableEventData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IAlarmTypeData.h"
#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/configuration_updates/src/EObjectType.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


#include <string>


namespace TA_Base_Core
{

	ConfigPrintableEvent::ConfigPrintableEvent(unsigned long pkey,std::string printer,
                              std::string alarmflag, std::string createtime,
                              std::string severity, std::string value,
                              std::string asset,std::string description ,
                              std::string mms,std::string dss,
                              std::string avalanche,std::string operatorname,
                              std::string alarmcomment,std::string alm,
                              unsigned long locationkey)
                              :m_helper(0)
	{
 		m_helper = new PrintableEventHelper( pkey, printer, alarmflag, createtime, severity,
							                 asset, description, value, mms, dss, avalanche,
											 operatorname, alarmcomment, alm, locationkey);
	}



	ConfigPrintableEvent::~ConfigPrintableEvent() 
	{
		FUNCTION_ENTRY("~ConfigPrintableEvent");

		if(0 != m_helper)
		{
			delete m_helper;
		}

		FUNCTION_EXIT;
	};




	void ConfigPrintableEvent::applyChanges()
	{
		FUNCTION_ENTRY("applyChanges");

		m_helper->applyChanges();

		FUNCTION_EXIT;
	}



	void ConfigPrintableEvent::invalidate()
	{

		m_helper->invalidate();
	}



	bool ConfigPrintableEvent::hasChanged()
	{
		FUNCTION_ENTRY("hasChanged");
		FUNCTION_EXIT;
		return m_helper->hasChanged();
	}


	void ConfigPrintableEvent::updateChanges(const std::string& name, unsigned long oldKey, 
		unsigned long newKey)
	{
		FUNCTION_ENTRY("updateChanges");

		//todo

		FUNCTION_EXIT;
	}


	unsigned long ConfigPrintableEvent::getPkey()
    {
        FUNCTION_ENTRY("getPkey");

		FUNCTION_EXIT;

		return m_helper->getPkey();

        
    }
	
	std::string ConfigPrintableEvent::getPrinter()
    {
        FUNCTION_ENTRY("getPrinter");

		FUNCTION_EXIT;
		
        return m_helper->getPrinter();

        
    }
	
	std::string ConfigPrintableEvent::getAlarmflag()
    {
        FUNCTION_ENTRY("getAlarmflag");

		FUNCTION_EXIT;
		
        return m_helper->getAlarmflag();  
        
    }
	
	std::string ConfigPrintableEvent::getCreatetime()
    {
        FUNCTION_ENTRY("getCreatetime");
		
		FUNCTION_EXIT;

        return m_helper->getCreatetime();        
		
		
    }
	
	std::string ConfigPrintableEvent::getSeverity()
    {
        FUNCTION_ENTRY("getSeverity");

		 FUNCTION_EXIT;
		
        return m_helper->getSeverity();       
		
		
    }
	
	std::string ConfigPrintableEvent::getAsset()
    {
        FUNCTION_ENTRY("getAsset");

		 FUNCTION_EXIT;
		
         return m_helper->getAsset();      
		
		
    }
	
	
	std::string ConfigPrintableEvent::getDescription()
    {
        FUNCTION_ENTRY("getDescription");

		FUNCTION_EXIT;
		
         return m_helper->getDescription();        
		
		
    }
	
	
	std::string ConfigPrintableEvent::getValue()
    {
        FUNCTION_ENTRY("getValue");

		FUNCTION_EXIT;
		
        return m_helper->getValue();     
		
		
    }
	
	
	std::string ConfigPrintableEvent::getAvalanche()
    {
        FUNCTION_ENTRY("getAvalanche");

		FUNCTION_EXIT;
		
        return m_helper->getAvalanche();
        
       
		
    }
	
	
	std::string ConfigPrintableEvent::getMMS()
    {
        FUNCTION_ENTRY("getMMS");

		FUNCTION_EXIT;
		
        return m_helper->getMMS();       
		
    }
	
	
	std::string ConfigPrintableEvent::getDSS()
    {
        FUNCTION_ENTRY("getDSS");

		FUNCTION_EXIT;

		return m_helper->getDSS();       
 
		
    }
	
	
	std::string ConfigPrintableEvent::getOperatorname()
    {
        FUNCTION_ENTRY("getOperatorname");

		FUNCTION_EXIT;
		
        return m_helper->getOperatorname();		
		
    }

	std::string ConfigPrintableEvent::getAlarm_Comment()
    {
        FUNCTION_ENTRY("getAlarm_Comment");

		FUNCTION_EXIT;
		
        return m_helper->getAlarm_Comment();    
 
		
		
    }


	std::string ConfigPrintableEvent::getAlm()
    {
        FUNCTION_ENTRY("getAlm");

		FUNCTION_EXIT;
		
        return m_helper->getAlm();		        
       
    }


	unsigned long ConfigPrintableEvent::getLocation_Key()
    {
        FUNCTION_ENTRY("getLocation_Key");

		FUNCTION_EXIT;
		
        return m_helper->getLocation_Key();        
	
    }


    void ConfigPrintableEvent::setPrinter( std::string printer)
	{
        m_helper->setPrinter(printer);
    }

    void ConfigPrintableEvent::setAlarmflag( std::string alarmflag)
	{
		m_helper->setAlarmflag(alarmflag);
	}

    void ConfigPrintableEvent::setCreatetime(  std::string createtime )
	{
        m_helper->setCreatetime(createtime);

	}

    void ConfigPrintableEvent::setSeverity( std::string severity)
	{
        m_helper->setSeverity(severity);

	}

    void ConfigPrintableEvent::setValue( std::string value)
	{
        m_helper->setValue(value);

	}

    void ConfigPrintableEvent::setAsset( std::string asset)
	{
        m_helper->setAsset(asset);

	}
    void ConfigPrintableEvent::setDescription( std::string description )
	{
        m_helper->setDescription(description);
 
	}

    void ConfigPrintableEvent::setMMS( std::string mms)
	{
        m_helper->setMMS(mms);

	}

    void ConfigPrintableEvent::setDSS( std::string dss)
	{
        m_helper->setDSS(dss);

	}
    void ConfigPrintableEvent::setAvalanche( std::string avalanche)
	{
        m_helper->setAvalanche(avalanche);

	}

    void ConfigPrintableEvent::setOperatorname( std::string operatorname)
	{
        m_helper->setOperatorname(operatorname);

	}

    void ConfigPrintableEvent::setAlarm_Comment( std::string alarmcomment)
	{
        m_helper->setAlarm_Comment(alarmcomment);

	}

    void ConfigPrintableEvent::setAlm( std::string alm)
	{
        m_helper->setAlm(alm);

	}

    void ConfigPrintableEvent::setLocation_Key( unsigned long locationkey)
	{
        m_helper->setLocation_Key(locationkey);

	}



};// TA_Base_Core

