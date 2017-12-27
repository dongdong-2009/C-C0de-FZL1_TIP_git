/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/CODE_TA_BASE_DDB/transactive/app/configuration/configuration_editor/src/ConfigurationEditorSessionUpdater.cpp $
  * @author:  Marc Oliver Castaneda
  * @version: $Revision: 0 $
  *
  *
  * This class is used by the worker thread in ConfigurationEditor.cpp to update
  * the timestamp in the database
  *
  */

#include "app/configuration/configuration_editor/src/stdafx.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorSessionUpdater.h"
#include "core/data_access_interface/src/DataTypeEnums.h"
#include "core/utilities/src/DebugUtil.h"
#include <string>

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    ConfigurationEditorSessionUpdater::ConfigurationEditorSessionUpdater()
    {
        // update config_editor_timestamp set last_timestamp = current_timestamp
        m_databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::OnlineUpdatable_Cd, TA_Base_Core::Write);
        m_databaseConnection->prepareSQLStatement(m_updateSql, ENTITYPARAMETER_UPDATE_20507);
        insertConfigSessionTimeStamp();
    }

    bool ConfigurationEditorSessionUpdater::updateConfigSessionTimeStamp()
    {
        FUNCTION_ENTRY("updateConfigSessionTimeStamp");

        try
        {
            m_databaseConnection->executeModification(m_updateSql);
            return true;
        }
        catch (...)
        {
            //If there was an exception, handle it quietly.
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Error updating timestamp in config_editor_timestamp table ");
        }

        FUNCTION_EXIT;
        return false;
    }

    void ConfigurationEditorSessionUpdater::insertConfigSessionTimeStamp()
    {
        FUNCTION_ENTRY("insertConfigSessionTimeStamp");

        //SELECT last_timestamp FROM config_editor_timestamp
        TA_Base_Core::SQLStatement strSql;
        m_databaseConnection->prepareSQLStatement(strSql, ENTITYPARAMETER_SELECT_20507);

        std::vector<std::string> columns;
        const std::string COUNT = "COUNT";
        columns.push_back(COUNT);
        TA_Base_Core::IData* data = NULL;

        try
        {
            data = m_databaseConnection->executeQuery(strSql, columns);

            if (data->getNumRows() == 1)
            {
                unsigned long count = data->getUnsignedLongData(0, COUNT);

                if (0 == count)
                {
                    // INSERT INTO config_editor_timestamp (last_timestamp) VALUES (current_timestamp)
                    delete data;
                    TA_Base_Core::SQLStatement insertSql;
                    m_databaseConnection->prepareSQLStatement(insertSql, ENTITYPARAMETER_UPDATE_20508);
                    m_databaseConnection->executeModification(insertSql);
                    data = m_databaseConnection->executeQuery(strSql, columns);
                }
            }
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine if a Configuration Editor is already connected to this schema");
        }

        delete data;
        data = NULL;
        FUNCTION_EXIT;
    }
}
