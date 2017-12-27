/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/online_printing_agent/src/PrintableEventHelper.cpp $
* @author:  Karen Graham
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2015/01/19 17:43:23 $
* Last modified by:  $Author: CM $
* 
* MmSchedulingHelper is an object that is held by all type-specific alarm plan association objects.
* It contains all data common across alarm plan associations, and manipulation
* methods for the data. It helps avoid re-writing code for each object.
*/

#include <vector>
#include <string>

#include "core/data_access_interface/online_printing_agent/src/PrintableEventHelper.h"
#include "core/data_access_interface/online_printing_agent/src/PrintableEventAccessFactory.h"
#include "ace/OS.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"


#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"


using TA_Base_Core::DataException;
using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	
	
	
    PrintableEventHelper::PrintableEventHelper( 
		unsigned long pkey,std::string printer,
        std::string alarmflag, std::string createtime,
        std::string severity, std::string value,
        std::string asset,std::string description ,
        std::string mms,std::string dss,
        std::string avalanche,std::string operatorname,
        std::string alarmcomment,std::string alm,
        unsigned long locationkey)
		: m_isValidData(true)
		
    {
        m_printer       = printer;
        m_pkey          = pkey;
        m_alarmflag     = alarmflag;
        m_createtime    = createtime;
        m_severity      = severity;
        m_asset         = asset;
        m_description   = description;
        m_value         = value;
        m_mms           = mms;
        m_dss           = dss;
        m_avalanche     = avalanche;
		m_operatorname  = operatorname;
		m_alarmcomment  = alarmcomment;
		m_alm           = alm;
		m_locationkey   = locationkey;	
		
        m_printerOrig       = printer;
        m_pkeyOrig          = pkey;
        m_alarmflagOrig     = alarmflag;
        m_createtimeOrig    = createtime;
        m_severityOrig      = severity;
        m_assetOrig         = asset;
        m_descriptionOrig   = description;
        m_valueOrig         = value;
        m_mmsOrig           = mms;
        m_dssOrig           = dss;
        m_avalancheOrig     = avalanche;
		m_operatornameOrig  = operatorname;
		m_alarmcommentOrig  = alarmcomment;
		m_almOrig           = alm;
		m_locationkeyOrig   = locationkey;	
    }
	
	
	
	
	PrintableEventHelper::PrintableEventHelper( const PrintableEventHelper& rhs)
		: m_isValidData(true)
    {
        m_printer       = rhs.m_printer;
        m_pkey          = rhs.m_pkey;
        m_alarmflag     = rhs.m_alarmflag;
        m_createtime    = rhs.m_createtime;
        m_severity      = rhs.m_severity;
        m_asset         = rhs.m_asset;
        m_description   = rhs.m_description;
        m_value         = rhs.m_value;
        m_mms           = rhs.m_mms;
        m_dss           = rhs.m_dss;
        m_avalanche     = rhs.m_avalanche;
		m_operatorname  = rhs.m_operatorname;
		m_alarmcomment  = rhs.m_alarmcomment;
		m_alm           = rhs.m_alm;
		m_locationkey   = rhs.m_locationkey;	
    }
	
	
    PrintableEventHelper::~PrintableEventHelper()
    {
    }
	
	
	
    void PrintableEventHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");
		
        m_isValidData = false;
		
        FUNCTION_EXIT;
    }
	
	unsigned long PrintableEventHelper::getPkey()
    {
        FUNCTION_ENTRY("getPkey");
		
        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_pkey;
    }
	
	std::string PrintableEventHelper::getPrinter()
    {
        FUNCTION_ENTRY("getPrinter");
		
        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_printer;
    }
	
	std::string PrintableEventHelper::getAlarmflag()
    {
        FUNCTION_ENTRY("getAlarmflag");
		
        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_alarmflag;
    }
	
	std::string PrintableEventHelper::getCreatetime()
    {
        FUNCTION_ENTRY("getCreatetime");
		
        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_createtime;
		
		
    }
	
	std::string PrintableEventHelper::getSeverity()
    {
        FUNCTION_ENTRY("getSeverity");
		
        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_severity;
		
		
    }
	
	std::string PrintableEventHelper::getAsset()
    {
        FUNCTION_ENTRY("getAsset");
		
        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_asset;
		
		
    }
	
	
	std::string PrintableEventHelper::getDescription()
    {
        FUNCTION_ENTRY("getDescription");
		
        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_description;
		
		
    }
	
	
	std::string PrintableEventHelper::getValue()
    {
        FUNCTION_ENTRY("getValue");
		
        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_value;
		
		
    }
	
	
	std::string PrintableEventHelper::getAvalanche()
    {
        FUNCTION_ENTRY("getAvalanche");
		
        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_avalanche;
		
		
    }
	
	
	std::string PrintableEventHelper::getMMS()
    {
        FUNCTION_ENTRY("getMMS");
		
        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_mms;
		
		
    }
	
	
	std::string PrintableEventHelper::getDSS()
    {
        FUNCTION_ENTRY("getDSS");
		
        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_dss;
		
		
    }
	
	
	std::string PrintableEventHelper::getOperatorname()
    {
        FUNCTION_ENTRY("getOperatorname");
		
        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_operatorname;
		
		
    }

	std::string PrintableEventHelper::getAlarm_Comment()
    {
        FUNCTION_ENTRY("getAlarm_Comment");
		
        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_alarmcomment;
		
		
    }


	std::string PrintableEventHelper::getAlm()
    {
        FUNCTION_ENTRY("getAlm");
		
        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_alm;
		
		
    }


	unsigned long PrintableEventHelper::getLocation_Key()
    {
        FUNCTION_ENTRY("getLocation_Key");
		
        if (!m_isValidData)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_locationkey;
		
		
    }


	
	
	
    void PrintableEventHelper::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");
		
        // note that the api does not provide a way to create new records, nor does it allow the
		//entry of invalid data. Hence, no validation is required
		
        modifyExisting();
		
        m_printerOrig       = m_printer;
        m_pkeyOrig          = m_pkey;
        m_alarmflagOrig     = m_alarmflag;
        m_createtimeOrig    = m_createtime;
        m_severityOrig      = m_severity;
        m_assetOrig         = m_asset;
        m_descriptionOrig   = m_description;
        m_valueOrig         = m_value;
        m_mmsOrig           = m_mms;
        m_dssOrig           = m_dss;
        m_avalancheOrig     = m_avalanche;
		m_operatornameOrig  = m_operatorname;
		m_alarmcommentOrig  = m_alarmcomment;
		m_almOrig           = m_alm;
		m_locationkeyOrig   = m_locationkey;	
		
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
		
        FUNCTION_EXIT;
    }
	
	
    void PrintableEventHelper::reload()
    {
        FUNCTION_ENTRY("reload");
		
        // get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Event_Ad, TA_Base_Core::Read);
		/*
        std::ostringstream sql;

        sql <<"select PKEY, PRINTER, ALARMFLAG, CREATETIME, ";
        sql <<" SEVERITY, ASSET, DESCRIPTION, ";
        sql <<" VALUE, MMS, DSS, AVALANCHE, ";
        sql <<" OPERATORNAME, ALARM_COMMENT, ALM, LOCATION_KEY";
        sql <<" from PRINTABLE_EVENT WHERE PKEY= " << m_pkey;*/
//		std::string strSql = databaseConnection->prepareSQLStatement( PRINTABLE_EVENT_SELECT_65004, m_pkey );
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql,  PRINTABLE_EVENT_SELECT_65004, m_pkey );
   	
		
        // Set up the columnNames vector to be passed to executeQuery()
		std::string pkeyColumn = "PKEY";
        std::string printerColumn = "PRINTER";
        std::string alarmflagColumn = "ALARMFLAG";
        std::string createtimeColumn = "CREATETIME";
        std::string severityColumn = "SEVERITY";
		std::string assetColumn = "ASSET";
		std::string descriptionColumn = "DESCRIPTION";
        std::string valueColumn = "VALUE";
		std::string mmsColumn = "MMS";
		std::string dssColumn = "DSS";
		std::string avalancheColumn = "AVALANCHE";
		std::string operatornameColumn = "OPERATORNAME";
		std::string alarmcommentColumn = "ALARM_COMMENT";
		std::string almColumn = "ALM";
		std::string locationkeyColumn = "LOCATION_KEY";

		
		
        std::vector<std::string> columnNames;
		columnNames.push_back(pkeyColumn);
        columnNames.push_back(printerColumn);
        columnNames.push_back(alarmflagColumn);
        columnNames.push_back(createtimeColumn);
		columnNames.push_back(severityColumn);
		columnNames.push_back(assetColumn);
		columnNames.push_back(descriptionColumn);
		columnNames.push_back(valueColumn);
		columnNames.push_back(mmsColumn);
		columnNames.push_back(dssColumn);
		columnNames.push_back(avalancheColumn);
		columnNames.push_back(operatornameColumn);
		columnNames.push_back(alarmcommentColumn);
		columnNames.push_back(almColumn);
		columnNames.push_back(locationkeyColumn);


		
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery( strSql, columnNames );
		
        // Need to bring in the DataException
        using TA_Base_Core::DataException;
		
        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;
			
            std::ostringstream reasonMessage;
            reasonMessage << "No record found for pkey = " << m_pkey;
			TA_THROW( TA_Base_Core::DataException( reasonMessage.str().c_str(),DataException::NO_VALUE,"" ) );
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;
            std::ostringstream reasonMessage;
            reasonMessage << "More than one record found for pkey = " << m_pkey;
			TA_THROW(TA_Base_Core::DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }
		
        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_pkey = data->getUnsignedLongData( 0, pkeyColumn );
		m_printer = data->getStringData( 0, printerColumn );
        m_alarmflag = data->getStringData( 0, alarmflagColumn );
		m_createtime = data->getStringData( 0, createtimeColumn );
		m_severity = data->getStringData( 0, severityColumn );
		m_asset = data->getStringData( 0, assetColumn );
		m_description = data->getStringData( 0, descriptionColumn );
		m_value = data->getStringData( 0, valueColumn );
		m_mms = data->getStringData( 0, mmsColumn );
		m_dss = data->getStringData( 0, dssColumn );
		m_avalanche = data->getStringData( 0, avalancheColumn );
		m_operatorname = data->getStringData( 0, operatornameColumn );
		m_alarmcomment = data->getStringData( 0, alarmcommentColumn );
		m_alm = data->getStringData( 0, almColumn );
		m_locationkey = data->getUnsignedLongData( 0, locationkeyColumn );

		
        m_printerOrig       = m_printer;
        m_pkeyOrig          = m_pkey;
        m_alarmflagOrig     = m_alarmflag;
        m_createtimeOrig    = m_createtime;
        m_severityOrig      = m_severity;
        m_assetOrig         = m_asset;
        m_descriptionOrig   = m_description;
        m_valueOrig         = m_value;
        m_mmsOrig           = m_mms;
        m_dssOrig           = m_dss;
        m_avalancheOrig     = m_avalanche;
		m_operatornameOrig  = m_operatorname;
		m_alarmcommentOrig  = m_alarmcomment;
		m_almOrig           = m_alm;
		m_locationkeyOrig   = m_locationkey;
		
        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;        
		
        m_isValidData = true;
		
        FUNCTION_EXIT;
    }
	
	
    void PrintableEventHelper::modifyExisting()
    {
        FUNCTION_ENTRY("modifyExisting");
		
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
			"record %lu already exists. It's details will be updated.", m_pkey );
		
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Event_Ad, TA_Base_Core::Write);
		
		//convert the m_scheduledTime to a string

		
       /* std::ostringstream sql;
        sql << "update PRINTABLE_EVENT set PRINTER = " << m_printer
			<< ", ALARMFLAG = " << m_alarmflag
			<< ", CREATETIME = " << m_createtime
			<< ", SEVERITY = " << m_severity
			<< ", ASSET = " << m_asset
			<< ", DESCRIPTION = " << m_description
			<< ", VALUE = " << m_value
			<< ", MMS = " << m_mms
			<< ", DSS = " << m_dss
			<< ", AVALANCHE = " << m_avalanche
            << ", OPERATORNAME = " << m_operatorname
			<< ", ALARM_COMMENT = " << m_alarmcomment
			<< ", ALM = " << m_alm
			<< ", LOCATION_KEY = " << m_locationkey
            << " where PKEY= " << m_pkey;*/
//		std::string strSql = databaseConnection->prepareSQLStatement(PRINTABLE_EVENT_DELETE_65005, m_printer, m_alarmflag, m_createtime, m_severity,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, PRINTABLE_EVENT_DELETE_65005, m_printer, m_alarmflag, m_createtime, m_severity,
			m_asset, m_description, m_value,m_mms,m_dss, m_avalanche, m_operatorname,m_alarmcomment,m_alm, m_locationkey, m_pkey );
		
        databaseConnection->executeModification( strSql );
		
		
        FUNCTION_EXIT;
    }
	
    void PrintableEventHelper::setPkey( unsigned long pkey)
    {
        m_pkey = pkey;
    }

    void PrintableEventHelper::setPrinter( std::string printer)
	{
        m_printer = printer;
    }

    void PrintableEventHelper::setAlarmflag( std::string alarmflag)
	{
        m_alarmflag = alarmflag;
	}

    void PrintableEventHelper::setCreatetime(  std::string createtime )
	{
        m_createtime = createtime;

	}

    void PrintableEventHelper::setSeverity( std::string severity)
	{
        m_severity = severity;

	}

    void PrintableEventHelper::setValue( std::string value)
	{
        m_value = value;

	}

    void PrintableEventHelper::setAsset( std::string asset)
	{
        m_asset = asset;

	}
    void PrintableEventHelper::setDescription( std::string description )
	{
        m_description = description;
 
	}

    void PrintableEventHelper::setMMS( std::string mms)
	{
        m_mms = mms;

	}

    void PrintableEventHelper::setDSS( std::string dss)
	{
        m_dss = dss;

	}
    void PrintableEventHelper::setAvalanche( std::string avalanche)
	{
        m_avalanche = avalanche;

	}

    void PrintableEventHelper::setOperatorname( std::string operatorname)
	{
        m_operatorname = operatorname;

	}

    void PrintableEventHelper::setAlarm_Comment( std::string alarmcomment)
	{
        m_alarmcomment = alarmcomment;

	}

    void PrintableEventHelper::setAlm( std::string alm)
	{
        m_alm = alm;

	}

    void PrintableEventHelper::setLocation_Key( unsigned long locationkey)
	{
        m_locationkey = locationkey;

	}


	
	bool PrintableEventHelper::hasChanged()
	{
		FUNCTION_ENTRY("hasChanged");
		
		if(
             (m_printerOrig!= m_printer) ||
             (m_pkeyOrig!= m_pkey)||
             (m_alarmflagOrig!= m_alarmflag)||
             (m_createtimeOrig!= m_createtime)||
             (m_severityOrig!= m_severity)||
             (m_assetOrig!= m_asset)||
             (m_descriptionOrig!= m_description)||
             (m_valueOrig!= m_value)||
             (m_mmsOrig!= m_mms)||
             (m_dssOrig!= m_dss)||
             (m_avalancheOrig!= m_avalanche)||
		     (m_operatornameOrig!= m_operatorname)||
		     (m_alarmcommentOrig!= m_alarmcomment)||
		     (m_almOrig!= m_alm)||
		     (m_locationkeyOrig!= m_locationkey)	
		  )
		{
            // Something is different
			return true;
		}
        
        // Nothing has changed
		FUNCTION_EXIT;
		return false;
	}
	
	
	
	
} // closes TA_IRS_Core


