/**
  * The source code in this file is the property of
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/tis_agent/src/ConfigPredefinedMessageLibraryData.cpp $
  * @author Robin Ashcroft
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/21 18:27:55 $
  * Last modified by:  $Author: hongzhi.zhang $
  *
  * This class contains the database retrievals and writes for Predefined Message Libraries
  * for the Traveller Information System (TIS) Agent
  * It implements the IConfigPredefinedMessageLibraryData interface.
  */

#pragma warning( disable : 4786 )

#include <string>
#include <sstream>

#include "core/data_access_interface/tis_agent/src/ConfigPredefinedMessageLibraryData.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/utilities/src/TAAssert.h"


namespace TA_Base_Core
{

    //
    // CameraPresetsData
    //
    ConfigPredefinedMessageLibraryData::ConfigPredefinedMessageLibraryData( EDataTypes conntype/*Tis_Cd*/ )
            : m_isValidData( false )
    {
        m_messageLibraryHelper = new PredefinedMessageLibraryHelper( conntype );
    }


    ConfigPredefinedMessageLibraryData::ConfigPredefinedMessageLibraryData( unsigned long version, const std::string& libraryType,
                                                                            std::vector<unsigned char>& libraryFileContent, EDataTypes conntype/*Tis_Cd*/ ) : m_isValidData( true )
    {
        m_messageLibraryHelper = new PredefinedMessageLibraryHelper( version, libraryType, libraryFileContent, conntype );

    }
    ConfigPredefinedMessageLibraryData::ConfigPredefinedMessageLibraryData( unsigned long key, EDataTypes conntyped )
    {
        m_messageLibraryHelper = new PredefinedMessageLibraryHelper( key, conntyped );
    }


    ConfigPredefinedMessageLibraryData::~ConfigPredefinedMessageLibraryData()
    {
        if ( m_messageLibraryHelper != NULL )
        {
            delete m_messageLibraryHelper;
            m_messageLibraryHelper = NULL;
        }

    }


    unsigned long ConfigPredefinedMessageLibraryData::getKey()
    {
        TA_ASSERT( m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null." );

        return m_messageLibraryHelper->getKey();
    }


    unsigned long ConfigPredefinedMessageLibraryData::getVersion()
    {
        TA_ASSERT( m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null." );

        return m_messageLibraryHelper->getVersion();
    }


    std::string ConfigPredefinedMessageLibraryData::getLibraryType()
    {
        TA_ASSERT( m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null." );

        return m_messageLibraryHelper->getLibraryType();
    }


    void ConfigPredefinedMessageLibraryData::getLibraryFileContent( std::vector<unsigned char>& fileContent )
    {
        TA_ASSERT( m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null." );

        m_messageLibraryHelper->getLibraryFileContent( fileContent );
    }


    void ConfigPredefinedMessageLibraryData::invalidate()
    {
        TA_ASSERT( m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null." );
        m_messageLibraryHelper->invalidate();
    }


    void ConfigPredefinedMessageLibraryData::addMessage( const PredefinedMessage& theMessage )
    {
        TA_ASSERT( m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null." );

        m_messageLibraryHelper->addMessage( theMessage );
    }

    IPredefinedMessageLibrary::PredefinedMessageList ConfigPredefinedMessageLibraryData::getMessages()
    {
        TA_ASSERT( m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null." );

        return m_messageLibraryHelper->getMessages();
    }

    PredefinedMessage* ConfigPredefinedMessageLibraryData::getMessage( int librarySection, unsigned short messageTag )
    {
        TA_ASSERT( m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null." );

        return m_messageLibraryHelper->getMessage( librarySection, messageTag );
    }



    DisplayAttributes ConfigPredefinedMessageLibraryData::getDefaultSTISAttributes()
    {
        TA_ASSERT( m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null." );
        return m_messageLibraryHelper->getDefaultSTISAttributes();
    }

    void ConfigPredefinedMessageLibraryData::setDefaultSTISDisplayAttributes( const DisplayAttributes& stisAttributes,
                                                                              const LCDAttributesSTIS& stisLcdAttributes,
                                                                              const LEDAttributes& stisLedAttributes )
    {
        TA_ASSERT( m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null." );
        m_messageLibraryHelper->setDefaultSTISDisplayAttributes( stisAttributes, stisLcdAttributes, stisLedAttributes );
    }


    LCDAttributesSTIS ConfigPredefinedMessageLibraryData::getDefaultSTISLcdAttributes()
    {
        TA_ASSERT( m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null." );
        return m_messageLibraryHelper->getDefaultSTISLcdAttributes();
    }

    LEDAttributes ConfigPredefinedMessageLibraryData::getDefaultSTISLedAttributes()
    {
        TA_ASSERT( m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null." );
        return m_messageLibraryHelper->getDefaultSTISLedAttributes();
    }

    void ConfigPredefinedMessageLibraryData::deleteThisLibrary()
    {
        TA_ASSERT( m_messageLibraryHelper != NULL, "The MessageLibraryHelper pointer is NULL" );

        m_messageLibraryHelper->deleteThisLibrary();
    }


    bool ConfigPredefinedMessageLibraryData::isNew()
    {
        TA_ASSERT( m_messageLibraryHelper != NULL, "The MessageLibraryHelper pointer is NULL" );

        return m_messageLibraryHelper->isNew();
    }


    void ConfigPredefinedMessageLibraryData::applyChanges()
    {
        TA_ASSERT( m_messageLibraryHelper != NULL, "The MessageLibraryHelper pointer is NULL" );

//        m_messageLibraryHelper->applyChanges();
        m_messageLibraryDataChanges.clear();
    }



} //end namespace TA_Base_Core
