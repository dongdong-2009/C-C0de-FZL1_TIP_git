/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/OnlinePrintCfgHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/21 00:07:03 $
  * Last modified by:  $Author: huirong.luo $
  *
  * OnlinePrintCfgHelper is an object that is held by OnlinePrintCfg and ConfigOnlinePrintCfg objects. 
  * It contains all data used by OnlinePrintCfgs, and manipulation
  * methods for the data. It helps avoid re-writing code for both OnlinePrintCfg and ConfigOnlinePrintCfg.
  */


#if defined(_MSC_VER)
	#pragma warning(disable:4786)
	#pragma warning(disable:4503)	// decorated name length exceeded, name was truncated
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/src/OnlinePrintCfgHelper.h"
#include "core/data_access_interface/src/OnlinePrintCfgAccessFactory.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    static const std::string DEFAULT_PRINTER_COL = "DEF_PRINTER";
    static const std::string LOCATION_COL = "LOCATIONKEY";
    static const std::string LOCATION_NAME_COL = "LOCATION.NAME";
	static const std::string LOCATION_DISPALYNAME_COL = "LOCATION.DISPLAY_NAME";

	OnlinePrintCfgHelper::OnlinePrintCfgHelper( const OnlinePrintCfgHelper& theOnlinePrintCfgHelper)
		: 
		  m_defaultPrinter(theOnlinePrintCfgHelper.m_defaultPrinter),
		  m_location(theOnlinePrintCfgHelper.m_location),
		  m_locationName(theOnlinePrintCfgHelper.m_locationName),
		  m_locationDisplayName(theOnlinePrintCfgHelper.m_locationDisplayName),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    OnlinePrintCfgHelper::OnlinePrintCfgHelper(const unsigned long idKey)
        : 
		  m_defaultPrinter(""),
          m_location(idKey),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    OnlinePrintCfgHelper::OnlinePrintCfgHelper()
        : 
		  m_defaultPrinter(""),
          m_location(0),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    OnlinePrintCfgHelper::OnlinePrintCfgHelper(unsigned long row, TA_Base_Core::IData& data)
        : 
		  m_defaultPrinter(""),
          m_location(0),
          m_isValidData(false),
          m_isNew(false)
    {
        m_location = data.getUnsignedLongData(row, LOCATION_COL);
        reloadUsing(row, data);
    }


    OnlinePrintCfgHelper::~OnlinePrintCfgHelper()
    {
    }



    std::string OnlinePrintCfgHelper::getDefaultPrinter()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_defaultPrinter;
    }

    unsigned long OnlinePrintCfgHelper::getLocation()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_location;
    }

    std::string OnlinePrintCfgHelper::getLocationName()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_locationName;
    }
    std::string OnlinePrintCfgHelper::getLocationDisplayName()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_locationDisplayName;
    }

    void OnlinePrintCfgHelper::setDefaultPrinter(const std::string& pPrinter)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_defaultPrinter = pPrinter;
    }

    void OnlinePrintCfgHelper::setLocation(unsigned long pLocation)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_location = pLocation;
    }

    void OnlinePrintCfgHelper::setLocationName(const std::string& pLocationName)
    {
        m_locationName = pLocationName;
    }

    void OnlinePrintCfgHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new OnlinePrintCfg");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

    void OnlinePrintCfgHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the OnlinePrintCfg has already been written to the database. It doesn't make
        // any sense to reload an OnlinePrintCfg that doesn't exist in the database.

        TA_ASSERT(!m_isNew, "This OnlinePrintCfg does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the data of the OnlinePrintCfg
        // based upon the key
        /*std::ostringstream sql;
		sql << " select " << DEFAULT_PRINTER_COL << "," << LOCATION_COL << "," << LOCATION_NAME_COL
            << " from ONLINE_PRINTCFG, LOCATION where " << LOCATION_COL << " = " << m_location
            << " AND LOCATION.PKEY = " << m_location;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ONLINE_PRINTCFG_STD_SELECT_28501, m_location, m_location);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ONLINE_PRINTCFG_SELECT_28501, m_location, m_location);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ONLINE_PRINTCFG_SELECT_28501, m_location, m_location);
 
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(DEFAULT_PRINTER_COL);
        columnNames.push_back(LOCATION_COL);
        columnNames.push_back(LOCATION_NAME_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql,columnNames));

        if (NULL == data.get() || 0 == data->getNumRows()) 
        {
            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for = " << m_location;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Branch Id" ) );
        }

		TA_ASSERT(data->getNumRows() == 1, "Invalid number of online printer config records."); 
        reloadUsing(0, *data);
        
        FUNCTION_EXIT;
    }

    void OnlinePrintCfgHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_defaultPrinter= data.getStringData( row, DEFAULT_PRINTER_COL );
		m_location		= data.getUnsignedLongData( row, LOCATION_COL );
        m_locationName	= data.getStringData( row, LOCATION_NAME_COL );
		m_locationDisplayName= data.getStringData( row, LOCATION_DISPALYNAME_COL );

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void OnlinePrintCfgHelper::writeOnlinePrintCfgData() throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeOnlinePrintCfgData" );     


        // Write the OnlinePrintCfg to the database

        // Need to check if this is a new OnlinePrintCfg or an existing OnlinePrintCfg - 
        // as a new OnlinePrintCfg will be inserted, while an existing OnlinePrintCfg will be updated
        if (m_isNew) // This is a new OnlinePrintCfg
        {
            addNewOnlinePrintCfg();
        }
        else // This is an existing OnlinePrintCfg
        {
            modifyExistingOnlinePrintCfg();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }



    void OnlinePrintCfgHelper::modifyExistingOnlinePrintCfg() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingOnlinePrintCfg");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "OnlinePrintCfg id=%ul already exists. It's data will be updated.",m_location);


        // get a connection to the database
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        /*std::ostringstream sql;
        sql << "update ONLINE_PRINTCFG set " 
            << DEFAULT_PRINTER_COL << " = '"
            << (databaseConnection->escapeInsertString(m_defaultPrinter)).c_str() << "'" 
            << " where " << LOCATION_COL << " = " << m_location;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ONLINE_PRINTCFG_STD_UPDATE_28651,
//		std::string strSql  = databaseConnection->prepareSQLStatement(ONLINE_PRINTCFG_UPDATE_28651,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ONLINE_PRINTCFG_UPDATE_28651,
			(databaseConnection->escapeInsertString(m_defaultPrinter)).c_str(), m_location);
        
        databaseConnection->executeModification(strSql);

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void OnlinePrintCfgHelper::addNewOnlinePrintCfg() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewOnlinePrintCfg");

        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        /*std::ostringstream sql;

        sql << "insert into ONLINE_PRINTCFG (" 
            << LOCATION_COL << ", " << DEFAULT_PRINTER_COL 
            << ") "
            << "values ("
            << m_location << ",'" 
            << (databaseConnection->escapeInsertString(m_defaultPrinter)).c_str() << "')";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ONLINE_PRINTCFG_STD_INSERT_28801,
//		std::string strSql  = databaseConnection->prepareSQLStatement(ONLINE_PRINTCFG_INSERT_28801,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ONLINE_PRINTCFG_INSERT_28801,
			m_location, databaseConnection->escapeInsertString(m_defaultPrinter) );

        databaseConnection->executeModification(strSql);
        
        FUNCTION_EXIT;
    }

} // closes TA_Core

