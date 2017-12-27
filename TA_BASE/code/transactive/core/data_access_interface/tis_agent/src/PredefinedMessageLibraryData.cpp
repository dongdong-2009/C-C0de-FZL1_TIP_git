/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/tis_agent/src/PredefinedMessageLibraryData.cpp $
  * @author Robin Ashcroft
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/21 18:27:55 $
  * Last modified by:  $Author: hongzhi.zhang $
  * 
  * This class contains the database retrievals and writes for Train Time Schedules
  * for the Traveller Information System (TIS) Agent
  * It implements the ITrainTimeSchedule interface.
  */

#pragma warning( disable : 4786 )

#include <string>
#include <sstream>

#include "core/data_access_interface/tis_agent/src/PredefinedMessageLibraryData.h"

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
    PredefinedMessageLibraryData::PredefinedMessageLibraryData( unsigned long pKey )
        : m_isValidData( false )
    {
        m_messageLibraryHelper = new PredefinedMessageLibraryHelper( pKey );
    }  


	PredefinedMessageLibraryData::~PredefinedMessageLibraryData()
	{
		if( m_messageLibraryHelper != NULL )
		{
			delete m_messageLibraryHelper;
			m_messageLibraryHelper = NULL;
		}

	}
	

	unsigned long PredefinedMessageLibraryData::getKey()
	{
		TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");

		return m_messageLibraryHelper->getKey();
	}


	unsigned long PredefinedMessageLibraryData::getVersion()
	{
		TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");
	
		return m_messageLibraryHelper->getVersion();
	}


	std::string PredefinedMessageLibraryData::getLibraryType()
	{
		TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");

		return m_messageLibraryHelper->getLibraryType();
	}


	void PredefinedMessageLibraryData::getLibraryFileContent( std::vector<unsigned char>& fileContent )
	{
		TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");

		m_messageLibraryHelper->getLibraryFileContent( fileContent );
	}


    IPredefinedMessageLibrary::PredefinedMessageList PredefinedMessageLibraryData::getMessages()
	{
		TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");
		
		return m_messageLibraryHelper->getMessages();
	}


    PredefinedMessage* PredefinedMessageLibraryData::getMessage(int librarySection, unsigned short messageTag)
    {
		TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");
		
		return m_messageLibraryHelper->getMessage(librarySection, messageTag);
    }

	DisplayAttributes PredefinedMessageLibraryData::getDefaultSTISAttributes()
	{
        TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");
		return m_messageLibraryHelper->getDefaultSTISAttributes();
	}


	void PredefinedMessageLibraryData::deleteThisLibrary()
	{
		TA_ASSERT(m_messageLibraryHelper != NULL, "The MessageLibraryHelper pointer is NULL");

		m_messageLibraryHelper->deleteThisLibrary();
	}


    void PredefinedMessageLibraryData::invalidate()
	{
        TA_ASSERT(m_messageLibraryHelper != NULL,"The MessageLibraryHelper pointer is null.");
        m_messageLibraryHelper->invalidate();
	}
    


} //end namespace TA_Base_Core
