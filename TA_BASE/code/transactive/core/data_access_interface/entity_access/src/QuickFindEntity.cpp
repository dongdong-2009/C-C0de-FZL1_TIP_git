/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/QuickFindEntity.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * QuickFindEntity is an implementation of IQuickFindEntity. It holds the data specific to an entity that
  * is a device. This device allows quick launch of its controller, associated display and/or associated
  * camera image from the Control Station.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/entity_access/src/QuickFindEntity.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;


namespace TA_Base_Core
{
    const std::string QuickFindEntity::CAMERA_ASSOCIATION = "CameraAssociation";
    const std::string QuickFindEntity::CAMERA_PRESET_ASSOCIATION = "CameraPresetAssociation";
    const std::string QuickFindEntity::ALARM_DISPLAY = "AlarmDisplay";


    QuickFindEntity::QuickFindEntity(const unsigned long key,
                                     const std::string& name,
                                     const std::string& description,
                                     const unsigned long guiApp)
        : m_key(key),
          m_name(name),
          m_description(description),
          m_guiApplicationKey(guiApp),
          m_alarmDisplay(""),
          m_cameraAssociation(""),
          m_cameraPresetAssociation(0),
          m_isValidData(false)
    {
    }


    QuickFindEntity::~QuickFindEntity()
    {
    }


    unsigned long QuickFindEntity::getKey()
    {
        return m_key;
    }


    std::string QuickFindEntity::getName()
    {
        // For name we only need to reload if an actual invalidate() has been called
        // If this is the case then the name will be empty.
        if( (!m_isValidData) && (m_name.empty()) )
        {
            reload();
        }

        return m_name;
    }


    std::string QuickFindEntity::getDescription()
    {
        // For description we only need to reload if an actual invalidate() has been called
        // If this is the case then the name will be empty.
        if( (!m_isValidData) && (m_name.empty()) )
        {
            reload();
        }

        return m_description;
    }

    
    unsigned long QuickFindEntity::getGuiApplicationKey()
    {
        // For guiApplication we only need to reload if an actual invalidate() has been called
        // If this is the case then the name will be empty.
        if( (!m_isValidData) && (m_name.empty()) )
        {
            reload();
        }

        return m_guiApplicationKey;
    }


    std::string QuickFindEntity::getAlarmDisplay()
    {
        if (!m_isValidData)
        {
            reload();
        }

        return m_alarmDisplay;
    }


    std::string QuickFindEntity::getCameraAssociation()
    {
        if (!m_isValidData)
        {
            reload();
        }

        return m_cameraAssociation;
    }


    unsigned long QuickFindEntity::getCameraPresetAssociation()
    {
        if (!m_isValidData)
        {
            reload();
        }

        return m_cameraPresetAssociation;
    }


    void QuickFindEntity::invalidate()
    {
        m_isValidData = false;

        // First reset all the members so if we don't retrieve a value they are back to their defaults.
      m_name = "";
      m_guiApplicationKey = 0;
      m_alarmDisplay = "";
      m_cameraAssociation = "";
      m_cameraPresetAssociation = 0;
    }


    void QuickFindEntity::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "reload" );

        // get a connection to the database
            IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        // create the SQL string to retrieve the data of the entity
        // based upon the key
       /* std::ostringstream sql;
        sql << "select entityparameter.name, entityparametervalue.value from entityparameter, entityparametervalue "
            << "where entityparameter.pkey = entityparametervalue.parameterkey and entityparametervalue.entitykey = "
            << m_key;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparameter_STD_SELECT_20506, m_key);
//		std::string strSql  = databaseConnection->prepareSQLStatement(ENTITYPARAMETER_SELECT_20506, m_key);
		SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, ENTITYPARAMETER_SELECT_20506, m_key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string nameColumn = "NAME";
        std::string valueColumn = "VALUE";
        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);
        columnNames.push_back(valueColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(strSql,columnNames);

        // Using this data we can set the CameraPreset, CameraPresetAssociation and AlarmDisplay

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        for (unsigned long i = 0;i < data->getNumRows(); ++i)
        {
            // get the parameter name that was retrieved
            std::string parameterName = data->getStringData(i,nameColumn);

            // now work out which parameter this is
            if (CAMERA_ASSOCIATION == parameterName)
            {
                m_cameraAssociation = data->getStringData(i, valueColumn);
            }
            else if (CAMERA_PRESET_ASSOCIATION == parameterName)
            {
                m_cameraPresetAssociation = data->getUnsignedLongData(i, valueColumn);
            }
            else if (ALARM_DISPLAY == parameterName)
            {
                m_alarmDisplay = data->getStringData(i, valueColumn);
            }
        }

        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;

        // create the SQL string to retrieve the data of the entity
        // based upon the key
        /*std::ostringstream sql2;
        sql2 << "select entitytype.guiapplicationkey, entity.name from entitytype, entity "
             << "where entitytype.pkey = entity.typekey and entity.pkey = "
             << m_key;*/
//		strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4019, m_key);
//		strSql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4019, m_key);

		databaseConnection->prepareSQLStatement(strSql, ENTITYTYPE_SELECT_4019, m_key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::string guiKeyColumn = "GUIKEY";
        std::string entityNameColumn = "ENTITY_NAME";
        columnNames.clear();
        columnNames.push_back(guiKeyColumn);
        columnNames.push_back(entityNameColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        data = databaseConnection->executeQuery(strSql,columnNames);

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            delete data;
            data = NULL;
            invalidate(); // Reset the member variables that may have been set
            std::ostringstream reasonMessage;
			reasonMessage << "No entity and entitytype details found for entity pkey = " << m_key;
                using TA_Base_Core::DataException;
                TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            delete data;
            data = NULL;
            invalidate(); // Reset the member variables that may have been set
            std::ostringstream reasonMessage;
			reasonMessage << "More than one entity and entitytype details found for entity pkey = " << m_key;
                using TA_Base_Core::DataException;
                TA_THROW(DataException(reasonMessage.str().c_str(),DataException::NOT_UNIQUE,""));
        }

        m_guiApplicationKey = data->getUnsignedLongData(0, guiKeyColumn);
        m_name = data->getStringData(0, entityNameColumn);

        delete data;
        data = NULL;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "reload" );
    }

} // closes TA_Base_Core
