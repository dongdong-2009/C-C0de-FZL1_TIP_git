/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/OnlinePrinterAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/26 14:05:40 $
  * Last modified by:  $Author: Ouyang $
  *
  * <description>
  *
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER


#include "core/data_access_interface/src/OnlinePrinterAccessFactory.h"
#include "core/data_access_interface/src/OnlinePrinter.h"
#include "core/data_access_interface/src/ConfigOnlinePrinter.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DataException;

namespace TA_Base_Core
{
    static const std::string PKEY_COL = "PKEY";
    static const std::string PRINTER_COL = "PRINTER";
    static const std::string LOCATION_COL = "LOCATIONKEY";
    static const std::string SUBSYSTEM_COL = "SUBSYSTEMKEY";
    static const std::string ISALARM_COL = "IS_ALARM";

    OnlinePrinterAccessFactory* OnlinePrinterAccessFactory::m_instance = 0;

    OnlinePrinterAccessFactory& OnlinePrinterAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new OnlinePrinterAccessFactory();
        }
        return *m_instance;
    }


    void OnlinePrinterAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }


	void OnlinePrinterAccessFactory::getOnlinePrinterBySql(IDatabase* pDatabase, const SQLStatement& sql, IOnlinePrinters& OnlinePrinters, const bool readWrite)
    {
        // get a connection to the database
        //TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);
		TA_ASSERT(NULL != pDatabase, "database connection is null");

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
        TA_Base_Core::IData* data = pDatabase->executeQuery(sql, columnNames);

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                if ( 0 == data->getUnsignedLongData(i, PKEY_COL) )
                {
					continue;
				}

                if (readWrite)
                {
                    OnlinePrinters.push_back( new ConfigOnlinePrinter(i, *data) );
                }
                else
                {
                    OnlinePrinters.push_back( new OnlinePrinter(i, *data) );
                }
            }

            delete data;
            data = NULL;
        }
        while( pDatabase->moreData(data) );
    }
    
  
    std::vector<OnlinePrinterAccessFactory::SubsystemRecord> OnlinePrinterAccessFactory::getSubsystems()
    {
        if (m_isSubsystemsLoaded)
        {
            return m_subsystems;
        }
		
		FUNCTION_ENTRY("getSubsystems");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Event_Ad, TA_Base_Core::Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SUBSYSTEM_STD_SELECT_18001);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SUBSYSTEM_SELECT_18001);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SUBSYSTEM_SELECT_18001);
//		TA_ASSERT(strSql.size() > 0, "OnlinePrinterAccessFactory::getSubsystems(): Prepare SQL Statement error");

		/*std::ostringstream sql;
		sql << "select pkey, name from SUBSYSTEM where pkey <> 0 order by name";*/

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);
        columnNames.push_back("NAME");

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified sql
        {
            delete data;
            data = NULL;

            std::ostringstream reasonMessage;
			reasonMessage << "No data found";
            TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }

        do
        {
            for (unsigned int i = 0; i < data->getNumRows(); ++i)
            {
                SubsystemRecord v;
                v.key = data->getUnsignedLongData(i, PKEY_COL);
                v.name = data->getStringData(i, "NAME");

                m_subsystems.push_back(v);
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        m_isSubsystemsLoaded = true;

		FUNCTION_EXIT;
        return m_subsystems;
    }

    
	IOnlinePrinter* OnlinePrinterAccessFactory::getOnlinePrinter(const unsigned long idKey, const bool readWrite)
    {
		FUNCTION_ENTRY("getOnlinePrinter");

		TA_Base_Core::IDatabase* databaseConnection =
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ONLINE_PRINTER_STD_SELECT_18503, PKEY_COL, PRINTER_COL,
//		std::string strSql  = databaseConnection->prepareSQLStatement(ONLINE_PRINTER_SELECT_18503, PKEY_COL, PRINTER_COL,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ONLINE_PRINTER_SELECT_18503, idKey);
//		TA_ASSERT(strSql.size() > 0, "OnlinePrinterAccessFactory::getOnlinePrinter(): Prepare SQL Statement error");

       /* std::ostringstream sql;
		sql << "select " << PKEY_COL << "," << PRINTER_COL << "," << LOCATION_COL << "," << SUBSYSTEM_COL
			<< "," << ISALARM_COL << " from ONLINE_PRINTER where " << PKEY_COL << " = " << idKey;*/
		
		IOnlinePrinters OnlinePrinters;
		getOnlinePrinterBySql(databaseConnection, strSql, OnlinePrinters, readWrite);

        if ( 0 == OnlinePrinters.size())
        {
            std::ostringstream message;
			message << "No data found for OnlinePrinter with key " << idKey;
            //TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strSql ) );
			std::string strExSql;
			int nExDbType= databaseConnection->getDbServerType();
			switch (nExDbType)
			{
			case enumOracleDb:
				strExSql = (strSql.strCommon.empty() ? strSql.strOracle : strSql.strCommon);
				break;
			case enumMysqlDb:
				strExSql = (strSql.strCommon.empty() ? strSql.strMySQL : strSql.strCommon);
				break;		
			}
			TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, strExSql ) );

        }

		TA_ASSERT(1 == OnlinePrinters.size(), "OnlinePrinter key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;		
		return OnlinePrinters[0];
    }
    
    IOnlinePrinters OnlinePrinterAccessFactory::getAllOnlinePrinters(unsigned long pLocation, const bool readWrite)
    {
		FUNCTION_ENTRY("getAllOnlinePrinters");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ONLINE_PRINTER_STD_SELECT_18504, PKEY_COL, PRINTER_COL, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(ONLINE_PRINTER_SELECT_18504, PKEY_COL, PRINTER_COL, 
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ONLINE_PRINTER_SELECT_18504, pLocation);
//		TA_ASSERT(strSql.size() > 0, "OnlinePrinterAccessFactory::getAllOnlinePrinters(): Prepare SQL Statement error");

        /*std::ostringstream sql;
		sql << "select " << PKEY_COL << "," << PRINTER_COL << "," << LOCATION_COL << "," << SUBSYSTEM_COL
			<< "," << ISALARM_COL << " from ONLINE_PRINTER where " << LOCATION_COL << " = " << pLocation
			<< " ORDER BY " << ISALARM_COL << "," << SUBSYSTEM_COL;*/

		IOnlinePrinters OnlinePrinters;
		getOnlinePrinterBySql(databaseConnection, strSql, OnlinePrinters, readWrite);

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;		
		return OnlinePrinters;
    }
    

    IConfigOnlinePrinter* OnlinePrinterAccessFactory::createOnlinePrinter()
    {            
        return new ConfigOnlinePrinter();    
    }


    IConfigOnlinePrinter* OnlinePrinterAccessFactory::copyOnlinePrinter(const IConfigOnlinePrinter* OnlinePrinterToCopy)
    {
        FUNCTION_ENTRY("copyOnlinePrinter");

        TA_ASSERT(OnlinePrinterToCopy !=  NULL, "The OnlinePrinter to copy was NULL");
        
        const ConfigOnlinePrinter* cast = dynamic_cast<const ConfigOnlinePrinter*>(OnlinePrinterToCopy);

        TA_ASSERT(cast != NULL, "OnlinePrinter passed could not be converted into a ConfigOnlinePrinter");

        FUNCTION_EXIT;
        return new ConfigOnlinePrinter(*cast);

    }


} // closes TA_Core


