/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/tis_agent/src/PredefinedMessageLibraryAccessFactory.cpp $
  * @author Robin Ashcroft
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/21 18:27:55 $
  * Last modified by:  $Author: hongzhi.zhang $
  *
  * PredefinedMessageLibraryAccessFactory is a singleton that is used to generate and retrieve
  * IPredefinedMessageLibrary objects.
  */

#pragma warning ( disable : 4786 )

#include <string>
#include <sstream>

#include "core/data_access_interface/tis_agent/src/PredefinedMessageLibraryData.h"
#include "core/data_access_interface/tis_agent/src/ConfigPredefinedMessageLibraryData.h"
#include "core/data_access_interface/tis_agent/src/PredefinedMessageLibraryAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/PredefinedMessageLibraryTable.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

	TA_Base_Core::ReEntrantThreadLockable PredefinedMessageLibraryAccessFactory::m_singletonLock;
	PredefinedMessageLibraryAccessFactory* PredefinedMessageLibraryAccessFactory::m_theClassInstance = NULL;

	PredefinedMessageLibraryAccessFactory& PredefinedMessageLibraryAccessFactory::getInstance()
	{
	    TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( m_theClassInstance == NULL )
		{
			// Create the one & only object

            m_theClassInstance = new PredefinedMessageLibraryAccessFactory();
		}

		return *m_theClassInstance;
	}

	std::vector<IPredefinedMessageLibrary*> PredefinedMessageLibraryAccessFactory::getPredefinedMessageLibraries(EDataTypes connType)
    {
        return getPredefinedMessageLibrariesWhere("", connType);
    }


	/*
	void PredefinedMessageLibraryAccessFactory::refreshTiTables(bool isOcc)
	{
		const std::string refreshScript = "begin dbms_snapshot.refresh('TABLE_NAME','F'); commit; end;";
		const std::string tableName = "TABLE_NAME";
		std::string s1 = refreshScript;
		s1.replace(s1.find(tableName), tableName.size(), TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME);
		std::string s2 = refreshScript;
		s2.replace(s2.find(tableName), tableName.size(), TA_TISAgentDAI::PREDEFINED_MESSAGE_TABLE_NAME);
		TA_Base_Core::IDatabase* databaseConnection = NULL;
		try
		{
			if(true == isOcc)
			{
				databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Tis_OCC_Cd, Write);
			}
			else
			{
				databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Tis_Cd, Write);
			}
			databaseConnection->executeModification(s1);
			databaseConnection->executeModification(s2);
		}
		catch(const TA_Base_Core::DatabaseException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", "");
			throw;
		}
	}
	*/

    std::vector<IPredefinedMessageLibrary*> PredefinedMessageLibraryAccessFactory::getPredefinedMessageLibrariesWhere(std::string whereSQL /* = "" */, EDataTypes connType /*Tis_Cd*/)
    {
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(connType, Read);

        // Get all the PredefinedMessageLibrary primary keys that exist in the table.

        /*std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN;
        selectSql << " from " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME;
        selectSql << " " << whereSQL;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_SELECT_38501, whereSQL);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38501, whereSQL);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38501, whereSQL);

        // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it,

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN );

        TA_Base_Core::IData* data = databaseConnection->executeQuery( strSql, columnNames );

        if ( 0 == data->getNumRows() )
        {
            std::stringstream error;
            error << "There are no Predefined Message Library in the database table ";
            error << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME;

            delete data;
            data = NULL;

            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // Create the vector of pointers to IPredefinedMessageLibrary.
        std::vector< IPredefinedMessageLibrary* > predefinedMessageLibraries;

        try
        {
            bool moreData = true;
            while (moreData)
            {
		        for ( unsigned long i = 0; i < data->getNumRows(); ++i )
		        {
			        predefinedMessageLibraries.push_back( new ConfigPredefinedMessageLibraryData(
				        data->getUnsignedLongData( i, TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN ), connType ) );
		        }

                // clean up used data object
                delete data;
                data = NULL;

                // try to read more data
                moreData = databaseConnection->moreData(data);
            }
        }
        catch(...)
        {
            // clean up in case an exception was thrown midway
            if (data != NULL)
            {
                delete data;
                data = NULL;
            }

            // throw on
            throw;
        }

        return predefinedMessageLibraries;
    }



	IConfigPredefinedMessageLibrary* PredefinedMessageLibraryAccessFactory::createNewPredefinedMessageLibrary( unsigned long version, const std::string& libraryType,
		std::vector<unsigned char>& libraryFileContent, EDataTypes conntype)
	{

		return new TA_Base_Core::ConfigPredefinedMessageLibraryData( version, libraryType, libraryFileContent, conntype);
	}

	IPredefinedMessageLibrary* PredefinedMessageLibraryAccessFactory::getPredefinedMessageLibrary( unsigned long version, const std::string& libraryType, EDataTypes connType )
	{

		LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Tis_Cd PredefinedMessageLibraryAccessFactory::getPredefinedMessageLibrary is called");

		std::stringstream whereSQL;
        whereSQL << "where " << TA_TISAgentDAI::VERSION_COLUMN
            << " = " << version << " and " << TA_TISAgentDAI::LIBRARY_TYPE_COLUMN << " = '" << libraryType << "'";

		std::vector<IPredefinedMessageLibrary*> messageLibraries; // There should be no more than one

		try
		{
			messageLibraries = getPredefinedMessageLibrariesWhere(whereSQL.str(), connType);
		}
		catch ( const TA_Base_Core::DataException& )
		{
			std::stringstream error;
            error << "No predefined message libraries exist in the database with the specified version number and type";
            error << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME;
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
		}

		return messageLibraries[0];
	}

	//hongran++ TD17500
	IPredefinedMessageLibrary* PredefinedMessageLibraryAccessFactory::getOCCLocalPredefinedMessageLibrary( unsigned long version, const std::string& libraryType )
	{
		
		LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Tis_OCC_Cd getOCCLocalPredefinedMessageLibrary is called");
		std::stringstream whereSQL;

        whereSQL << "where " << TA_TISAgentDAI::VERSION_COLUMN
            << " = " << version << " and " << TA_TISAgentDAI::LIBRARY_TYPE_COLUMN << " = '" << libraryType << "'";

		std::vector<IPredefinedMessageLibrary*> messageLibraries; // There should be no more than one

		try
		{
			messageLibraries = getOCCLocalPredefinedMessageLibrariesWhere(whereSQL.str());
		}
		catch ( const TA_Base_Core::DataException& )
		{
			std::stringstream error;
            error << "No predefined message libraries exist in the database with the specified version number and type";
            error << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME;
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
		}

		return messageLibraries[0];
	}
    std::vector<IPredefinedMessageLibrary*> PredefinedMessageLibraryAccessFactory::getOCCLocalPredefinedMessageLibrariesWhere(std::string whereSQL /* = "" */)
    {
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Tis_OCC_Cd, Read);

        // Get all the PredefinedMessageLibrary primary keys that exist in the table.

      /*  std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN;
        selectSql << " from " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME;
        selectSql << " " << whereSQL;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_SELECT_38501, whereSQL);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38501, whereSQL);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38501, whereSQL);

        // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it,

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN );

        TA_Base_Core::IData* data = databaseConnection->executeQuery( strSql, columnNames );

        if ( 0 == data->getNumRows() )
        {
            std::stringstream error;
            error << "There are no Predefined Message Library in the database table ";
            error << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME;

            delete data;
            data = NULL;

            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // Create the vector of pointers to IPredefinedMessageLibrary.
        std::vector< IPredefinedMessageLibrary* > predefinedMessageLibraries;

        try
        {
            bool moreData = true;
            while (moreData)
            {
		        for ( unsigned long i = 0; i < data->getNumRows(); ++i )
		        {
			        predefinedMessageLibraries.push_back( new ConfigPredefinedMessageLibraryData(
				        data->getUnsignedLongData( i, TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN ), Tis_OCC_Cd ) );
		        }

                // clean up used data object
                delete data;
                data = NULL;

                // try to read more data
                moreData = databaseConnection->moreData(data);
            }
        }
        catch(...)
        {
            // clean up in case an exception was thrown midway
            if (data != NULL)
            {
                delete data;
                data = NULL;
            }

            // throw on
            throw;
        }

        return predefinedMessageLibraries;
    }

	
    void PredefinedMessageLibraryAccessFactory::deleteOCCLocalAllMessageLibrariesOfTypeNotMatching( const std::string& libraryType, unsigned long currentVersion, unsigned long nextVersion )
    {
		LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Tis_OCC_Cd deleteOCCLocalAllMessageLibrariesOfTypeNotMatching is called");		
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Tis_OCC_Cd, Write);

        // Prepare a list of comma separated message libraries primary keys/Versions
        std::stringstream messageLibrariesPKeys;
        std::stringstream messageLibrariesVersion;

       /* std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN
                  << "," << TA_TISAgentDAI::VERSION_COLUMN
                  << " from " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME
                  << " where " << TA_TISAgentDAI::LIBRARY_TYPE_COLUMN << " = '" << libraryType << "' and "
                  << TA_TISAgentDAI::VERSION_COLUMN << " not in (" << currentVersion << "," << nextVersion << ")";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_SELECT_38502,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38502,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38502,
			libraryType, currentVersion, nextVersion);

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::VERSION_COLUMN );

        TA_Base_Core::IData* data = databaseConnection->executeQuery( strSql, columnNames );

        if ( 0 == data->getNumRows() )
        {
            delete data;
            data = NULL;

            std::stringstream message;
            message << "There are no Predefined Message Library of type " << libraryType
                << " in the database table " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME
                << " not matching version " << currentVersion << " and " << nextVersion;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, message.str().c_str());
            return;
        }

        try
        {
            bool moreData = true;
            while (moreData)
            {
		        for ( unsigned long i = 0; i < data->getNumRows(); ++i )
		        {
                    if (i > 0)
                    {
                        messageLibrariesPKeys << ",";
                        messageLibrariesVersion << ",";
                    }

                    messageLibrariesPKeys << data->getUnsignedLongData( i, TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN );
                    messageLibrariesVersion << data->getUnsignedLongData( i, TA_TISAgentDAI::VERSION_COLUMN );
		        }

                // clean up used data object
                delete data;
                data = NULL;

                // try to read more data
                moreData = databaseConnection->moreData(data);
            }
        }
        catch(...)
        {
            // clean up in case an exception was thrown midway
            if (data != NULL)
            {
                delete data;
                data = NULL;
            }

            // throw on
            throw;
        }

        std::stringstream logMessage;
        logMessage << "Deleting Predefined Message Library of type " << libraryType
                   << " matching version " << messageLibrariesVersion.str();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, logMessage.str().c_str());

        // delete the default attributes
       /* std::ostringstream formatSQL;
		formatSQL.str("");
        formatSQL << "delete from " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE
                  << " where " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PARENT_LIBRARY_COLUMN
                  << " in (" << messageLibrariesPKeys.str() << ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_DELETE_38901,
//		strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38901,

		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38901,
			messageLibrariesPKeys.str());

        databaseConnection->executeModification( strSql );

        // Then delete all the messages that 'belong' to this library
	/*	formatSQL.str("");
        formatSQL << "delete from " << TA_TISAgentDAI::PREDEFINED_MESSAGE_TABLE_NAME
                  << " where " << TA_TISAgentDAI::PARENT_LIBRARY_COLUMN
                  << " in (" << messageLibrariesPKeys.str() << ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_DELETE_38902,
//		strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38902,

		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38902,
			messageLibrariesPKeys.str());

        databaseConnection->executeModification( strSql );

		// Then delete the message library itself
	/*	formatSQL.str("");
        formatSQL << "delete from " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME
                  << " where " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN
                  << " in (" << messageLibrariesPKeys.str() << ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_DELETE_38903,
//		strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38903,

		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38903,
			messageLibrariesPKeys.str());

		databaseConnection->executeModification( strSql );
    }
	//++hongran TD17500

    void PredefinedMessageLibraryAccessFactory::deleteAllMessageLibrariesOfTypeNotMatching( const std::string& libraryType, unsigned long currentVersion, unsigned long nextVersion, EDataTypes connType )
    {

		LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Tis_Cd deleteAllMessageLibrariesOfTypeNotMatching::getPredefinedMessageLibrary is called");
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(connType, Write);

        // Prepare a list of comma separated message libraries primary keys/Versions
        std::stringstream messageLibrariesPKeys;
        std::stringstream messageLibrariesVersion;

       /* std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN
                  << "," << TA_TISAgentDAI::VERSION_COLUMN
                  << " from " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME
                  << " where " << TA_TISAgentDAI::LIBRARY_TYPE_COLUMN << " = '" << libraryType << "' and "
                  << TA_TISAgentDAI::VERSION_COLUMN << " not in (" << currentVersion << "," << nextVersion << ")";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_SELECT_38502,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38502,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38502,
			libraryType, currentVersion, nextVersion);

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::VERSION_COLUMN );

        TA_Base_Core::IData* data = databaseConnection->executeQuery( strSql, columnNames );

        if ( 0 == data->getNumRows() )
        {
            delete data;
            data = NULL;

            std::stringstream message;
            message << "There are no Predefined Message Library of type " << libraryType
                << " in the database table " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME
                << " not matching version " << currentVersion << " and " << nextVersion;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, message.str().c_str());
            return;
        }

        try
        {
            bool moreData = true;
            while (moreData)
            {
		        for ( unsigned long i = 0; i < data->getNumRows(); ++i )
		        {
                    if (i > 0)
                    {
                        messageLibrariesPKeys << ",";
                        messageLibrariesVersion << ",";
                    }

                    messageLibrariesPKeys << data->getUnsignedLongData( i, TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN );
                    messageLibrariesVersion << data->getUnsignedLongData( i, TA_TISAgentDAI::VERSION_COLUMN );
		        }

                // clean up used data object
                delete data;
                data = NULL;

                // try to read more data
                moreData = databaseConnection->moreData(data);
            }
        }
        catch(...)
        {
            // clean up in case an exception was thrown midway
            if (data != NULL)
            {
                delete data;
                data = NULL;
            }

            // throw on
            throw;
        }

        std::stringstream logMessage;
        logMessage << "Deleting Predefined Message Library of type " << libraryType
                   << " matching version " << messageLibrariesVersion.str();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, logMessage.str().c_str());

        // delete the default attributes
       /* std::ostringstream formatSQL;
		formatSQL.str("");
        formatSQL << "delete from " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE
                  << " where " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PARENT_LIBRARY_COLUMN
                  << " in (" << messageLibrariesPKeys.str() << ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_DELETE_38901,
//		strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38901,

		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38901,
			messageLibrariesPKeys.str());

        databaseConnection->executeModification( strSql );

        // Then delete all the messages that 'belong' to this library
		/*formatSQL.str("");
        formatSQL << "delete from " << TA_TISAgentDAI::PREDEFINED_MESSAGE_TABLE_NAME
                  << " where " << TA_TISAgentDAI::PARENT_LIBRARY_COLUMN
                  << " in (" << messageLibrariesPKeys.str() << ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_DELETE_38902,
//		strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38902,

		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38902,
			messageLibrariesPKeys.str());

        databaseConnection->executeModification( strSql );

		// Then delete the message library itself
		/*formatSQL.str("");
        formatSQL << "delete from " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME
                  << " where " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN
                  << " in (" << messageLibrariesPKeys.str() << ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_DELETE_38903,
//		strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38903,

		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38903,
			messageLibrariesPKeys.str());

		databaseConnection->executeModification( strSql );
    }

	PredefinedMessage PredefinedMessageLibraryAccessFactory::getPredefinedMessage( unsigned long version, std::string& libraryType, 
																				   int librarySection, unsigned short messageTag, EDataTypes connType)
	{
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(connType, Read);
		std::vector<std::string> columnNames;
		//std::stringstream messageSql;
		PredefinedMessage ret;


	/*	messageSql << "select ";
		messageSql << TA_TISAgentDAI::PARENT_LIBRARY_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::MESSAGE_TAG_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::LIBRARY_SECTION_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::DESCRIPTION_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::PRIORITY_COLUMN << ", ";
        messageSql << TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::MESSAGE_CONTENT_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::ENGLISH_ONLY_COLUMN;
		messageSql << " from " << TA_TISAgentDAI::PREDEFINED_MESSAGE_TABLE_NAME;
		messageSql << " where " << TA_TISAgentDAI::PARENT_LIBRARY_COLUMN 
				   << " in (select " << TA_TISAgentDAI::PARENT_LIBRARY_COLUMN 
				   << " from " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME 
				   << " where version = " << version
				   << " and LIBRARY_TYPE = '" << libraryType <<"')"
				   << " and " << TA_TISAgentDAI::LIBRARY_SECTION_COLUMN << " = " << librarySection 
				   << " and " << TA_TISAgentDAI::MESSAGE_TAG_COLUMN << " = " << messageTag ;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_SELECT_38503,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38503,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38503,
			version, libraryType, librarySection, messageTag);
		
		// Set up the column names for data we are expecting to receive
		columnNames.push_back( TA_TISAgentDAI::PARENT_LIBRARY_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::MESSAGE_TAG_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::LIBRARY_SECTION_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::DESCRIPTION_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::PRIORITY_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::MESSAGE_CONTENT_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::ENGLISH_ONLY_COLUMN );
		
		std::auto_ptr<TA_Base_Core::IData> messagedata (databaseConnection->executeQuery(strSql, columnNames));
		
		if( 0 == messagedata->getNumRows() )
		{
			std::stringstream error;
            error << "There are no Predefined Message with " 
				  << "version = " << version << ", "
				  << "library type = " << libraryType << ", "
				  << "message section = " << librarySection << ", "
				  << "message tag = " << messageTag << " "
				  << "in the database table "
				  << TA_TISAgentDAI::PREDEFINED_MESSAGE_TABLE_NAME;
            
			TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ) );
		}
		else if (messagedata->getNumRows() > 1)
		{
			std::stringstream error;
            error << "More than one Predefined Message Library found with " 
				<< "version = " << version << ", "
				<< "library type = " << libraryType << ", "
				<< "message section = " << librarySection << ", "
				<< "message tag = " << messageTag << " "
				<< "in the database table "
				<< TA_TISAgentDAI::PREDEFINED_MESSAGE_TABLE_NAME;
            
			TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE, "" ) );
		}
		else
		{
			ret.messageTag = messagedata->getIntegerData( 0, TA_TISAgentDAI::MESSAGE_TAG_COLUMN );
			ret.librarySection = messagedata->getIntegerData( 0, TA_TISAgentDAI::LIBRARY_SECTION_COLUMN );
			ret.description = messagedata->getStringData( 0, TA_TISAgentDAI::DESCRIPTION_COLUMN );
			ret.priority = messagedata->getIntegerData( 0, TA_TISAgentDAI::PRIORITY_COLUMN );
			ret.repeatInterval = messagedata->getIntegerData( 0, TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN );
			ret.message = messagedata->getStringData( 0, TA_TISAgentDAI::MESSAGE_CONTENT_COLUMN ); 
			ret.englishOnly = (0 != messagedata->getIntegerData( 0, TA_TISAgentDAI::ENGLISH_ONLY_COLUMN ) ); 
		}
		
		return ret;
	}
} //end namespace TA_Base_Core
