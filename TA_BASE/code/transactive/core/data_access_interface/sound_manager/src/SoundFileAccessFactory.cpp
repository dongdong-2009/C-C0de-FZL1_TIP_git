/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/sound_manager/src/SoundFileAccessFactory.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * SoundFileAccessFactory is a singleton that is used
  * to create ISoundFile and IConfigSoundFile objects.
  *
  */

#ifdef _MSC_VER
	#pragma warning(disable:4786)
#endif

#include "ConfigSoundFile.h"
#include "SoundFileAccessFactory.h"
#include "SoundFile.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/utilities/src/TAAssert.h"

#include <string>
#include <vector>
#include <sstream>

namespace TA_Base_Core
{

    SoundFileAccessFactory* SoundFileAccessFactory::m_instance = 0;


	SoundFileAccessFactory& SoundFileAccessFactory::getInstance()
	{
        if (m_instance == NULL)
        {
            m_instance = new SoundFileAccessFactory();
        }
        return *m_instance;
	}


    std::vector<ISoundFile*> SoundFileAccessFactory::getAllSoundFiles( bool readWrite /*=false*/ )
    {
        // Get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sound_Cd, Read);

        // Get the priorities of the sounds belonging to this category.
	/*	std::ostringstream sql;
        sql << "SELECT soscat_id, priority FROM so_sound_file";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SO_SOUND_FILE_STD_SELECT_38001);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SO_SOUND_FILE_SELECT_38001);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SO_SOUND_FILE_SELECT_38001);

        // Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
        std::string categoryColumn = "category";
		std::string priorityColumn = "priority";
        columnNames.push_back(categoryColumn);
        columnNames.push_back(priorityColumn);

        // Execute the query. 
        IData* data = databaseConnection->executeQuery( strSql, columnNames );

        // Create a vector of ISoundFile pointers, and populate it.
        std::vector<ISoundFile*> result;
        int numRows = data->getNumRows();
        for ( int i = 0; i < numRows; i++ )
        {
            if ( readWrite )
            {
                result.push_back(new ConfigSoundFile(data->getUnsignedLongData(i, categoryColumn),
                                                     data->getUnsignedLongData(i, priorityColumn)));
            }
            else
            {
                result.push_back(new SoundFile(data->getUnsignedLongData(i, categoryColumn),
                                               data->getUnsignedLongData(i, priorityColumn)));

            }
        }
        return result;
    }


	std::vector<ISoundFile*> SoundFileAccessFactory::getSoundFileByCategory(unsigned long categoryKey)
	{
        // Get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sound_Cd, Read);

        // Get the priorities of the sounds belonging to this category.
	/*	std::ostringstream sql;
        sql << "SELECT priority FROM so_sound_file where "
            << "soscat_id = " << categoryKey;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), SO_SOUND_FILE_STD_SELECT_38002, categoryKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(SO_SOUND_FILE_SELECT_38002, categoryKey);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, SO_SOUND_FILE_SELECT_38002, categoryKey);

        // Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		std::string priorityColumn = "priority";
        columnNames.push_back(priorityColumn);

        // Execute the query. 
        IData* data = databaseConnection->executeQuery(strSql, columnNames);

        // Create a vector of ISoundFile pointers, and populate it.
        std::vector<ISoundFile*> result;
        int numRows = data->getNumRows();
        for (int i = 0; i < numRows; i++)
        {
            result.push_back(new SoundFile(categoryKey,
                               data->getUnsignedLongData(i, priorityColumn)));
        }
        return result;
	}


    IConfigSoundFile* SoundFileAccessFactory::createSoundFile()
	{
		return new ConfigSoundFile();
	}


    IConfigSoundFile* SoundFileAccessFactory::copySoundFile( const IConfigSoundFile* original )
    {
        FUNCTION_ENTRY("copySoundFile");

        TA_ASSERT(original !=  NULL, "The Sound File to copy was NULL");
        
        const ConfigSoundFile* cast = dynamic_cast<const ConfigSoundFile*>(original);

        TA_ASSERT(cast != NULL, "Sound File passed could not be converted into a ConfigSoundFile");

        FUNCTION_EXIT;
        return new ConfigSoundFile( *cast );
    }

} // TA_Base_Core
