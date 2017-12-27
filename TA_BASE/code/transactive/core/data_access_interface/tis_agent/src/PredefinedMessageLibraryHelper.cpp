/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/tis_agent/src/PredefinedMessageLibraryHelper.cpp $
  * @author Robin Ashcroft
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/21 18:27:55 $
  * Last modified by: $Author: hongzhi.zhang $
  * 
  * PredefinedMessageLibraryHelper is an object that is held by PredefinedMessageLibraryData and ConfigPredefinedMessageLibraryData objects. 
  * It allows reading and writing a pre-defined message library to and from the database.
  */

#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: decorated type length exceeded, type was truncated

#include <vector>
#include <memory>

#include "core/data_access_interface/tis_agent/src/PredefinedMessageLibraryHelper.h"
#include "core/data_access_interface/tis_agent/src/PredefinedMessageLibraryTable.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

	PredefinedMessageLibraryHelper::PredefinedMessageLibraryHelper( unsigned long version,
                                                                    const std::string& libraryType,
                                                                    std::vector<unsigned char>& libraryFileContent,
																	EDataTypes conntype)
		: m_version( version ),
          m_libraryType( libraryType ),
          m_key(0),
		  m_isValidData(true),
          m_isNew(true),
          m_libraryFileContentPopulated(false),
		  m_connType(conntype)
	{
		m_libraryFileContent = libraryFileContent;
        m_libraryFileContentPopulated = true;

		// write this to the database straight away!
		addNewMessageLibraryData();
	}


	PredefinedMessageLibraryHelper::PredefinedMessageLibraryHelper(EDataTypes conntype)
		: m_isValidData( false ),
          m_key( 0 ),
          m_isNew( true ),
          m_libraryFileContentPopulated(false),
		  m_connType(conntype)
	{
	}


    PredefinedMessageLibraryHelper::PredefinedMessageLibraryHelper( unsigned long predefinedMessageLibraryKey, EDataTypes conntype)
        : m_isValidData( false ),
          m_key( predefinedMessageLibraryKey ),
          m_isNew( false ),
          m_libraryFileContentPopulated(false),
		  m_connType(conntype)
    {
	}


    unsigned long PredefinedMessageLibraryHelper::getKey()
    {
        // This method cannot be called until the key has been set.

        TA_ASSERT( 0 != m_key, "The message library key has not been initialised." );
        return m_key;
    }


	unsigned long PredefinedMessageLibraryHelper::getVersion()
	{
       TA_ASSERT(!m_isNew,"The data must be written to the database before the version can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_version;
	}


	std::string PredefinedMessageLibraryHelper::getLibraryType()
	{
       TA_ASSERT(!m_isNew,"The data must be written to the database before the library type can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_libraryType;
	}


	void PredefinedMessageLibraryHelper::getLibraryFileContent( std::vector<unsigned char>& fileContent )
	{
       TA_ASSERT(!m_isNew,"The data must be written to the database before the library file content can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        // the library file content is not used much and it is slow
        // so only load it when necessary
        if (!m_libraryFileContentPopulated && !m_isNew)
        {
            loadLibraryFileContent();
        }

		fileContent = m_libraryFileContent;
	}


	IPredefinedMessageLibrary::PredefinedMessageList PredefinedMessageLibraryHelper::getMessages()
	{
       TA_ASSERT(!m_isNew,"The data must be written to the database before the messages can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_messageList;
	}


    PredefinedMessage* PredefinedMessageLibraryHelper::getMessage(int librarySection, unsigned short messageTag)
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the messages can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        // now find the matching message
        PredefinedMessage* matchingMessage = NULL;


        for (IPredefinedMessageLibrary::PredefinedMessageList::iterator iter = m_messageList.begin();
        iter != m_messageList.end(); iter++)
        {
            if ( ((*iter)->messageTag == messageTag) &&
                 ((*iter)->librarySection == librarySection) )
            {
                // found!
                matchingMessage = (*iter);
                break;
            }
        }


        return matchingMessage;
    }


	DisplayAttributes PredefinedMessageLibraryHelper::getDefaultSTISAttributes()
	{
        TA_ASSERT(!m_isNew,"The data must be written to the database before the stis attributes can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        // if there are none, then this will return defaults
		return m_stisAttributes;

	}

	LCDAttributesSTIS PredefinedMessageLibraryHelper::getDefaultSTISLcdAttributes()
	{
        TA_ASSERT(!m_isNew,"The data must be written to the database before the stis lcd attributes can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        // if there are none, then this will return defaults
		return m_stisLcdAttributes;
	}

	LEDAttributes PredefinedMessageLibraryHelper::getDefaultSTISLedAttributes()
	{
        TA_ASSERT(!m_isNew,"The data must be written to the database before the stis led attributes can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        // if there are none, then this will return defaults
		return m_stisLedAttributes;
	}

    void PredefinedMessageLibraryHelper::addMessage( const PredefinedMessage& theMessage )
	{
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(m_connType, Write);
        
		// run a query to get a new primary key for this message
        /*std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::PREDEFINED_MESSAGE_PKEY_SEQUENCE;
        selectSql << " from DUAL";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_Oracle_SELECT_38551);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38551);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38551);

        std::vector< std::string > columnNames;
        columnNames.push_back( "NEXTVAL" );
        
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( strSql, columnNames ) );

        // Check that one entry was found
        if ( 1 != data->getNumRows() )
        {
            // unable to get a new key, throw an exception
            std::stringstream error;
            error << "Cannot insert predefined message into ";
            error << TA_TISAgentDAI::PREDEFINED_MESSAGE_TABLE_NAME << ".";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // get the pkey to use
        unsigned long messageKey = data->getUnsignedLongData( 0, "NEXTVAL" );

        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Got a unique pkey for new predefined message. Inserting with key %d", messageKey);

        // Now we can formulate our SQL string
		/*std::ostringstream formatSQL;
		formatSQL.str("");
        formatSQL << "insert into ";
		formatSQL << TA_TISAgentDAI::PREDEFINED_MESSAGE_TABLE_NAME;
		formatSQL << " (";
		formatSQL << TA_TISAgentDAI::PREDEFINED_MESSAGE_PKEY_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::MESSAGE_TAG_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::LIBRARY_SECTION_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::DESCRIPTION_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::PRIORITY_COLUMN << ",";
        formatSQL << TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::MESSAGE_CONTENT_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::ENGLISH_ONLY_COLUMN  << ") ";
		formatSQL << "values (";
		formatSQL << messageKey << ",";
		formatSQL << m_key << ",";
		formatSQL << theMessage.messageTag << ",";
		formatSQL << theMessage.librarySection << ",";
		formatSQL << "'" << databaseConnection->escapeInsertString(theMessage.description) << "',";
		formatSQL << theMessage.priority << ",";
        formatSQL << theMessage.repeatInterval << ",";
		formatSQL << "'" << databaseConnection->escapeInsertString(theMessage.message) << "',";
		formatSQL << theMessage.englishOnly << ")";*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_INSERT_38801,
//		strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_INSERT_38801,

		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_INSERT_38801,
			messageKey, m_key, theMessage.messageTag, theMessage.librarySection, databaseConnection->escapeInsertString(theMessage.description),
			theMessage.priority, theMessage.repeatInterval, databaseConnection->escapeInsertString(theMessage.message), theMessage.englishOnly);

        databaseConnection->executeModification( strSql );

		m_isNew = false;
	}

	void PredefinedMessageLibraryHelper::setDefaultSTISDisplayAttributes ( const DisplayAttributes& stisAttributes,
                                                   const LCDAttributesSTIS& stisLcdAttributes,
                                                   const LEDAttributes& stisLedAttributes )
	{
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(m_connType, Write);

        // check if the default attributes exist in the database
        // for this message library
       /* std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PKEY_COLUMN;
        selectSql << " from " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE;
        selectSql << " where " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PARENT_LIBRARY_COLUMN << " = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_SELECT_38504,m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38504,m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38504,m_key);

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PKEY_COLUMN );
        
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( strSql, columnNames ) );

        // if no entries were found, add one
		if ( 0 != data->getNumRows())
		{
			//delete old entries		
// 			std::ostringstream formatSQL;
// 			formatSQL.str("");
// 			formatSQL << "delete from " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE;
// 			formatSQL << " where " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PARENT_LIBRARY_COLUMN;
// 			formatSQL << " = " << m_key;		 
				
//			std::string strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38904, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38904, m_key);
			databaseConnection->executeModification( strSql );
		}

         
		// add new entry
		std::ostringstream formatSQL;
		formatSQL.str("");
		formatSQL << "insert into ";
		formatSQL << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE;
		formatSQL << " (";
		formatSQL << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PKEY_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PARENT_LIBRARY_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::STIS_DEFAULT_DISPLAY_MODE_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::STIS_DEFAULT_SCROLL_SPEED_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::STIS_DEFAULT_REPEAT_INTERVAL_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::STIS_DEFAULT_DISPLAY_TIME_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::STIS_DEFAULT_JUSTIFICATION_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_SIZE_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_COLOUR_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::STIS_LCD_DEFAULT_FONT_SIZE_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::STIS_LCD_DEFAULT_FONT_COLOUR_COLUMN; 
		formatSQL << ") ";
		formatSQL << "values (";
		formatSQL << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PKEY_SEQUENCE << ",";
		formatSQL << m_key << ",";
		formatSQL << stisAttributes.displayMode << ",";
		formatSQL << stisAttributes.scrollSpeed << ",";
		formatSQL << stisAttributes.repeatInterval << ",";
		formatSQL << stisAttributes.displayTime << ",";
		formatSQL << stisAttributes.justification << ",";
		formatSQL << stisLedAttributes.fontSize << ",";
		formatSQL << stisLedAttributes.fontColour << ",";
		formatSQL << stisLcdAttributes.fontSize << ",";
		formatSQL << stisLcdAttributes.fontColour;
		formatSQL << ")";
		 
		//strSql="";
//		strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_INSERT_38853,

		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_INSERT_38853,
			m_key, stisAttributes.displayMode, stisAttributes.scrollSpeed, stisAttributes.repeatInterval,stisAttributes.displayTime,
			stisAttributes.justification,stisLedAttributes.fontSize,stisLedAttributes.fontColour,stisLcdAttributes.fontSize,
			stisLcdAttributes.fontColour);
        databaseConnection->executeModification( strSql );   
         
        
		m_isNew = false;
    }


    void PredefinedMessageLibraryHelper::invalidate()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "PredefinedMessageLibraryHelper::invalidate" );

        // This method cannot be called until the pkey has been set

        TA_ASSERT( !m_isNew, "Attempted to call invalidate() on a new PredefinedMessageLibrary entry.");
        
        m_isValidData = false;

        LOG_FUNCTION_EXIT( SourceInfo, "PredefinedMessageLibraryHelper::invalidate" );
    }


    void PredefinedMessageLibraryHelper::reload()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "PredefinedMessageLibraryHelper::reload" );
        
        // Ensure the Predefined message library entry has already been written to the database. 
        // It doesn't make any sense to reload a predefined message library entry that doesn't
        // exist in the database.
        
        TA_ASSERT( 0 != m_key, 
            "This MessageLibrary does not yet exist in the database. Therefore it cannot be reloaded.");
  
		m_messageList.clear();
        
        // Get a connection to the database.
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(m_connType, Read);

		// check for existing libraries that match version and type
      /*  std::ostringstream sql;
        sql << "select ";
        sql << TA_TISAgentDAI::VERSION_COLUMN << ", ";
        sql << TA_TISAgentDAI::LIBRARY_TYPE_COLUMN;
        sql << " FROM " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME;
        sql << " WHERE " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN << " = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_SELECT_38505,
//		std::string strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38505,
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38505,
			m_key);
 
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back( TA_TISAgentDAI::VERSION_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::LIBRARY_TYPE_COLUMN );

        // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
		std::auto_ptr< TA_Base_Core::IData > data;
        data = std::auto_ptr< TA_Base_Core::IData >(databaseConnection->executeQuery(strSql,columnNames) );

		if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            std::stringstream error;
			error << "No message library found for pkey = " << m_key;
            
            TA_THROW(TA_Base_Core::DataException(error.str().c_str(), TA_Base_Core::DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            std::stringstream error;
			error << "More than one Predefined Message Library found with pkey = " << m_key;

			TA_THROW(TA_Base_Core::DataException(error.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_version = data->getUnsignedLongData(0,TA_TISAgentDAI::VERSION_COLUMN);
        m_libraryType = data->getStringData(0,TA_TISAgentDAI::LIBRARY_TYPE_COLUMN);

		// get the messages that form this library
		//std::stringstream messageSql;
		columnNames.clear();

	/*	messageSql << "SELECT ";
        messageSql << TA_TISAgentDAI::PREDEFINED_MESSAGE_PKEY_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::PARENT_LIBRARY_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::MESSAGE_TAG_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::LIBRARY_SECTION_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::DESCRIPTION_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::PRIORITY_COLUMN << ", ";
        messageSql << TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::MESSAGE_CONTENT_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::ENGLISH_ONLY_COLUMN;
		messageSql << " FROM " << TA_TISAgentDAI::PREDEFINED_MESSAGE_TABLE_NAME;
		messageSql << " WHERE " << TA_TISAgentDAI::PARENT_LIBRARY_COLUMN << " = " << m_key;
        messageSql << " ORDER BY " << TA_TISAgentDAI::LIBRARY_SECTION_COLUMN << ", " << TA_TISAgentDAI::MESSAGE_TAG_COLUMN;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_SELECT_38506,
//		strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38506,

		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38506,
			m_key);

		// Set up the column names for data we are expecting to receive
		columnNames.push_back( TA_TISAgentDAI::PREDEFINED_MESSAGE_PKEY_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::PARENT_LIBRARY_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::MESSAGE_TAG_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::LIBRARY_SECTION_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::DESCRIPTION_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::PRIORITY_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::MESSAGE_CONTENT_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::ENGLISH_ONLY_COLUMN );

		int size = columnNames.size();

		TA_Base_Core::IData* messagedata = databaseConnection->executeQuery(strSql, columnNames);

        try
        {
            bool moreData = true;
            while (moreData)
            {
		        for ( unsigned long i = 0; i < messagedata->getNumRows(); ++i )
		        {   
			        PredefinedMessage* messageData = new PredefinedMessage();
		        
			        messageData->messageTag = messagedata->getIntegerData( i, TA_TISAgentDAI::MESSAGE_TAG_COLUMN );
			        messageData->librarySection = messagedata->getIntegerData( i, TA_TISAgentDAI::LIBRARY_SECTION_COLUMN );
			        messageData->description = messagedata->getStringData( i, TA_TISAgentDAI::DESCRIPTION_COLUMN );
			        messageData->priority = messagedata->getIntegerData( i, TA_TISAgentDAI::PRIORITY_COLUMN );
                    messageData->repeatInterval = messagedata->getIntegerData( i, TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN );
			        messageData->message = messagedata->getStringData( i, TA_TISAgentDAI::MESSAGE_CONTENT_COLUMN ); 
			        messageData->englishOnly = (0 != messagedata->getIntegerData( i, TA_TISAgentDAI::ENGLISH_ONLY_COLUMN ) ); 

			        m_messageList.push_back( messageData );
		        }
            
                // clean up used data object
                delete messagedata;
                messagedata = NULL;

                // try to read more data
                moreData = databaseConnection->moreData(messagedata);
            }
        }
        catch(...)
        {
            // clean up in case an exception was thrown midway
            if (messagedata != NULL)
            {
                delete messagedata;
                messagedata = NULL;
            }

            // throw on
            throw;
        }


		do
		{
			if(m_libraryType == "TTIS")
			{
				break; // 955, TTIS not have default attributes
			}
			// read default attributes
			std::stringstream attributeSql;
			attributeSql << "select ";
			 
			attributeSql << TA_TISAgentDAI::STIS_DEFAULT_DISPLAY_MODE_COLUMN << ",";
			attributeSql << TA_TISAgentDAI::STIS_DEFAULT_SCROLL_SPEED_COLUMN << ",";
			attributeSql << TA_TISAgentDAI::STIS_DEFAULT_REPEAT_INTERVAL_COLUMN << ",";
			attributeSql << TA_TISAgentDAI::STIS_DEFAULT_DISPLAY_TIME_COLUMN << ",";
			attributeSql << TA_TISAgentDAI::STIS_DEFAULT_JUSTIFICATION_COLUMN << ",";
			attributeSql << TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_SIZE_COLUMN << ",";
			attributeSql << TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_COLOUR_COLUMN << ",";
			attributeSql << TA_TISAgentDAI::STIS_LCD_DEFAULT_FONT_SIZE_COLUMN << ",";
			attributeSql << TA_TISAgentDAI::STIS_LCD_DEFAULT_FONT_COLOUR_COLUMN; 
			attributeSql << " from " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE;
			attributeSql << " where " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PARENT_LIBRARY_COLUMN << " = " << m_key;
			columnNames.clear();
			columnNames.push_back( TA_TISAgentDAI::STIS_DEFAULT_DISPLAY_MODE_COLUMN );
			columnNames.push_back( TA_TISAgentDAI::STIS_DEFAULT_SCROLL_SPEED_COLUMN );
			columnNames.push_back( TA_TISAgentDAI::STIS_DEFAULT_REPEAT_INTERVAL_COLUMN );
			columnNames.push_back( TA_TISAgentDAI::STIS_DEFAULT_DISPLAY_TIME_COLUMN );
			columnNames.push_back( TA_TISAgentDAI::STIS_DEFAULT_JUSTIFICATION_COLUMN );
			columnNames.push_back( TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_SIZE_COLUMN );
			columnNames.push_back( TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_COLOUR_COLUMN );
			columnNames.push_back( TA_TISAgentDAI::STIS_LCD_DEFAULT_FONT_SIZE_COLUMN );
			columnNames.push_back( TA_TISAgentDAI::STIS_LCD_DEFAULT_FONT_COLOUR_COLUMN ); 
			
//			strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38509, m_key);

		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38509, m_key);

			std::auto_ptr< TA_Base_Core::IData > attributeData = std::auto_ptr< TA_Base_Core::IData >( 
				databaseConnection->executeQuery( strSql, columnNames ) );

			if ( 1 == attributeData->getNumRows() )
			{

				m_stisAttributes.displayMode = static_cast<EDisplayMode>
					( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_DEFAULT_DISPLAY_MODE_COLUMN ) );
				m_stisAttributes.scrollSpeed = static_cast<EScrollSpeed> 
					( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_DEFAULT_SCROLL_SPEED_COLUMN ) );
				m_stisAttributes.repeatInterval = 
					attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_DEFAULT_REPEAT_INTERVAL_COLUMN );
				m_stisAttributes.displayTime = 
					attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_DEFAULT_DISPLAY_TIME_COLUMN );
				m_stisAttributes.justification = static_cast<EJustification>
					( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_DEFAULT_JUSTIFICATION_COLUMN ) );
				m_stisLedAttributes.fontSize = static_cast<ELEDFontSize>
					( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_SIZE_COLUMN) );
				m_stisLedAttributes.fontColour = static_cast<ELEDFontColour>
					(attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_COLOUR_COLUMN ) );
				m_stisLcdAttributes.fontSize = static_cast<ELcdFontSize>
					( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_LCD_DEFAULT_FONT_SIZE_COLUMN ) );
				m_stisLcdAttributes.fontColour = static_cast<ELcdFontColour>
					( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_LCD_DEFAULT_FONT_COLOUR_COLUMN ) );
			}
			// otherwise there are none
			else if ( 0 == attributeData->getNumRows() )
			{
				std::stringstream error;
				error << "No default adhoc attributes found in "
					  << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE
					  << " for pre-defined message library key = " << m_key;
	            
				TA_THROW(TA_Base_Core::DataException(error.str().c_str(), TA_Base_Core::DataException::NO_VALUE,""));
			}
			// or multiple
			else
			{
				std::stringstream error;
				error << "More than one default adhoc attributes entry found in "
					  << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE
					  << " for pre-defined message library key = " << m_key;

				TA_THROW(TA_Base_Core::DataException(error.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE,""));
			}
		} while(false);

        // The data is now valid.
        m_isValidData = true;

        LOG_FUNCTION_EXIT( SourceInfo, "PredefinedMessageLibraryHelper::reload" );
    }


    void PredefinedMessageLibraryHelper::loadLibraryFileContent()
    {
        // Get a connection to the database.
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(m_connType, Read);

        std::stringstream whereSql;
		whereSql << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN;
		whereSql << " = " << m_key;

		// Get the predefined message library raw file content
		databaseConnection->readBLOB( TA_TISAgentDAI::LIBRARY_FILE_CONTENT_COLUMN, 
			                          TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME,
			                          whereSql.str(),
                                      m_libraryFileContent );

        m_libraryFileContentPopulated = true;
    }


	void PredefinedMessageLibraryHelper::addNewMessageLibraryData()
	{
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(m_connType, Write);
   
		// run a query to get a new primary key for this message
      /*  std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_SEQUENCE;
        selectSql << " from DUAL";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_Oracle_SELECT_38552);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38552);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38552);

        std::vector< std::string > columnNames;
        columnNames.push_back( "NEXTVAL" );
        
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( strSql, columnNames ) );

        // Check that one entry was found
        if ( 1 != data->getNumRows() )
        {
            // unable to get a new key, throw an exception
            std::stringstream error;
            error << "Cannot insert predefined message into ";
            error << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME << ".";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // get the pkey to use
        m_key = data->getUnsignedLongData( 0, "NEXTVAL" );

        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Got a unique pkey for new predefined message library. Inserting with key %d", m_key);

        // Now we can formulate our SQL string
		/*std::ostringstream formatSQL;
		formatSQL.str("");
        formatSQL << "insert into ";
		formatSQL << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME;
		formatSQL << "(";
		formatSQL << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::VERSION_COLUMN << ",";
        formatSQL << TA_TISAgentDAI::LIBRARY_TYPE_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::LIBRARY_FILE_CONTENT_COLUMN << ") ";
		formatSQL << "values (";
		formatSQL << m_key << ",";
		formatSQL << m_version << ",";
		formatSQL << "'" << databaseConnection->escapeQueryString(m_libraryType) << "',";
        formatSQL <<  "EMPTY_BLOB())";  */    // must set the blob to empty (not null) or oci cant open it.
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_Oracle_INSERT_38852,
//		strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38852,

		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_SELECT_38852,
			m_key, m_version, databaseConnection->escapeQueryString(m_libraryType));

        databaseConnection->executeModification( strSql );

		std::stringstream whereSql;
		whereSql << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN;
		whereSql << " = " << m_key;

		// Write the predefined message library raw file content
		databaseConnection->writeBLOB( TA_TISAgentDAI::LIBRARY_FILE_CONTENT_COLUMN, 
			TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME,
			whereSql.str(), m_libraryFileContent );

		m_isNew = false;
	}


	void PredefinedMessageLibraryHelper::deleteThisLibrary()
	{
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(m_connType, Write);
        
       /* std::ostringstream formatSQL;
		formatSQL.str("");
        formatSQL << "delete from " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE;
        formatSQL << " where " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PARENT_LIBRARY_COLUMN;
        formatSQL << " = " << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_DELETE_38904, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38904, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38904, m_key);

        databaseConnection->executeModification( strSql );

        // Then delete all the messages that 'belong' to this library
	/*	formatSQL.str("");
        formatSQL << "delete from " << TA_TISAgentDAI::PREDEFINED_MESSAGE_TABLE_NAME;
        formatSQL << " where " << TA_TISAgentDAI::PARENT_LIBRARY_COLUMN << " = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_DELETE_38905, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38905, m_key);

		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38905, m_key);
 
        databaseConnection->executeModification( strSql );

		// Then delete the message library itself
	/*	formatSQL.str("");
        formatSQL << "delete from " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME;
        formatSQL << " where " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN << " = " << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ti_predefined_message_library_STD_DELETE_38906, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38906, m_key);

		databaseConnection->prepareSQLStatement(strSql, TI_PREDEFINED_MESSAGE_LIBRARY_DELETE_38906, m_key);

		databaseConnection->executeModification( strSql );
	}



  
} // closes TA_Base_Core

