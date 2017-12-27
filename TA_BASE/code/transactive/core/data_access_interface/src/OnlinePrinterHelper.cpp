/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/OnlinePrinterHelper.cpp $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * OnlinePrinterHelper is an object that is held by OnlinePrinter and ConfigOnlinePrinter objects. 
  * It contains all data used by OnlinePrinters, and manipulation
  * methods for the data. It helps avoid re-writing code for both OnlinePrinter and ConfigOnlinePrinter.
  */


#if defined(_MSC_VER)
	#pragma warning(disable:4786)
	#pragma warning(disable:4503)	// decorated name length exceeded, name was truncated
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <vector>
#include <algorithm>
#include <ctime>

#include "core/data_access_interface/src/OnlinePrinterHelper.h"
#include "core/data_access_interface/src/OnlinePrinterAccessFactory.h"
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

    static const std::string PKEY_COL = "PKEY";
    static const std::string PRINTER_COL = "PRINTER";
    static const std::string LOCATION_COL = "LOCATIONKEY";
    static const std::string SUBSYSTEM_COL = "SUBSYSTEMKEY";
    static const std::string ISALARM_COL = "IS_ALARM";

    static std::string toUpper(const std::string& in)
    {
        std::string out(in);
        std::transform(out.begin(), out.end(), out.begin(), toupper);
        return out;
    }

	OnlinePrinterHelper::OnlinePrinterHelper( const OnlinePrinterHelper& theOnlinePrinterHelper)
		: m_idKey(-1),
		  m_printer(theOnlinePrinterHelper.m_printer),
		  m_location(theOnlinePrinterHelper.m_location),
		  m_subsystem(theOnlinePrinterHelper.m_subsystem),
		  m_isAlarm(theOnlinePrinterHelper.m_isAlarm),
          m_isValidData(false),
          m_isNew(true)
    {	  
	}


    OnlinePrinterHelper::OnlinePrinterHelper(const unsigned long idKey)
        : m_idKey(idKey),
		  m_printer(""),
          m_location(0),
          m_subsystem(0),
          m_isAlarm(false),
          m_isValidData(false),
          m_isNew(false)
    {
    }


    OnlinePrinterHelper::OnlinePrinterHelper()
        : m_idKey(-1),
		  m_printer(""),
          m_location(0),
          m_subsystem(0),
          m_isAlarm(false),
          m_isValidData(false),
          m_isNew(true)
    {
    }


    OnlinePrinterHelper::OnlinePrinterHelper(unsigned long row, TA_Base_Core::IData& data)
        : m_idKey(-1),
		  m_printer(""),
          m_location(0),
          m_subsystem(0),
          m_isAlarm(false),
          m_isValidData(false),
          m_isNew(false)
    {
        m_idKey = data.getUnsignedLongData(row, PKEY_COL);

        reloadUsing(row, data);
    }


    OnlinePrinterHelper::~OnlinePrinterHelper()
    {
    }

	unsigned long OnlinePrinterHelper::getKey()
    {
        FUNCTION_ENTRY("getKey");

        FUNCTION_EXIT;
        return m_idKey;
    }


    std::string OnlinePrinterHelper::getPrinter()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_printer;
    }

    unsigned long OnlinePrinterHelper::getLocation()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_location;
    }

    unsigned long OnlinePrinterHelper::getSubsystem()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_subsystem;
    }

    bool OnlinePrinterHelper::getIsAlarm()
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        return m_isAlarm;
    }

    void OnlinePrinterHelper::setPrinter(const std::string& pPrinter)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_printer = pPrinter;
    }

    void OnlinePrinterHelper::setLocation(unsigned long pLocation)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_location = pLocation;
    }

    void OnlinePrinterHelper::setSubsystem(unsigned long pSubsystem)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_subsystem = pSubsystem;
    }

    void OnlinePrinterHelper::setIsAlarm(bool pIsAlarm)
    {
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_isAlarm = pIsAlarm;
    }

    void OnlinePrinterHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate" );

        // This method cannot be called until the key has been set
        TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new OnlinePrinter");

        m_isValidData = false;

        FUNCTION_EXIT;
    }


    void OnlinePrinterHelper::reload()
    {
        FUNCTION_ENTRY("reload" );
        // Ensure the OnlinePrinter has already been written to the database. It doesn't make
        // any sense to reload an OnlinePrinter that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This OnlinePrinter does not yet exist in the database. Therefore it cannot be reloaded.");
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);


        // create the SQL string to retrieve the data of the OnlinePrinter
        // based upon the key
        /*std::ostringstream sql;
		sql << "select " << PKEY_COL << "," << PRINTER_COL << ","  << LOCATION_COL
            << "," << SUBSYSTEM_COL << "," << ISALARM_COL << " from ONLINE_PRINTER where " 
            << PKEY_COL << " = " << m_idKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ONLINE_PRINTER_STD_SELECT_18505, m_idKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ONLINE_PRINTER_SELECT_18505, m_idKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ONLINE_PRINTER_SELECT_18505, m_idKey);
 
        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
		columnNames.push_back(PKEY_COL);
        columnNames.push_back(PRINTER_COL);
        columnNames.push_back(LOCATION_COL);
        columnNames.push_back(SUBSYSTEM_COL);
        columnNames.push_back(ISALARM_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

        if (NULL == data || 0 == data->getNumRows()) 
        {
            // OnlinePrinter record not found
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
		    reasonMessage << "No data found for = " << m_idKey;			
            TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"Branch Id" ) );
        }
        
		TA_ASSERT(data->getNumRows() == 1, "Invalid number of online printer records."); 
        reloadUsing(0, *data);
        		
        FUNCTION_EXIT;
    }

    void OnlinePrinterHelper::reloadUsing(unsigned long row, TA_Base_Core::IData& data)
    {   
        FUNCTION_ENTRY("reloadUsing" );

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
		m_idKey	   = data.getUnsignedLongData( row, PKEY_COL );
        m_printer  = data.getStringData( row, PRINTER_COL );
        m_location = data.getUnsignedLongData( row, LOCATION_COL );
        m_subsystem= data.getUnsignedLongData( row, SUBSYSTEM_COL );
        m_isAlarm = (0 != data.getUnsignedLongData( row, ISALARM_COL )) ? true : false;

        // Need to record that we now have valid data
        m_isValidData = true;
        FUNCTION_EXIT;
    }

    void OnlinePrinterHelper::writeOnlinePrinterData() throw(TA_Base_Core::TransactiveException)
    {       
        FUNCTION_ENTRY( "writeOnlinePrinterData" );     

        // Check all values exist. If not add them to the dataConfiguratonException
        std::vector<std::string> fieldNames;
        if (m_printer.empty())
        {
            fieldNames.push_back(PRINTER_COL);
        }
        if (m_subsystem == 0)
        {
            fieldNames.push_back(SUBSYSTEM_COL);
        }
        if (m_location == 0)
        {
            fieldNames.push_back(LOCATION_COL);
        }
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("OnlinePrinter data not fully specified. OnlinePrinter cannot be written to database",fieldNames));
        }

        // Write the OnlinePrinter to the database

        // Need to check if this is a new OnlinePrinter or an existing OnlinePrinter - 
        // as a new OnlinePrinter will be inserted, while an existing OnlinePrinter will be updated
        if (m_isNew) // This is a new OnlinePrinter
        {
            addNewOnlinePrinter();
        }
        else // This is an existing OnlinePrinter
        {
            modifyExistingOnlinePrinter();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }


    void OnlinePrinterHelper::deleteOnlinePrinter(bool cascade)
    {
        FUNCTION_ENTRY("deleteOnlinePrinter");

        TA_ASSERT(!m_isNew, "This OnlinePrinter does not yet exist in the database, and therefore cannot be deleted");
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);
        /*std::ostringstream sql;
        
        sql << "delete ONLINE_PRINTER where " << PKEY_COL << " = " << m_idKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ONLINE_PRINTER_STD_DELETE_18901, m_idKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ONLINE_PRINTER_DELETE_18901, m_idKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ONLINE_PRINTER_DELETE_18901, m_idKey);
        databaseConnection->executeModification(strSql);

        FUNCTION_EXIT;
    }


    void OnlinePrinterHelper::modifyExistingOnlinePrinter() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("modifyExistingOnlinePrinter");

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "OnlinePrinter id=%ul already exists. It's data will be updated.",m_idKey);


        // get a connection to the database
        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        /*std::ostringstream sql;
        sql << "update ONLINE_PRINTER set " 
            << PRINTER_COL << " = '"
            << (databaseConnection->escapeInsertString(m_printer)).c_str() << "', " 
            << SUBSYSTEM_COL << " = " << m_subsystem << ","
            << LOCATION_COL << " = " << m_location << ","
            << ISALARM_COL << " = " << m_isAlarm
            << " where " << PKEY_COL << " = " << m_idKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ONLINE_PRINTER_STD_UPDATE_18651,
//		std::string strSql  = databaseConnection->prepareSQLStatement(ONLINE_PRINTER_UPDATE_18651,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ONLINE_PRINTER_UPDATE_18651,
			databaseConnection->escapeInsertString(m_printer), m_subsystem, m_location, m_isAlarm, m_idKey);
        
        databaseConnection->executeModification(strSql);

        // nothing more to be done here
        FUNCTION_EXIT;
    }


    void OnlinePrinterHelper::addNewOnlinePrinter() throw(TA_Base_Core::TransactiveException)
    {
        FUNCTION_ENTRY("addNewOnlinePrinter");

        TA_Base_Core::IDatabase *databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
        /*std::ostringstream sql;
        sql << "select ONLINE_PRINTER_SEQ.NEXTVAL from DUAL"; */
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ONLINE_PRINTER_Oracle_SELECT_18551);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ONLINE_PRINTER_SELECT_18551);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ONLINE_PRINTER_SELECT_18551);

        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);

        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (data->getNumRows() == 0)
        {
            delete data;
            data = NULL;
            TA_THROW( TA_Base_Core::TransactiveException("Cannot get next value") );
        }

        m_idKey = data->getUnsignedLongData(0, PKEY_COL);

        delete data;
        data = NULL;

        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "OnlinePrinter %ul is a new OnlinePrinter. It will be added to the database.",m_idKey);

        /*sql.str("");
        sql << "insert into ONLINE_PRINTER (" 
            << PKEY_COL << ", "
            << PRINTER_COL << "," << LOCATION_COL << "," << SUBSYSTEM_COL << "," << ISALARM_COL
            << ") "
            << "values ("
            << m_idKey << ",'" 
            << (databaseConnection->escapeInsertString(m_printer)).c_str() << "',"
            << m_location << "," << m_subsystem << "," << m_isAlarm << ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ONLINE_PRINTER_STD_INSERT_18801, m_idKey,
//		strSql  = databaseConnection->prepareSQLStatement(ONLINE_PRINTER_INSERT_18801, m_idKey,

		databaseConnection->prepareSQLStatement(strSql, ONLINE_PRINTER_INSERT_18801, m_idKey,
			databaseConnection->escapeInsertString(m_printer), m_location, m_subsystem, m_isAlarm);

        databaseConnection->executeModification(strSql);
        
        FUNCTION_EXIT;
    }

} // closes TA_Core

