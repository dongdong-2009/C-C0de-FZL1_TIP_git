/**
  * The source code in this file is the property of ST Electronics and is not for redistribution
  * in any form.
  *
  *  
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/EntityAccessFactory.cpp $
  * @author Nick Jardine
  * @version $Revision: #9 $
  * Last modification: $DateTime: 2017/08/30 13:52:13 $
  * Last modified by: $Author: yong.liu $
  *
  * EntityAccessFactory is a singleton class that is used to retrieve entities, and entity strings
  * from the database. Entities are returned as IEntityData objects, while entity strings are
  * contian the CORBA IOR for locating the entity in the naming service.
  */

#include <set>

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/entity_access/src/ConfigEntity.h"
#include "core/data_access_interface/entity_access/src/DefaultEntity.h"
// include the header files for each entity type
#include "core/data_access_interface/entity_access/src/Camera.h"
#include "core/data_access_interface/entity_access/src/PlanAgentData.h"
#include "core/data_access_interface/entity_access/src/PlanManagerEntityData.h"
#include "core/data_access_interface/entity_access/src/AlarmAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/HistoryViewerData.h"
#include "core/data_access_interface/entity_access/src/ScadaHistoryAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/ScadaHistoryConfigData.h"
#include "core/data_access_interface/entity_access/src/NotificationAgentData.h"
#include "core/data_access_interface/entity_access/src/System.h"
#include "core/data_access_interface/entity_access/src/Process.h"
#include "core/data_access_interface/entity_access/src/ControlStation.h"
#include "core/data_access_interface/entity_access/src/AlarmGUI.h"
#include "core/data_access_interface/entity_access/src/PMSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/PEnergyUnitEntityData.h"
#include "core/data_access_interface/entity_access/src/EMSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/PEnergyAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/SNMPStatusAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/VirtualDataPointAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/StationECSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/MasterECSAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "core/data_access_interface/entity_access/src/StationEntityData.h"
#include "core/data_access_interface/entity_access/src/StationSystemEntityData.h"
#include "core/data_access_interface/entity_access/src/StationSubSystemEntityData.h"
#include "core/data_access_interface/entity_access/src/EquipmentEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/AlarmVolumeTestEntityType.h"
#include "core/data_access_interface/entity_access/src/Banner.h"
#include "core/data_access_interface/entity_access/src/RightsAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/DutyAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/VideoMonitor.h"
#include "core/data_access_interface/entity_access/src/VideoOutputGroup.h"
#include "core/data_access_interface/entity_access/src/Sequence.h"
#include "core/data_access_interface/entity_access/src/Quad.h"
#include "core/data_access_interface/entity_access/src/BVSStage.h"
#include "core/data_access_interface/entity_access/src/RecordingUnit.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"
#include "core/data_access_interface/entity_access/src/EventViewer.h"
#include "core/data_access_interface/entity_access/src/DataNodeAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/ConfigurationEditor.h"
#include "core/data_access_interface/entity_access/src/ECSManager.h"
#include "core/data_access_interface/entity_access/src/RadioEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioSessionEntityData.h"
#include "core/data_access_interface/entity_access/src/TISAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/STISEntityData.h"
#include "core/data_access_interface/entity_access/src/PIDSEntityData.h"
#include "core/data_access_interface/entity_access/src/SchedulingAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/StationPAAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/MasterPAAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/WILDAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/AlarmStoreEntityData.h"
#include "core/data_access_interface/entity_access/src/SystemStatusAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioDirectoryEntityData.h"
#include "core/data_access_interface/entity_access/src/AuthenticationAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainSettingsEntityData.h"
#include "core/data_access_interface/entity_access/src/ArchiveManagerEntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/MmsAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/MmsAlarmSubmitterEntityData.h"
#include "core/data_access_interface/entity_access/src/MmsJobRequestSubmitterEntityData.h"
#include "core/data_access_interface/entity_access/src/MmsPeriodicEntityData.h"
#include "core/data_access_interface/entity_access/src/MmsConnectiontEntityData.h"
#include "core/data_access_interface/entity_access/src/DisplayManagerEntityData.h"
#include "core/data_access_interface/entity_access/src/RegionManagerEntityData.h"
#include "core/data_access_interface/entity_access/src/DutyManagerEntityData.h"
#include "core/data_access_interface/entity_access/src/TisLogViewerEntityData.h"
#include "core/data_access_interface/entity_access/src/OnlinePrintingAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainCctvCameraEntityData.h"
#include "core/data_access_interface/entity_access/src/CallBannerPage.h"
#include "core/data_access_interface/entity_access/src/LessAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioStateSynchronisationEntityData.h"
#include "core/data_access_interface/entity_access/src/EquipmentStatusViewerEntityData.h"
#include "core/data_access_interface/entity_access/src/RadioProfileEntityData.h"
#include "core/data_access_interface/entity_access/src/TelephoneManagerProfileEntityData.h"
#include "core/data_access_interface/entity_access/src/PidLogViewerEntityData.h"
#include "core/data_access_interface/entity_access/src/TTISManagerEntityData.h"
// wenching++ (TD12997)
#include "core/data_access_interface/entity_access/src/InspectorPanelEntityData.h"
#include "core/data_access_interface/entity_access/src/PowerDemandEditorEntityData.h" //TD14000
// ++wenching (TD12997)
// Raymond Pau++ (TD13367)
#include "core/data_access_interface/entity_access/src/ScadaRootEntityData.h"
// ++Raymond Pau (TD13367)
#include "core/data_access_interface/entity_access/src/ServerEntityData.h"

#include "core/data_access_interface/entity_access/src/AtsTwpEntityData.h"     //TD15213
#include "core/data_access_interface/entity_access/src/AtsPsdEntityData.h"     //TD15213
#include "core/data_access_interface/src/SQLCode.h"   // zhilin ouyang++

//TD15918 (Mintao++)
#include "core/data_access_interface/entity_access/src/MisAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/MisAlarmSubmitterEntityData.h"
#include "core/data_access_interface/entity_access/src/MisConnectiontEntityData.h"
#include "core/data_access_interface/entity_access/src/MisJobRequestSubmitterEntityData.h"
#include "core/data_access_interface/entity_access/src/MisPeriodicEntityData.h"
//lizettejl++ (TD14697)
#include "core/data_access_interface/entity_access/src/HierarchicalAlarmViewEntityData.h"
//++lizettejl
//TD17997 marc++
#include "core/data_access_interface/entity_access/src/RadioGlobalEntityData.h"
// +marc
//for GF BAS
#include "core/data_access_interface/entity_access/src/StationBASAgentEntityData.h"
//for SOE
#include "core/data_access_interface/entity_access/src/SOEEventViewer.h"

//Shiva++ - Add DataLogAgent
#include "core/data_access_interface/entity_access/src/DataLogAgentEntityData.h"
//++Noel
#include "core/data_access_interface/entity_access/src/EventAgentEntityData.h"
//++Noel
#include "core/data_access_interface/entity_access/src/SNMPDeviceEntityData.h"
#include "core/data_access_interface/entity_access/src/GenaConfigEntity.h"

#include "core/data_access_interface/entity_access/src/ModbusServerAgentEntityData.h"
#include <boost/foreach.hpp>

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    EntityAccessFactory& EntityAccessFactory::getInstance()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "In EntityAccessFactory::getInstance");
        static EntityAccessFactory factory;
        LOG(SourceInfo, DebugUtil::FunctionExit, "EntityAccessFactory::getInstance");
        return factory;
    }


    EntityAccessFactory::~EntityAccessFactory()
    {
        m_entityParameterCache.clear();
        m_entityTypesCache.clear();
    }

    EntityAccessFactory::EntityAccessFactory()
    {
        setUpMap();
        m_entityParameterCache.clear();
        m_entityTypesCache.clear();
    }


    IEntityData* EntityAccessFactory::getEntity(const std::string& name, const bool readWrite /* = false */)
    {
        FUNCTION_ENTRY("getEntity(name)");
        // create the SQL string to retrieve the entity parameters pkey, and typeName
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // note: upper() is used for case insensitivity.
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME,";
        sql << " e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << " e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS') as DATE_CREATED,";
        sql << " TO_CHAR(e.DATE_MODIFIED,'YYYYMMDDHH24MISS') as DATE_MODIFIED,";
        sql << " e.pkey, et.name FROM entity_v e, entitytype et where upper(e.name) = upper('";
        sql <<  databaseConnection->escapeQueryString(name) << "') and et.pkey = e.typekey and e.deleted = 0";
        */
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(),             entity_v_Oracle_SELECT_2551, databaseConnection->escapeQueryString(name));
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2551, databaseConnection->escapeQueryString(name));
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2551, databaseConnection->escapeQueryString(name));
        //      TA_ASSERT(sql.size() > 0, "getEntity: GetSQLStatement error");
        IEntityDataList entities;
        if(readWrite == false)
        {
            /*std::ostringstream parameterSQL;
            parameterSQL << "SELECT entitykey, parametername, value "
                         << "from entityparametervalue_v "
                         << "where entitykey in (select pkey FROM entity_v where upper(name) = upper('"
                         << databaseConnection->escapeQueryString(name) << "') and deleted = 0 )";*/
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3051,
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3051,
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3051,
                                                    databaseConnection->escapeQueryString(name));
            //          TA_ASSERT(parameterSQL.size() > 0, "getEntity: GetSQLStatement error");
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, readWrite);
        }
        // Check to see if there were any values returned
        if(entities.size() == 0)
        {
            std::string message = "No data found for " + name;
            // TA_THROW( DataException( message.c_str(), DataException::NO_VALUE, sql ) );
            std::string strExSql;
            int nExDbType = databaseConnection->getDbServerType();
            switch(nExDbType)
            {
                case enumOracleDb:
                    strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
                    break;
                case enumMysqlDb:
                    strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
                    break;
            }
            TA_THROW(DataException(message.c_str(), DataException::NO_VALUE, strExSql));
        }
        // entity names are unique, therefore if we get to this point
        // there should be one and only one element in the vector
        TA_ASSERT(1 == entities.size(), "Entity name unique constraint violated");
        FUNCTION_EXIT;
        return entities[0];
    }

	IEntityData* EntityAccessFactory::getEntity2(const std::string& name, const bool readWrite /* = false */)
    {
        FUNCTION_ENTRY("getEntity(name)");
        // create the SQL string to retrieve the entity parameters pkey, and typeName
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // note: upper() is used for case insensitivity.
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME,";
        sql << " e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << " e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS') as DATE_CREATED,";
        sql << " TO_CHAR(e.DATE_MODIFIED,'YYYYMMDDHH24MISS') as DATE_MODIFIED,";
        sql << " e.pkey, et.name FROM entity_v e, entitytype et where upper(e.name) = upper('";
        sql <<  databaseConnection->escapeQueryString(name) << "') and et.pkey = e.typekey and e.deleted = 0";
        */
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(),             entity_v_Oracle_SELECT_2551, databaseConnection->escapeQueryString(name));
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2551, databaseConnection->escapeQueryString(name));
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2580, databaseConnection->escapeQueryString(name));
        //      TA_ASSERT(sql.size() > 0, "getEntity: GetSQLStatement error");
        IEntityDataList entities;
        if(readWrite == false)
        {
            /*std::ostringstream parameterSQL;
            parameterSQL << "SELECT entitykey, parametername, value "
                         << "from entityparametervalue_v "
                         << "where entitykey in (select pkey FROM entity_v where upper(name) = upper('"
                         << databaseConnection->escapeQueryString(name) << "') and deleted = 0 )";*/
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3051,
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3051,
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3080,
                                                    databaseConnection->escapeQueryString(name));
            //          TA_ASSERT(parameterSQL.size() > 0, "getEntity: GetSQLStatement error");
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, readWrite);
        }
        // Check to see if there were any values returned
        if(entities.size() == 0)
        {
            std::string message = "No data found for " + name;
            // TA_THROW( DataException( message.c_str(), DataException::NO_VALUE, sql ) );
            std::string strExSql;
            int nExDbType = databaseConnection->getDbServerType();
            switch(nExDbType)
            {
                case enumOracleDb:
                    strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
                    break;
                case enumMysqlDb:
                    strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
                    break;
            }
            TA_THROW(DataException(message.c_str(), DataException::NO_VALUE, strExSql));
        }
        // entity names are unique, therefore if we get to this point
        // there should be one and only one element in the vector
        TA_ASSERT(1 == entities.size(), "Entity name unique constraint violated");
        FUNCTION_EXIT;
        return entities[0];
    }

    IEntityData* EntityAccessFactory::getEntity(const unsigned long key, const bool readWrite /* = false */)
    {
        FUNCTION_ENTRY("getEntity(key)");
        // create the SQL string to retrieve the entity parameters and typeName
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << " e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << " TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'), ";
        sql << " e.pkey, et.name FROM entity_v e, entitytype et WHERE e.pkey = ";
        sql <<  key << " and et.pkey = e.typekey";*/
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2552, key);
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2552, key);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2552, key);
        //      TA_ASSERT(sql.size() > 0, "getEntity: GetSQLStatement error");
        IEntityDataList entities;
        if(readWrite == false)
        {
            /*std::ostringstream parameterSQL;
            parameterSQL << "SELECT entitykey, parametername, value "
                << "from entityparametervalue_v "
                << "where entitykey in (select pkey FROM entity_v where pkey = "
                << key
                << " and deleted = 0)";*/
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(),                entityparametervalue_v_Oracle_SELECT_3052, key);
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3052, key);
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3052, key);
            //          TA_ASSERT(parameterSQL.size() > 0, "getEntity: GetSQLStatement error");
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, readWrite);
        }
        // Check to see if there were any values returned
        if(entities.size() == 0)
        {
            std::ostringstream message;
            message << "No data found for entity with key " << key;
            // TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );
            std::string strExSql;
            int nExDbType = databaseConnection->getDbServerType();
            switch(nExDbType)
            {
                case enumOracleDb:
                    strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
                    break;
                case enumMysqlDb:
                    strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
                    break;
            }
            TA_THROW(DataException(message.str().c_str(), DataException::NO_VALUE, strExSql));
        }
        // entity keys are unique, therefore if we get to this point
        // there should be one and only one element in the vector
        TA_ASSERT(1 == entities.size(), "Entity key unique constraint violated");
        FUNCTION_EXIT;
        return entities[0];
    }


    CorbaName EntityAccessFactory::getCorbaNameOfEntity(const std::string& name, const bool isAgentName)
    {
        FUNCTION_ENTRY("getCorbaNameOfEntity(name)");
        // create the SQL string to retrieve the type and sybsystem of the entity
        // this also doubles as a check to ensure an entity with the specified
        // NAME actually exists.
        // note: upper() is used for case insensitivity.
        //get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey from entity_v e, entity_v a where upper(e.name) = upper('";
        sql << databaseConnection->escapeQueryString(name) << "') and a.pkey = e.agentkey and e.deleted = 0 and a.deleted = 0";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(),             entity_v_STD_SELECT_2501, databaseConnection->escapeQueryString(name));
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2501, databaseConnection->escapeQueryString(name));
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2501, databaseConnection->escapeQueryString(name));
        //      TA_ASSERT(sql.size() > 0, "getCorbaNameOfEntity: GetSQLStatement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        // Check to see if there were any values returned
        if(corbaNames.size() == 0)
        {
            std::string message = "No data found for CorbaName of " + name;
            // TA_THROW( DataException( message.c_str(), DataException::NO_VALUE, sql ) );
            std::string strExSql;
            int nExDbType = databaseConnection->getDbServerType();
            switch(nExDbType)
            {
                case enumOracleDb:
                    strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
                    break;
                case enumMysqlDb:
                    strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
                    break;
            }
            TA_THROW(DataException(message.c_str(), DataException::NO_VALUE, strExSql));
        }
        // entity names are unique, therefore if we get to this point
        // there should be one and only one element in the vector
        TA_ASSERT(1 == corbaNames.size(), "Entity name unique constraint violated");
        FUNCTION_EXIT;
        return corbaNames[0];
    }


    CorbaName EntityAccessFactory::getCorbaNameOfEntity(const unsigned long key, const bool isAgentKey)
    {
        FUNCTION_ENTRY("getCorbaNameOfEntity(key)");
        // create the SQL string to retrieve the type and sybsystem of the entity
        // this also doubles as a check to ensure an entity with the specified
        // key actually exists.
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey from entity_v e, entity_v a where e.PKEY = ";
        sql << key << " and a.pkey = e.agentkey";*/
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_STD_SELECT_2502, key);
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2502, key);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2502, key);
        //      TA_ASSERT(sql.size() > 0, "getCorbaNameOfEntity: GetSQLStatement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        // Check to see if there were any values returned
        if(corbaNames.size() == 0)
        {
            std::ostringstream message;
            message << "No data found for CorbaName of  entity with key " << key;
            //TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );
            std::string strExSql;
            int nExDbType = databaseConnection->getDbServerType();
            switch(nExDbType)
            {
                case enumOracleDb:
                    strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
                    break;
                case enumMysqlDb:
                    strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
                    break;
            }
            TA_THROW(DataException(message.str().c_str(), DataException::NO_VALUE, strExSql));
        }
        // entity keys are unique, therefore if we get to this point
        // there should be one and only one element in the vector
        TA_ASSERT(1 == corbaNames.size(), "Entity key unique constraint violated");
        FUNCTION_EXIT;
        return corbaNames[0];
    }

    IEntityDataList EntityAccessFactory::getTTISLibraryVersionEntityFromNamelike(const std::string& entiyName)
    {
        FUNCTION_ENTRY("getTTISLibraryVersionEntityFromNamelike");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the entity data of the specified entity type
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, ";
        sql << "TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << "TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << "e.pkey, et.name FROM entity_v e, entitytype et WHERE e.name like '%";
        sql << databaseConnection->escapeQueryString( entiyName ) << "' and et.pkey = e.typekey and e.deleted = 0";*/
        /*SQLParse.add ("SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME,");
        SQLParse.addTo_Char("e.DATE_CREATED");
        SQLParse.add ("e.pkey, et.name FROM entity_v e, entitytype et WHERE e.name like '%");
        SQLParse.addBindPara();
        SQLParse.addParameter();*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(),             entity_v_Oracle_SELECT_2553, databaseConnection->escapeQueryString( entiyName ));
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2553, databaseConnection->escapeQueryString( entiyName ));
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2553, databaseConnection->escapeQueryString(entiyName));
        //      TA_ASSERT(sql.size() > 0, "getTTISLibraryVersionEntityFromNamelike: GetSQLStatement error");
        //set up the columns
        std::string typeColumn = "TYPEKEY";
        std::string nameColumn = "NAME";
        std::string addressColumn = "ADDRESS";
        std::string descriptionColumn = "DESCRIPTION";
        std::string subsystemColumn = "SUBSYSTEMKEY";
        std::string subsystemNameColumn = "SUBSYSTEMNAME";
        std::string physicalSubsystemColumn = "PHYSICAL_SUBSYSTEM_KEY";
        std::string physicalSubsystemNameColumn = "PHYSICAL_SUBSYSTEM_NAME";
        std::string locationColumn = "LOCATIONKEY";
        std::string locationNameColumn = "LOCATIONNAME";
        std::string regionColumn = "SEREGI_ID";
        std::string regionNameColumn = "SEREGINAME";
        std::string parentColumn = "PARENTKEY";
        std::string parentNameColumn = "PARENTNAME";
        std::string agentColumn = "AGENTKEY";
        std::string agentNameColumn = "AGENTNAME";
        std::string dateCreatedColumn = "DATECREATED";
        std::string dateModifiedColumn = "DATEMODIFIED";
        std::string keyColumn = "PKEY";
        std::string typeNameColumn = "TYPENAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(typeColumn);
        columnNames.push_back(nameColumn);
        columnNames.push_back(addressColumn);
        columnNames.push_back(descriptionColumn);
        columnNames.push_back(subsystemColumn);
        columnNames.push_back(subsystemNameColumn);
        columnNames.push_back(physicalSubsystemColumn);
        columnNames.push_back(physicalSubsystemNameColumn);
        columnNames.push_back(locationColumn);
        columnNames.push_back(locationNameColumn);
        columnNames.push_back(regionColumn);
        columnNames.push_back(regionNameColumn);
        columnNames.push_back(parentColumn);
        columnNames.push_back(parentNameColumn);
        columnNames.push_back(agentColumn);
        columnNames.push_back(agentNameColumn);
        columnNames.push_back(dateCreatedColumn);
        columnNames.push_back(dateModifiedColumn);
        columnNames.push_back(keyColumn);
        columnNames.push_back(typeNameColumn);
        // Execute the query. The method can throw a DatabaseException.
        // We are responsible for deleting the returned IData object when we're done with it
        // We use this call direct, as we *want* multiple values returned
        // get a connection to the database
        IData* data = databaseConnection->executeQuery(sql, columnNames);
        IEntityDataList entities;
        unsigned long entityKey = 0;
        do
        {
            for(unsigned long i = 0; i < data->getNumRows() ; i++)
            {
                IEntityData* iEntity;
                entityKey = data->getUnsignedLongData(i, keyColumn);
                iEntity = createEntityFromType(data->getStringData(i, typeNameColumn),
                                               entityKey);
                // Cast to an entity data and assign the value
                EntityData* entity = dynamic_cast< EntityData* >(iEntity);
                // If the cast succeeds, assign the data
				if ( NULL != entity )
                {
                    entity->assignDefaultData(data, i);
                    //entityDataMap.insert( EntityKeyToEntityMap::value_type(entityKey, entity) );
                }
                //T temp(iEntity);
                entities.push_back(iEntity);
            }
            delete data;
            data = NULL;
        }
        while(databaseConnection->moreData(data));
        FUNCTION_EXIT;
        return entities;
    }


    IEntityDataList EntityAccessFactory::getEntitiesOfType(const std::string& type, const bool readWrite /* = false */, const bool loadParam /* = true */, const bool isOnly/*false*/)
    {
        FUNCTION_ENTRY("getEntitiesOfType");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(),             entity_v_Oracle_SELECT_2554, databaseConnection->escapeQueryString(type));
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2554, databaseConnection->escapeQueryString(type));
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2554, databaseConnection->escapeQueryString(type));
        //      TA_ASSERT(sql.size() > 0, "getEntitiesOfType: Get SQL Statement error");
        // create the SQL string to retrieve the entity data of the specified entity type
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, ";
        sql << "TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << "TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << "e.pkey, et.name FROM entity_v e, entitytype et WHERE et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and et.pkey = e.typekey and e.deleted = 0";*/
        IEntityDataList entities;
        if(false == readWrite && true == loadParam)
        {
            //std::string parameterSQL;
            SQLStatement parameterSQL;
            if(false == isOnly)
            {
                //parameterSQL << "SELECT entitykey, parametername, value "
                //  << "from entityparametervalue_v "
                //  << "where entitykey in (select pkey FROM entity_v where typekey = "
                //  << "(select pkey from entitytype where name = '"
                //  << databaseConnection->escapeQueryString(type) << "') "
                //  << " and deleted = 0)";
                //std::string entitytypeSQL,entitySQL;
                SQLStatement entitytypeSQL;
                SQLStatement entitySQL;
                std::string stemp0, stemp1, temp;
                std::vector<std::string> columnNames, columns;
                std::string pkey = "PKEY";
                columnNames.push_back(pkey);
                //  entitytypeSQL<< "select pkey from entitytype where name = '"<<databaseConnection->escapeQueryString(type) << "'";
                //                entitytypeSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4001,
                //                entitytypeSQL  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4001,
                databaseConnection->prepareSQLStatement(entitytypeSQL, ENTITYTYPE_SELECT_4001,
                                                        databaseConnection->escapeQueryString(type));
                IData* data = databaseConnection->executeQuery(entitytypeSQL, columnNames);
				if (data->getNumRows()==0)
				{
					return entities;
				}
                stemp0 = data->getStringData(0, "PKEY");
                delete data;
                data = NULL;
                columns.push_back(pkey);
                //      entitySQL<<"select pkey FROM entity_v where typekey = "<<stemp0.c_str()<< " and deleted = 0";
                //              entitySQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(),                   entity_v_STD_SELECT_2503, stemp0.c_str());
                //              entitySQL  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2503, stemp0.c_str());
                databaseConnection->prepareSQLStatement(entitySQL, ENTITY_V_SELECT_2503, stemp0.c_str());
                IData* data1 = databaseConnection->executeQuery(entitySQL, columns);
                stemp1 = "(";
                for(int i = 0; i < data1->getNumRows() ; i++)
                {
                    temp = data1->getStringData(i, "PKEY");
                    stemp1.append(temp);
                    stemp1.append(",");
                }
                stemp1.erase(stemp1.size() - 1, 1);
                stemp1.append(")");
                delete data1;
                data1 = NULL;
                //      parameterSQL << "SELECT entitykey, parametername, value from entityparametervalue_v where entitykey in "<<stemp1.c_str();
                //              parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(),                    entityparametervalue_v_Oracle_SELECT_3053, stemp1.c_str() );
                //              parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3053, stemp1.c_str() );
                databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3053, stemp1.c_str());
                //MMS=>author:jiangshengguang 2008-08-26
            }
            else
            {
                /*parameterSQL << "SELECT entitykey, parametername, value "
                    << "from entityparametervalue_v "
                    << "where entitykey = (select pkey FROM entity_v where typekey = "
                    << "(select pkey from entitytype where name = '"
                    << databaseConnection->escapeQueryString(type) << "') "
                    << " and deleted = 0)";*/
                //              parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(),                    entityparametervalue_v_Oracle_SELECT_3054, databaseConnection->escapeQueryString(type));
                //              parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3054, databaseConnection->escapeQueryString(type));
                databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3054, databaseConnection->escapeQueryString(type));
            }
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, readWrite);
        }
        FUNCTION_EXIT;
        return entities;
    }


    IEntityDataList EntityAccessFactory::getEntitiesOfTypeAtLocationWithNameLikeToken(const std::string& type,
            const unsigned long locationKey, const std::string& token)
    {
        FUNCTION_ENTRY("getEntitiesOfTypeAtLocationWithNameLikeToken");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the entity data of the specified entity type
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2555,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2555,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2555,
                                                databaseConnection->escapeQueryString(type), locationKey, databaseConnection->escapeQueryString(token));
        //      TA_ASSERT(sql.size() > 0, "getEntitiesOfTypeAtLocationWithNameLikeToken: Get SQL statement error");
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "PHYSICAL_SUBSYSTEM_KEY, PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, ";
        sql << "TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << "TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << "e.pkey, et.name FROM entity_v e, entitytype et WHERE e.deleted = 0 and et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and et.pkey = e.typekey and e.locationKey = " << locationKey;
        sql << " and upper(e.name) LIKE upper('" << databaseConnection->escapeQueryString(token) << "')";*/
        IEntityDataList entities;
        //std::string parameterSQL;
        SQLStatement parameterSQL;
        /*parameterSQL << "SELECT entitykey, parametername, value "
            << "from entityparametervalue_v "
            << "where entitykey in (select pkey FROM entity_v where typekey = "
            << "(select pkey from entitytype where name = '"
            << databaseConnection->escapeQueryString(type) << "') "
            << "and locationKey = " << locationKey << " "
            << "and upper(name) LIKE upper('" << databaseConnection->escapeQueryString(token) << "') "
            << "and deleted = 0)";*/
        //      parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3055,
        //      parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3055,
        databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3055,
                                                databaseConnection->escapeQueryString(type), locationKey, databaseConnection->escapeQueryString(token));
        //TA_ASSERT(parameterSQL.size() > 0, "getEntitiesOfTypeAtLocationWithNameLikeToken: Get SQL statement error");
        getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        FUNCTION_EXIT;
        return entities;
    }

    IEntityDataList EntityAccessFactory::getEntitiesOfTypeAtLocationWithCustomToken(const std::string& type,
                                                                                    const unsigned long locationKey,
                                                                                    const std::string& customToken,
                                                                                    const bool loadParam)
    {
        FUNCTION_ENTRY("getEntitiesOfTypeAtLocationWithCustomToken");
        // get a connection to the database
        IDatabase* databaseConnection =  TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        IEntityDataList entities;
        // create the SQL string to retrieve the entity data of the specified entity type
        /*
        std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "PHYSICAL_SUBSYSTEM_KEY, PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, ";
        sql << "TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << "TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << "e.pkey, et.name FROM entity_v e, entitytype et WHERE e.deleted = 0 and et.name = '";
        sql << databaseConnection->escapeQueryString(type);
        sql << "' and et.pkey = e.typekey and e.locationKey = ";
        sql << locationKey << " ";
        sql << customToken;
        */
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2577,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2577,
                                                databaseConnection->escapeQueryString(type), locationKey, customToken);
        //      TA_ASSERT(sql.size() > 0, "getEntitiesOfTypeAtLocationWithCustomToken: Get SQL Statement error");
        if(loadParam)
        {
            /*
            std::ostringstream parameterSQL;
            parameterSQL << "SELECT entitykey, parametername, value "
                << "from entityparametervalue_v "
                << "where entitykey in (select pkey FROM entity_v e where e.typekey = "
                << "(select pkey from entitytype where name = '"
                << databaseConnection->escapeQueryString(type) << "') "
                << "and e.locationKey = " << locationKey << " "
                << customToken
                << " and deleted = 0)";
            */
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3072, databaseConnection->escapeQueryString(type),
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3072, databaseConnection->escapeQueryString(type),
                                                    locationKey, customToken);
            //          TA_ASSERT(parameterSQL.size() > 0, "getEntitiesOfTypeAtLocationWithCustomToken: Get SQL Statement error");
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, false);
        }
        FUNCTION_EXIT;
        return entities;
    }

    IEntityDataList EntityAccessFactory::getEntitiesOfTypeWithCustomToken(const std::string& type,
                                                                          const std::string& customToken, const bool loadParam /* = true */)
    {
        FUNCTION_ENTRY("getEntitiesOfTypeWithCustomToken");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        IEntityDataList entities;
        // create the SQL string to retrieve the entity data of the specified entity type
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "PHYSICAL_SUBSYSTEM_KEY, PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, ";
        sql << "TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << "TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << "e.pkey, et.name FROM entity_v e, entitytype et WHERE e.deleted = 0 and et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and et.pkey = e.typekey ";
        sql << databaseConnection->escapeQueryString(customToken);*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2556,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2556,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2556,
                                                databaseConnection->escapeQueryString(type), databaseConnection->escapeQueryString(customToken));
        //      TA_ASSERT(sql.size() > 0, "getEntitiesOfTypeWithCustomToken: Get SQL Statement error");
        if(true == loadParam)
        {
            /*std::ostringstream parameterSQL;
            parameterSQL << "SELECT entitykey, parametername, value "
                << "from entityparametervalue_v "
                << "where entitykey in (select pkey FROM entity_v where typekey = "
                << "(select pkey from entitytype where name = '"
                << databaseConnection->escapeQueryString(type) << "') "
                << databaseConnection->escapeQueryString(customToken) << " "
                << "and deleted = 0)";*/
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(),                entityparametervalue_v_Oracle_SELECT_3056, databaseConnection->escapeQueryString(type),
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3056, databaseConnection->escapeQueryString(type),
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3056, databaseConnection->escapeQueryString(type),
                                                    databaseConnection->escapeQueryString(customToken));
            //          TA_ASSERT(parameterSQL.size() > 0, "getEntitiesOfTypeWithCustomToken: Get SQL Statement error");
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, false);
        }
        FUNCTION_EXIT;
        return entities;
    }

    //Mintao++ TD16762
    IEntityDataList EntityAccessFactory::getAllChildEntitiesOfType(const std::string& type, const bool readWrite /*=false*/)
    {
        FUNCTION_ENTRY("getEntitiesOfTypeWithTheChildEntities");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the entity data of the specified entity type
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, ";
        sql << "TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'), ";
        sql << "TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'), ";
        sql << "e.pkey, ec.name ";
        sql << "FROM entity_v e, entity_v p, entitytype et, entitytype ec WHERE e.deleted = 0 and e.PARENTKEY = p.pkey ";
        sql << "and ec.PKEY = e.TYPEKEY ";
        sql << "and (p.deleted=0 and p.typekey = et.pkey and et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "')";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2557,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2557,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2557,
                                                databaseConnection->escapeQueryString(type));
        //      TA_ASSERT(sql.size() > 0, "getAllChildEntitiesOfType: Get SQL Statement error");
        IEntityDataList entities;
        getEntityData(databaseConnection, sql, entities, readWrite);
        FUNCTION_EXIT;
        return entities;
    }
    //Mintao++ TD16762

    SharedIEntityDataList EntityAccessFactory::getAllChildEntitiesOfAgentDataNode(const unsigned long agentKey, bool excludeAgentEntities)
    {
        FUNCTION_ENTRY("getEntitiesOfTypeWithTheChildEntities");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the entity data of the specified entity type
        std::ostringstream strAgentKey;
        (true == excludeAgentEntities) ? (strAgentKey << "e.agentkey != " << agentKey << " and ") : strAgentKey << " ";
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2558,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2558,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2558,
                                                strAgentKey.str(), agentKey, DataNodeEntityData::getStaticType());
        //      TA_ASSERT(sql.size() > 0, "getAllChildEntitiesOfAgentDataNode: Get SQL Statement error");
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, ";
        sql << "TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'), ";
        sql << "TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'), ";
        sql << "e.pkey, et.name ";
        sql << "from entity_v e, entitytype et WHERE e.deleted = 0 and ";
        if (true == excludeAgentEntities)
        {
            sql << "e.agentkey != " << agentKey << " and ";
        }
        sql << "e.agentkey != 0 and ";
        sql << "e.PARENTKEY in (select pkey from entity where agentkey = ";
        sql << agentKey;
        sql << " and typekey = (select pkey from entitytype where name = '";
        sql << DataNodeEntityData::getStaticType();
        sql << "' ) and deleted =0 ) and e.typekey = et.pkey";*/
        strAgentKey.clear();
        strAgentKey.str("");
        (true == excludeAgentEntities) ? (strAgentKey << "agentkey != " << agentKey << " and ") : strAgentKey << " ";
        //      std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(),            entityparametervalue_v_Oracle_SELECT_3057, strAgentKey.str(), agentKey, DataNodeEntityData::getStaticType());
        //      std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3057, strAgentKey.str(), agentKey, DataNodeEntityData::getStaticType());
        SQLStatement parameterSQL;
        databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3057, strAgentKey.str(), agentKey, DataNodeEntityData::getStaticType());
        //      TA_ASSERT(parameterSQL.size() > 0, "getAllChildEntitiesOfAgentDataNode: Get SQL Statement error");
        /*std::ostringstream parameterSQL;
        parameterSQL << "SELECT entitykey, parametername, value "
            << "from entityparametervalue_v "
            << "where entitykey in (select pkey FROM entity where "
            << "deleted = 0 and "
            << "agentkey != 0 and ";
        if (true == excludeAgentEntities)
        {
            parameterSQL << "agentkey != " << agentKey << " and ";
        }
        parameterSQL << "PARENTKEY in (select pkey from entity where agentkey = "
            << agentKey
            << " and typekey = (select pkey from entitytype where name = '"
            << DataNodeEntityData::getStaticType()
            << "' ) and deleted =0 ) )";*/
        SharedIEntityDataList entities;
        getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        FUNCTION_EXIT;
        return entities;
    }

    IEntityDataList EntityAccessFactory::getEntitiesOfTypeWithNameStrictLikeToken(const std::string& type, const std::string& token, const bool readWrite)
    {
        FUNCTION_ENTRY("getEntitiesOfTypeWithNameStrictLikeToken");
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2578,
                                                databaseConnection->escapeQueryString(type), databaseConnection->escapeQueryString(token));
        IEntityDataList entities;
        if(readWrite == false)
        {
            SQLStatement parameterSQL;
            SQLStatement entitytypeSQL;
            SQLStatement entitySQL;
            std::string stemp0, stemp1, stemp2;
            std::vector<std::string> columnNames, columns;
            std::string pkey = "PKEY";
            columnNames.push_back(pkey);
            databaseConnection->prepareSQLStatement(entitytypeSQL, ENTITYTYPE_SELECT_4001,
                                                    databaseConnection->escapeQueryString(type));
            IData* data0 = databaseConnection->executeQuery(entitytypeSQL, columnNames);
            stemp0 = data0->getStringData(0, "PKEY");
            delete data0;
            data0 = NULL;
            columns.push_back(pkey);
            databaseConnection->prepareSQLStatement(entitySQL, ENTITY_V_SELECT_2504,
                                                    stemp0.c_str(), databaseConnection->escapeQueryString(token));
            IData* data1 = databaseConnection->executeQuery(entitySQL, columns);
            stemp1 = "(";
            for(int i = 0; i < data1->getNumRows(); i++)
            {
                stemp2 = data1->getStringData(i, "PKEY");
                stemp1.append(stemp2);
                stemp1.append(",");
            }
            stemp1.erase(stemp1.size() - 1, 1);
            stemp1.append(")");
            delete data1;
            data1 = NULL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3053, stemp1.c_str());
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, readWrite);
        }
        FUNCTION_EXIT;
        return entities;
    }

    IEntityDataList EntityAccessFactory::getEntitiesOfTypeWithNameLikeToken(const std::string& type, const std::string& token, const bool readWrite /* = false */)
    {
        FUNCTION_ENTRY("getEntitiesOfTypeWithNameLikeToken");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the entity data of the specified entity type
        // note: upper() is used for case insensitivity.
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, ";
        sql << "TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << "TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << "e.pkey, et.name FROM entity_v e, entitytype et WHERE e.deleted = 0 and et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and et.pkey = e.typekey and upper(e.name) LIKE upper('%" << databaseConnection->escapeQueryString(token) << "%')";
        */
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2559,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2559,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2559,
                                                databaseConnection->escapeQueryString(type), databaseConnection->escapeQueryString(token));
        //      TA_ASSERT(sql.size() > 0, "getEntitiesOfTypeWithNameLikeToken: Get SQL Statement error");
        IEntityDataList entities;
        if(readWrite == false)
        {
            //std::string parameterSQL;
            SQLStatement parameterSQL;
            /////////////////////////////////////////////////////////////////////
            //Launch Manager modified by: zhangjunsheng 2008/09/11 11:00:00
            /*parameterSQL << "SELECT entitykey, parametername, value "
                << "from entityparametervalue_v "
                << "where entitykey in (select pkey FROM entity_v where typekey = "
                << "(select pkey from entitytype where name = '"
                << databaseConnection->escapeQueryString(type) << "') "
                << "and upper(name) LIKE upper('%" << databaseConnection->escapeQueryString(token) << "%') "
                << "and deleted = 0)";*/
            //std::string entitytypeSQL,entitySQL;
            SQLStatement entitytypeSQL;
            SQLStatement entitySQL;
            std::string stemp0, stemp1, stemp2;
            std::vector<std::string> columnNames, columns;
            std::string pkey = "PKEY";
            columnNames.push_back(pkey);
            //      entitytypeSQL<< "select pkey from entitytype where name = '"<<databaseConnection->escapeQueryString(type) << "'";
            //          entitytypeSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4001,
            //          entitytypeSQL  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4001,
            databaseConnection->prepareSQLStatement(entitytypeSQL, ENTITYTYPE_SELECT_4001,
                                                    databaseConnection->escapeQueryString(type));
            IData* data0 = databaseConnection->executeQuery(entitytypeSQL, columnNames);
            stemp0 = data0->getStringData(0, "PKEY");
            delete data0;
            data0 = NULL;
            columns.push_back(pkey);
            //      entitySQL<<"select pkey FROM entity_v where typekey = "<<stemp0.c_str()<< " and upper(name) LIKE upper('%" << databaseConnection->escapeQueryString(token) << "%') "<<"and deleted = 0";
            //          entitySQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_STD_SELECT_2504,
            //          entitySQL  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2504,
            databaseConnection->prepareSQLStatement(entitySQL, ENTITY_V_SELECT_2504,
                                                    stemp0.c_str(), databaseConnection->escapeQueryString(token));
            IData* data1 = databaseConnection->executeQuery(entitySQL, columns);
            stemp1 = "(";
            for(int i = 0; i < data1->getNumRows(); i++)
            {
                stemp2 = data1->getStringData(i, "PKEY");
                stemp1.append(stemp2);
                stemp1.append(",");
            }
            stemp1.erase(stemp1.size() - 1, 1);
            stemp1.append(")");
            delete data1;
            data1 = NULL;
            //  parameterSQL << "SELECT entitykey, parametername, value from entityparametervalue_v where entitykey in "<<stemp1.c_str();
            //          parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(),                entityparametervalue_v_Oracle_SELECT_3053, stemp1.c_str());
            //          parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3053, stemp1.c_str());
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3053, stemp1.c_str());
            //TA_ASSERT(parameterSQL.size() > 0, "getEntitiesOfTypeWithNameLikeToken: Get SQL Statement error");
            //Launch Manager/////////////////////////////////////////////////////
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, readWrite);
        }
        FUNCTION_EXIT;
        return entities;
    }
    IEntityDataList EntityAccessFactory::getEntitiesOfTypeWithAddressLikeToken(const std::string& type, const std::string& token, const bool readWrite /*=false*/)
    {
        FUNCTION_ENTRY("getEntitiesOfTypeWithAddressLikeToken");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the entity data of the specified entity type
        // note: upper() is used for case insensitivity.
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, ";
        sql << "TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS') as DATE_CREATED,";
        sql << "TO_CHAR(nvl(e.DATE_MODIFIED, TO_DATE('12:00:00 01/01/1990', 'hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS') as DATE_MODIFIED,";
        sql << "e.pkey, et.name FROM entity_v e, entitytype et WHERE et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and et.pkey = e.typekey and upper(e.address) LIKE upper('%" << databaseConnection->escapeQueryString(token) << "%')";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2560,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2560,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2560,
                                                databaseConnection->escapeQueryString(type), databaseConnection->escapeQueryString(token));
        //      TA_ASSERT(sql.size() > 0, "getEntitiesOfTypeWithAddressLikeToken: Get SQL Statement error");
        IEntityDataList entities;
        if(readWrite == false)
        {
            /*std::ostringstream parameterSQL;
            parameterSQL << "SELECT entitykey, parametername, value "
                << "from entityparametervalue_v "
                << "where entitykey in (select pkey FROM entity_v where typekey = "
                << "(select pkey from entitytype where name = '"
                << databaseConnection->escapeQueryString(type) << "') "
                << "and upper(address) LIKE upper('%" << databaseConnection->escapeQueryString(token) << "%') "
                << "and deleted = 0)";*/
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(),                entityparametervalue_v_Oracle_SELECT_3058, databaseConnection->escapeQueryString(type),
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3058, databaseConnection->escapeQueryString(type),
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3058, databaseConnection->escapeQueryString(type),
                                                    databaseConnection->escapeQueryString(token));
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, readWrite);
        }
        FUNCTION_EXIT;
        return entities;
    }

    CorbaNameList EntityAccessFactory::getCorbaNamesOfType(const std::string& type, const bool isAgentType)
    {
        FUNCTION_ENTRY("getCorbaNamesOfType( type )");
        CorbaNameList corbaNames;
        corbaNames = getCorbaNamesOfTypeAtAllLocation(type, isAgentType);
        FUNCTION_EXIT;
        return corbaNames;
    }

    CorbaNameList EntityAccessFactory::getCorbaNamesOfTypeWithNameLikeToken(const std::string& type, const std::string& token, const bool isAgentType)
    {
        FUNCTION_ENTRY("getCorbaNamesOfTypeWithNameLikeToken");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkey of the specified entity type
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey from entity_v e, entity_v a, entitytype et where e.deleted = 0 and a.deleted = 0 and et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and e.typekey = et.pkey and a.PKEY = e.agentkey and upper(e.name) LIKE upper('" << databaseConnection->escapeQueryString(token) << "')";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_STD_SELECT_2505,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2505,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2505,
                                                databaseConnection->escapeQueryString(type), databaseConnection->escapeQueryString(token));
        //      TA_ASSERT(sql.size() > 0, "getCorbaNamesOfTypeWithNameLikeToken: Get SQL Statement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        FUNCTION_EXIT;
        return corbaNames;
    }


    IEntityDataList EntityAccessFactory::getEntitiesOfTypeAtLocation(const std::string& type, unsigned long locationKey, const bool loadParam /* = true */)
    {
        FUNCTION_ENTRY("getEntitiesOfTypeAtLocation( type, key )");
        // get a connection to the database
        IDatabase* databaseConnection =  TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        IEntityDataList entities;
        // create the SQL string to retrieve the entity data of the specified entity type
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "PHYSICAL_SUBSYSTEM_KEY, PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, ";
        sql << "TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << "TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << "e.pkey, et.name FROM entity_v e, entitytype et WHERE e.deleted = 0 and et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and et.pkey = e.typekey and e.locationKey = " << locationKey;*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2561,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2561,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2561,
                                                databaseConnection->escapeQueryString(type), locationKey);
        //      TA_ASSERT(sql.size() > 0, "getEntitiesOfTypeAtLocation: Get SQL Statement error");
        if(true == loadParam)
        {
            /*std::ostringstream parameterSQL;
            parameterSQL << "SELECT entitykey, parametername, value "
                << "from entityparametervalue_v "
                << "where entitykey in (select pkey FROM entity_v where typekey = "
                << "(select pkey from entitytype where name = '"
                << databaseConnection->escapeQueryString(type) << "') "
                << "and locationKey = " << locationKey << " "
                << "and deleted = 0)";*/
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3059,
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3059,
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3059,
                                                    databaseConnection->escapeQueryString(type), locationKey);
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, false);
        }
        FUNCTION_EXIT;
        return entities;
    }


    IEntityDataList EntityAccessFactory::getEntitiesOfTypeAtLocation(const std::string& type, std::vector<unsigned long> locationKeys, const bool loadParam /* = true */)
    {
        FUNCTION_ENTRY("getEntitiesOfTypeAtLocation( type, key )");
        // get a connection to the database
        IDatabase* databaseConnection =  TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        IEntityDataList entities;
        std::ostringstream locationClause;
        if(!locationKeys.empty())
        {
            locationClause << " and ( ";
            for(int i = 0; i < locationKeys.size(); ++ i)
            {
                // If this isn't the first element then it needs an OR added first..
                if(i != 0)
                {
                    locationClause << " OR ";
                }
                locationClause << " locationKey=" << locationKeys[i] << " ";
            }
            locationClause << ")";
        }
        // create the SQL string to retrieve the entity data of the specified entity type
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "PHYSICAL_SUBSYSTEM_KEY, PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, ";
        sql << "TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << "TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << "e.pkey, et.name FROM entity_v e, entitytype et WHERE e.deleted = 0 and et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and et.pkey = e.typekey " << databaseConnection->escapeQueryString(locationClause.str());*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2556,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2556,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2556,
                                                databaseConnection->escapeQueryString(type), databaseConnection->escapeQueryString(locationClause.str()));
        //      TA_ASSERT(sql.size() > 0, "getEntitiesOfTypeAtLocation: Get SQL Statement error");
        if(true == loadParam)
        {
            /*std::ostringstream parameterSQL;
            parameterSQL << "SELECT entitykey, parametername, value "
                << "from entityparametervalue_v "
                << "where entitykey in (select pkey FROM entity_v where typekey = "
                << "(select pkey from entitytype where name = '"
                << databaseConnection->escapeQueryString(type) << "') "
                << databaseConnection->escapeQueryString(locationClause.str()) << " "
                << "and deleted = 0)";*/
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3060,
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3060,
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3060,
                                                    databaseConnection->escapeQueryString(type), databaseConnection->escapeQueryString(locationClause.str()));
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, false);
        }
        FUNCTION_EXIT;
        return entities;
    }


    IEntityDataList EntityAccessFactory::getEntitiesOfTypeAtLocation(const std::string& type, const std::string& locationName)
    {
        FUNCTION_ENTRY("getEntitiesOfTypeAtLocation( type, name )");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the entity data of the specified entity type
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, ";
        sql << "TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << "TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << "e.pkey, et.name FROM entity_v e, entitytype et, location l WHERE e.deleted = 0 and et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and et.pkey = e.typekey and e.locationKey = l.pkey and l.name = '" << databaseConnection->escapeQueryString(locationName) << "'";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2562,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2562,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2562,
                                                databaseConnection->escapeQueryString(type), databaseConnection->escapeQueryString(locationName));
        //      TA_ASSERT(sql.size() > 0, "getEntitiesOfTypeAtLocation: Get SQL Statement error");
        IEntityDataList entities;
        /*std::ostringstream parameterSQL;
        parameterSQL << "SELECT entitykey, parametername, value "
            << "from entityparametervalue_v "
            << "where entitykey in (select pkey FROM entity_v where typekey = "
            << "(select pkey from entitytype where name = '"
            << databaseConnection->escapeQueryString(type) << "') "
            << "and locationKey = (select pkey from location where name = '"
            << databaseConnection->escapeQueryString(locationName) << "') "
            << "and deleted = 0)";*/
        //      std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3061,
        //      std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3061,
        SQLStatement parameterSQL;
        databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3061,
                                                databaseConnection->escapeQueryString(type), databaseConnection->escapeQueryString(locationName));
        getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        FUNCTION_EXIT;
        return entities;
    }


    //TD16609 Mintao++
    //only those datanodes that has datapoints are considered as equipment with entitytype as 'DataNode'
    IEntityDataList EntityAccessFactory::getEquipmentsAtLocation(const unsigned long locationKey)
    {
        FUNCTION_ENTRY("getEquipmentsAtLocation( key )");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the entity data of the specified entity type
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "PHYSICAL_SUBSYSTEM_KEY, PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, ";
        sql << "TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'), ";
        sql << "TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'), ";
        sql << "e.pkey, 'DataNode' ";
        sql << "FROM entity_v e WHERE e.deleted = 0 and e.locationKey = " << locationKey;
        sql << " ";
        sql << "and e.pkey in (select parentkey from entity where typekey = (select pkey from entitytype where name='DataPoint'))";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2563,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2563,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2563,
                                                locationKey);
        //      TA_ASSERT(sql.size() > 0, "getEquipmentsAtLocation: Get SQL Statment error");
        IEntityDataList entities;
        /*std::ostringstream parameterSQL;
        parameterSQL << "SELECT entitykey, parametername, value "
            << "from entityparametervalue_v "
            << "where entitykey in (select pkey FROM entity_v where typekey = "
            << "(select pkey from entitytype where name = 'DataNode') "
            << "and locationKey = " << locationKey << " "
            << "and deleted = 0)";*/
        //std::string strName("DataNode"), parameterSQL;
        std::string strName("DataNode");
        SQLStatement parameterSQL;
        //      parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3059,
        //      parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3059,
        databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3059,
                                                strName, locationKey);
        getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        FUNCTION_EXIT;
        return entities;
    }
    //TD16609 Mintao++

    //TD16889 Mintao
    //current assumption is that the datanode with only datapoints as children is equipment datanode
    bool EntityAccessFactory::isEquipmentDataNode(unsigned long entitykey)
    {
        FUNCTION_ENTRY("getEquipmentCorbaNamesOfTypeAtLocation( type, key )");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkey of the specified entity type
        /*std::ostringstream sql;

        sql << "select count(c.pkey) from entity c, entity p ";
        sql << "where c.deleted = 0 and p.deleted = 0 and c.parentkey = p.pkey and c.typekey = (select pkey from entitytype where name = 'DataPoint') and ";
        sql << "p.pkey = " << entitykey;*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entity_STD_SELECT_4501, entitykey);
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_SELECT_4501, entitykey);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_SELECT_4501, entitykey);
        //      TA_ASSERT(sql.size() > 0, "isEquipmentDataNode: Get SQL Statement error");
        std::string countColumn = "NumberofDP";
        std::vector<std::string> columnNames;
        columnNames.push_back(countColumn);
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        // We use this call direct, as we *want* multiple values returned
        IData* data = databaseConnection->executeQuery(sql, columnNames);
        bool toreturn = false;
        if(data->getNumRows() == 1)
        {
            unsigned long numberofDn = data->getUnsignedLongData(0, countColumn);
            if(numberofDn > 0)
            {
                toreturn = true;
            }
        }
        delete data;
        data = NULL;
        return toreturn;
    }
    //TD16889 Mintao

    CorbaNameList EntityAccessFactory::getCorbaNamesOfTypeAtLocation(const std::string& type,
                                                                     const unsigned long locationKey, const bool isAgentType)
    {
        FUNCTION_ENTRY("getCorbaNamesOfTypeAtLocation( type, key )");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkey of the specified entity type
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey from entity_v e, entity_v a, entitytype et where e.deleted = 0 and a.deleted = 0 and et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and e.typekey = et.pkey and e.locationkey = ";
        sql << locationKey << " and a.PKEY = e.agentkey";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4002,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4002,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4002,
                                                databaseConnection->escapeQueryString(type), locationKey);
        //      TA_ASSERT(sql.size() > 0, "getCorbaNamesOfTypeAtLocation: Get SQL Statement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        FUNCTION_EXIT;
        return corbaNames;
    }


    CorbaNameList EntityAccessFactory::getCorbaNamesOfTypeAtAllLocation(const std::string& type, const bool isAgentType)
    {
        FUNCTION_ENTRY("getCorbaNamesOfTypeAtLocation( type, key )");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkey of the specified entity type
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey from entity_v e, entity_v a, entitytype et where e.deleted = 0 and a.deleted = 0 and et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and e.typekey = et.pkey and a.PKEY = e.agentkey";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4003,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4003,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4003,
                                                databaseConnection->escapeQueryString(type));
        //      TA_ASSERT(sql.size() > 0, "getCorbaNamesOfTypeAtAllLocation: Get SQL Statement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        FUNCTION_EXIT;
        return corbaNames;
    }

    CorbaNameList EntityAccessFactory::getCorbaNamesOfTypeAtLocation(const std::string& type,
                                                                     const std::string& locationName, const bool isAgentType)
    {
        FUNCTION_ENTRY("getCorbaNamesOfTypeAtLocation( type, name )");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkey of the specified entity type
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey from entity_v e, entity_v a, entitytype et, location l where e.deleted = 0 and a.deleted = 0 and et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and e.typekey = et.pkey and e.locationkey = l.pkey and l.name = '";
        sql << databaseConnection->escapeQueryString(locationName) << "' and a.PKEY = e.agentkey";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4004,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4004,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4004,
                                                databaseConnection->escapeQueryString(type), databaseConnection->escapeQueryString(locationName));
        //      TA_ASSERT(sql.size() > 0, "getCorbaNamesOfTypeAtLocation: Get SQL Statement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        FUNCTION_EXIT;
        return corbaNames;
    }

    CorbaNameList EntityAccessFactory::getCorbaNamesOfTypeAtLocationWithNameLikeToken
    (const std::string& type,
     const unsigned long locationKey,
     const std::string& token,
     const bool isAgentType)
    {
        FUNCTION_ENTRY("getCorbaNamesOfTypeWithNameLikeToken");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkey of the specified entity type
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey "
            << "from entity_v e, entity_v a, entitytype et where e.deleted = 0 and a.deleted = 0 and "
            << "et.name = '" << databaseConnection->escapeQueryString(type) << "' and e.typekey = et.pkey and a.PKEY = e.agentkey "
            << "and e.locationkey =" << locationKey << " "
            << "and upper(e.name) LIKE upper('" << databaseConnection->escapeQueryString(token) << "')";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4005,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4005,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4005,
                                                databaseConnection->escapeQueryString(type), locationKey, databaseConnection->escapeQueryString(token));
        //      TA_ASSERT(sql.size() > 0, "getCorbaNamesOfTypeWithNameLikeToken: Get SQL Statement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        FUNCTION_EXIT;
        return corbaNames;
    }

    IEntityDataList EntityAccessFactory::getEntitiesOfTypeInPhysicalSubsystem(const std::string& type,
                                                                              unsigned long physicalSubsystemKey, const bool readWrite /* = false  */)
    {
        FUNCTION_ENTRY("getEntitiesOfTypeInPhysicalSubsystem");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the entity data of the specified entity type
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, ";
        sql << "TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << "TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << "e.pkey, et.name FROM entity_v e, entitytype et WHERE e.deleted = 0 and et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and et.pkey = e.typekey ";
        sql << "and e.physical_subsystem_key = " << physicalSubsystemKey;*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2564,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2564,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2564,
                                                databaseConnection->escapeQueryString(type), physicalSubsystemKey);
        //      TA_ASSERT(sql.size() > 0, "getEntitiesOfTypeInPhysicalSubsystem: Get SQL statement error");
        IEntityDataList entities;
        if(readWrite == false)
        {
            /*std::ostringstream parameterSQL;
            parameterSQL << "SELECT entitykey, parametername, value "
                << "from entityparametervalue_v "
                << "where entitykey in (select pkey FROM entity_v where typekey = "
                << "(select pkey from entitytype where name = '"
                << databaseConnection->escapeQueryString(type) << "') "
                << "and physical_subsystem_key = " << physicalSubsystemKey << " "
                << "and deleted = 0)";*/
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3062,
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3062,
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3062,
                                                    databaseConnection->escapeQueryString(type), physicalSubsystemKey);
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, readWrite);
        }
        FUNCTION_EXIT;
        return entities;
    }

    CorbaNameList EntityAccessFactory::getCorbaNamesOfTypeInPhysicalSubsystem
    (const std::string& type,
     unsigned long physicalSubsystemKey, const bool isAgentType)
    {
        FUNCTION_ENTRY("getCorbaNamesOfTypeInPhysicalSubsystem");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkey of the specified entity type
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey from entity_v e, entity_v a, entitytype et where e.deleted = 0 and a.deleted = 0 and et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and e.typekey = et.pkey and a.PKEY = e.agentkey ";
        sql << "and e.physical_subsystem_key = " << physicalSubsystemKey;*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4006,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4006,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4006,
                                                databaseConnection->escapeQueryString(type), physicalSubsystemKey);
        //      TA_ASSERT(sql.size() > 0, "getCorbaNamesOfTypeInPhysicalSubsystem: Get SQL Statement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        FUNCTION_EXIT;
        return corbaNames;
    }

    IEntityDataList EntityAccessFactory::getChildEntitiesOf(const std::string& name, const bool readWrite /* = false */)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "EntityAccessFactory::getChildEntitiesOf(name)");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkey of the specified entity
        // Formulate the query
        // note: upper() is used for case insensitivity.
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << " TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << " e.pkey, et.name FROM entity_v e, entity_v p, entitytype et WHERE e.deleted = 0 and p.deleted = 0 and upper(p.name) = upper('";
        sql <<  databaseConnection->escapeQueryString(name) << "') and e.PARENTKEY = p.pkey and et.pkey = e.typekey";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2565,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2565,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2565,
                                                databaseConnection->escapeQueryString(name));
        //      TA_ASSERT(sql.size() > 0, "getChildEntitiesOf: Get SQL Statement error");
        IEntityDataList entities;
        if(readWrite == false)
        {
            /*std::ostringstream parameterSQL;
            parameterSQL << "SELECT entitykey, parametername, value "
                << "from entityparametervalue_v "
                << "where entitykey in (select pkey FROM entity_v where "
                << "parentkey in (select pkey from entity_v where upper(name) = upper('"
                << databaseConnection->escapeQueryString(name) << "'))"
                << "and deleted = 0)";*/
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3063,
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3063,
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3063,
                                                    databaseConnection->escapeQueryString(name));
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, readWrite);
        }
        LOG(SourceInfo, DebugUtil::FunctionExit, "EntityAccessFactory::getChildEntitiesOf(name)");
        return entities;
    }


    IEntityDataList EntityAccessFactory::getChildEntitiesOf(const unsigned long key, const bool readWrite /* = false */)
    {
        FUNCTION_ENTRY("getChildEntitiesOf(key)");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkey of the specified entity
        // Formulate the query
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << " e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << " TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << " e.pkey, et.name FROM entity_v e, entitytype et WHERE e.deleted = 0 and e.PARENTKEY = ";
        sql << key << " and et.pkey = e.typekey";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2566,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2566,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2566,
                                                key);
        //      TA_ASSERT(sql.size() > 0, "getChildEntitiesOf: Get SQL Statement error");
        IEntityDataList entities;
        if(readWrite == false)
        {
            /*std::ostringstream parameterSQL;
            parameterSQL << "SELECT entitykey, parametername, value "
                << "from entityparametervalue_v "
                << "where entitykey in (select pkey FROM entity_v where "
                << "parentkey = " << key << " "
                << "and deleted = 0)";*/
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3064,
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3064,
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3064,
                                                    key);
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, readWrite);
        }
        FUNCTION_EXIT;
        return entities;
    }


    std::vector<IEntityDataList> EntityAccessFactory::getChildEntitiesOf(const std::vector<unsigned long> entityKeys, const bool readWrite /* = false */)
    {
        FUNCTION_ENTRY("getChildEntitiesOf(key)");
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkey of the specified entity
        // Formulate the query
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << " e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << " TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << " e.pkey, et.name FROM entity_v e, entitytype et WHERE e.deleted = 0 and e.PARENTKEY in (";
        sql << keys << ") and et.pkey = e.typekey";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2566,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2587,

        std::stringstream keys;

        for (size_t i = 0; i <entityKeys.size(); ++i)
        {
            keys << (0 == i ? "" : ",") << entityKeys[i];
        }

        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2587, keys.str());
        //      TA_ASSERT(sql.size() > 0, "getChildEntitiesOf: Get SQL Statement error");
        std::vector<IEntityDataList> entitiesList;
        IEntityDataList entities;

        if(readWrite == false)
        {
            /*std::ostringstream parameterSQL;
            parameterSQL << "SELECT entitykey, parametername, value "
                << "from entityparametervalue_v "
                << "where entitykey in (select pkey FROM entity_v where "
                << "parentkey in (" << keys << ") "
                << "and deleted = 0)";*/
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3064,
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3082,
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3082, keys.str());
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, readWrite);
        }

        std::map<unsigned long, IEntityDataList> map;

        for (size_t i = 0; i < entities.size(); ++i)
        {
            map[entities[i]->getParent()].push_back(entities[i]);
        }

        for (size_t i = 0; i < entityKeys.size(); ++i)
        {
            entitiesList.push_back(map[entityKeys[i]]);
        }

        FUNCTION_EXIT;
        return entitiesList;
    }


    CorbaNameList EntityAccessFactory::getCorbaNamesOfChildren(const std::string& name)
    {
        FUNCTION_ENTRY("getCorbaNamesOfChildren( name )");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey from entity_v e, entity_v a, entity_v p where e.deleted = 0 and a.deleted = 0 and p.deleted = 0 and e.parentkey = p.pkey and upper(p.name) = upper('";
        sql << databaseConnection->escapeQueryString(name) << "') and a.pkey = e.agentkey";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_STD_SELECT_2506,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2506,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2506,
                                                databaseConnection->escapeQueryString(name));
        //      TA_ASSERT(sql.size() > 0, "getCorbaNamesOfChildren: Get SQL Statement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        FUNCTION_EXIT;
        return corbaNames;
    }


    CorbaNameList EntityAccessFactory::getCorbaNamesOfChildren(const unsigned long key)
    {
        FUNCTION_ENTRY("getCorbaNamesOfChildren( key )");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkey of the specified entity
        // This will act as a check that the parent entity is unique
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey from entity_v e, entity_v a where e.deleted = 0 and a.deleted = 0 and e.parentkey = ";
        sql << key << " and a.pkey = e.agentkey";   */
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_STD_SELECT_2507, key);
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2507, key);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2507, key);
        //      TA_ASSERT(sql.size() > 0, "getCorbaNamesOfChildren: Get SQL Statement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        FUNCTION_EXIT;
        return corbaNames;
    }


    IEntityDataList EntityAccessFactory::getChildEntitiesOfType(const std::string& type, const bool readWrite /* = false */)
    {
        FUNCTION_ENTRY("getChildEntitiesOfType");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkeys of the entity type
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << "TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << "e.pkey, et.name FROM entity_v e, entitytype et WHERE e.deleted = 0 and et.PKEY = e.typekey ";
        sql << "and e.parentkey in (select e.pkey from ENTITY_V e, ENTITYTYPE et where e.deleted = 0 and et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and e.typekey = et.pkey)";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2567,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2567,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2567,
                                                databaseConnection->escapeQueryString(type));
        //      TA_ASSERT(sql.size() > 0, "getChildEntitiesOfType: Get SQL Statement error");
        IEntityDataList entities;
        if(readWrite == false)
        {
            /*std::ostringstream parameterSQL;
            parameterSQL << "SELECT entitykey, parametername, value "
                << "from entityparametervalue_v "
                << "where entitykey in (select pkey FROM entity_v where "
                << "parentkey in (select pkey from ENTITY_V where typekey = (select pkey from entitytype where name = '"
                << databaseConnection->escapeQueryString(type) << "') and deleted = 0) "
                << "and deleted = 0)";*/
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3065,
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3065,
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3065,
                                                    databaseConnection->escapeQueryString(type));
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, readWrite);
        }
        FUNCTION_EXIT;
        return entities;
    }

    CorbaName EntityAccessFactory::getCorbaNameOfParent(const std::string& name)
    {
        FUNCTION_ENTRY("getCorbaNamesOfChildren( name )");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey from entity_v e, entity_v a, entity_v c where e.deleted = 0 and a.deleted = 0 and c.deleted = 0 and c.parentkey = e.pkey and upper(c.name) = upper('";
        sql << databaseConnection->escapeQueryString(name) << "') and a.pkey = e.agentkey";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_STD_SELECT_2508,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2508,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2508,
                                                databaseConnection->escapeQueryString(name));
        //      TA_ASSERT(sql.size() > 0, "getCorbaNameOfParent: Get SQL Statement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        // Check to see if there were any values returned
        if(corbaNames.size() == 0)
        {
            std::string message = "No data found for parent of CorbaName of " + name;
            //TA_THROW( DataException( message.c_str(), DataException::NO_VALUE, sql ) );
            std::string strExSql;
            int nExDbType = databaseConnection->getDbServerType();
            switch(nExDbType)
            {
                case enumOracleDb:
                    strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
                    break;
                case enumMysqlDb:
                    strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
                    break;
            }
            TA_THROW(DataException(message.c_str(), DataException::NO_VALUE, strExSql));
        }
        // entity names are unique, therefore if we get to this point
        // there should be one and only one element in the vector
        TA_ASSERT(1 == corbaNames.size(), "Entity name unique constraint violated");
        FUNCTION_EXIT;
        return corbaNames[0];
    }

    CorbaName EntityAccessFactory::getCorbaNameOfParent(const unsigned long key)
    {
        FUNCTION_ENTRY("getCorbaNamesOfChildren( key )");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkey of the specified entity
        // This will act as a check that the parent entity is unique
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey from entity_v e, entity_v a, entity_v c where c.parentkey = e.pkey and c.pkey = ";
        sql << key << " and a.pkey = e.agentkey and e.deleted = 0 and a.deleted = 0 and c.deleted = 0 ";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_STD_SELECT_2509, key);
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2509, key);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2509, key);
        //      TA_ASSERT(sql.size() > 0, "getCorbaNameOfParent: Get SQL statement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        // Check to see if there were any values returned
        if(corbaNames.size() == 0)
        {
            std::ostringstream message;
            message << "No data found for CorbaName of  entity with key " << key;
            //TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql ) );
            std::string strExSql;
            int nExDbType = databaseConnection->getDbServerType();
            switch(nExDbType)
            {
                case enumOracleDb:
                    strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
                    break;
                case enumMysqlDb:
                    strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
                    break;
            }
            TA_THROW(DataException(message.str().c_str(), DataException::NO_VALUE, strExSql));
        }
        // entity keys are unique, therefore if we get to this point
        // there should be one and only one element in the vector
        TA_ASSERT(1 == corbaNames.size(), "Entity key unique constraint violated");
        FUNCTION_EXIT;
        return corbaNames[0];
    }


    CorbaNameList EntityAccessFactory::getCorbaNamesOfChildrenOfType(const std::string& type)
    {
        FUNCTION_ENTRY("getCorbaNamesOfChildrenOfType");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkeys of the entity type
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey from entity_v e, entity_v a, entity_v p, entitytype et where e.deleted = 0 and a.deleted = 0 and p.deleted = 0 and et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and p.typekey = et.pkey and e.parentkey = p.pkey and a.PKEY = e.agentkey";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_STD_SELECT_2510,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2510,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2510,
                                                databaseConnection->escapeQueryString(type));
        //      TA_ASSERT(sql.size() > 0, "getCorbaNamesOfChildrenOfType: Get SQL statement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        FUNCTION_EXIT;
        return corbaNames;
    }


    CorbaNameList EntityAccessFactory::getCorbaNamesOfChildrenOfTypeAtLocation(const std::string& type,
                                                                               unsigned long locationKey)
    {
        FUNCTION_ENTRY("getCorbaNamesOfChildrenOfTypeAtLocation");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkeys of the entity type
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey from entity_v e, entity_v a, entity_v p, entitytype et where et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "' and p.typekey = et.pkey and e.parentkey = p.pkey and e.locationkey = ";
        sql << locationKey << " and a.PKEY = e.agentkey and e.deleted = 0 and a.deleted = 0 and p.deleted = 0 ";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_STD_SELECT_2511,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2511,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2511,
                                                databaseConnection->escapeQueryString(type), locationKey);
        //      TA_ASSERT(sql.size() > 0, "getCorbaNamesOfChildrenOfTypeAtLocation: Get SQL Statement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        FUNCTION_EXIT;
        return corbaNames;
    }


    IEntityDataList EntityAccessFactory::getChildEntitiesOfEntityOfType(const std::string& name, const std::string& type, bool readWrite /* = false */)
    {
        FUNCTION_ENTRY("getChildEntitiesOfEntityOfType");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkeys of the entity type
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << "e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << "TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << "e.pkey, et.name FROM entity_v e, entity_v p, entitytype et WHERE e.deleted = 0 and p.deleted = 0 and upper(p.name) = upper('";
        sql << databaseConnection->escapeQueryString(name) << "') and e.PARENTKEY = p.pkey and et.pkey = e.typekey and e.typekey = et.pkey and et.name = '";
        sql << databaseConnection->escapeQueryString(type) << "'";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2568,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2568,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2568,
                                                databaseConnection->escapeQueryString(name), databaseConnection->escapeQueryString(type));
        //      TA_ASSERT(sql.size() > 0, "getChildEntitiesOfEntityOfType: Get SQL Statement error");
        IEntityDataList entities;
        if(readWrite == false)
        {
            /*std::ostringstream parameterSQL;
            parameterSQL << "SELECT entitykey, parametername, value "
                << "from entityparametervalue_v "
                << "where entitykey in (select pkey FROM entity_v where "
                << "parentkey in (select pkey from ENTITY_V where upper(name) = upper('"
                << databaseConnection->escapeQueryString(name) << "') and deleted = 0 ) "
                << "and typekey = (select pkey from entitytype where name = '"
                << databaseConnection->escapeQueryString(type) << "') and deleted = 0 )";*/
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3066,
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3066,
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3066,
                                                    databaseConnection->escapeQueryString(name), databaseConnection->escapeQueryString(type));
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            getEntityData(databaseConnection, sql, entities, readWrite);
        }
        FUNCTION_EXIT;
        return entities;
    }


    CorbaNameList EntityAccessFactory::getCorbaNamesOfChildrenOfEntityOfType(const std::string& name, const std::string& type)
    {
        FUNCTION_ENTRY("getCorbaNamesOfChildrenOfEntityOfType");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey from entity_v e, entity_v a, entity_v p, entitytype et where e.deleted = 0 and a.deleted = 0 and p.deleted = 0 and ";
        sql << "e.parentkey = p.pkey and upper(p.name) = upper('" << databaseConnection->escapeQueryString(name) << "') and a.pkey = e.agentkey and ";
        sql << "et.pkey = e.typekey and e.typekey = et.pkey and et.name = '" << databaseConnection->escapeQueryString(type) << "'";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4007,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4007,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4007,
                                                databaseConnection->escapeQueryString(name), databaseConnection->escapeQueryString(type));
        //      TA_ASSERT(sql.size() > 0, "getCorbaNamesOfChildrenOfEntityOfType: Get SQL Statement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        FUNCTION_EXIT;
        return corbaNames;
    }


    SharedIEntityDataList EntityAccessFactory::getDescendantsOfAgent(const unsigned long key)
    {
        FUNCTION_ENTRY("getDescendantsOfAgent( key )");
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // Formulate the query
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << " e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << " TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << " e.pkey, et.name FROM entity_v e, entity_v a, entitytype et WHERE e.deleted = 0 and a.deleted = 0 and a.pkey = ";
        sql <<  key << " and e.agentkey = a.pkey and e.pkey <> a.pkey and et.pkey = e.typekey";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2569, key);
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2569, key);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2569, key);
        //      TA_ASSERT(sql.size() > 0, "getDescendantsOfAgent: Get SQL Statement error");
        SharedIEntityDataList childEntities;
        IEntityDataList entities;
        /*std::ostringstream parameterSQL;
        parameterSQL << "SELECT entitykey, parametername, value "
            << "from entityparametervalue_v "
            << "where entitykey in (select pkey FROM entity_v where "
            << "agentkey in (select pkey from ENTITY_V where pkey = "
            << key << " and deleted = 0) and pkey <> agentkey "
            << "and deleted = 0)";*/
        //      std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(),            entityparametervalue_v_Oracle_SELECT_3067, key);
        //      std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3067, key);
        SQLStatement parameterSQL;
        databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3067, key);
        getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        // Sort the descendants
        IEntityDataList::iterator it;
        std::set< unsigned long > entityMap;
        std::set< unsigned long > allEntityKeys;
        // Build a set of all entity keys
        for(it = entities.begin(); it != entities.end(); ++it)
        {
            allEntityKeys.insert((*it)->getKey());
        }
        entityMap.insert(key);
        // Find entity whose parent aren't in entities list
        for(it = entities.begin(); it != entities.end();)
        {
            if(allEntityKeys.find((*it)->getParent()) == allEntityKeys.end())
            {
                entityMap.insert((*it)->getKey());
                childEntities.push_back(SharedIEntityDataList::value_type(*it));
                it = entities.erase(it);
            }
            else
            {
                ++it;
            }
        }
        do
        {
            for(it = entities.begin(); it != entities.end();)
            {
                // see if the parent exists
                if(entityMap.find((*it)->getParent()) != entityMap.end())
                {
                    entityMap.insert((*it)->getKey());
                    childEntities.push_back(SharedIEntityDataList::value_type(*it));
                    it = entities.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }
        while(false == entities.empty());
        FUNCTION_EXIT;
        return childEntities;
    }

    SharedIEntityDataList EntityAccessFactory::getDescendantsOfAgent(const std::string& agentName)
    {
        FUNCTION_ENTRY("getDescendantsOfAgent( agentName )");
        /*
        * TD5055 - The agent entity is put into an auto pointer to ensure
        *          that it is destroyed when no longer needed.
        */
        std::auto_ptr< IEntityData > agent(getEntity(agentName));
        FUNCTION_EXIT;
        return getDescendantsOfAgent(agent->getKey());
    }

	IEntityDataList EntityAccessFactory::getDataNodeNotInAgentButChildDPInAgent(unsigned long agentKey)
	{
		FUNCTION_ENTRY("getDataNodeNotInAgentButChildDPInAgent");
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Read);
     
		SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2532, agentKey, agentKey);
        
        SQLStatement parameterSQL;
        databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3075, agentKey, agentKey);
		
		IEntityDataList entities;
        getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        
		FUNCTION_EXIT;
        return entities;
	}

    CorbaNameList EntityAccessFactory::getCorbaNamesOfDescendantsOfAgent(const std::string& agentName)
    {
        FUNCTION_ENTRY("getCorbaNamesOfDescendantsOfAgent");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // Formulate the query
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey from entity_v e, entity_v a where e.deleted = 0 and a.deleted = 0 and upper(a.name) = upper('";
        sql << databaseConnection->escapeQueryString(agentName);
        sql << "') and e.agentkey = a.pkey and e.pkey <> a.pkey";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_STD_SELECT_2512,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2512,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2512,
                                                databaseConnection->escapeQueryString(agentName));
        //      TA_ASSERT(sql.size() > 0, "getCorbaNamesOfDescendantsOfAgent: Get SQL Statement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        FUNCTION_EXIT;
        return corbaNames;
    }


    IEntityDataList EntityAccessFactory::getDescendantsOfAgentOfType(const std::string& agentName, const std::string& typeName)
    {
        FUNCTION_ENTRY("getDescendantsOfAgentOfType");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // create the SQL string to retrieve the pkey of the specified entity
        // Formulate the query
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME, ";
        sql << "e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << " e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << " TO_CHAR(e.DATE_MODIFIED, 'YYYYMMDDHH24MISS'),";
        sql << " e.pkey, et.name FROM entity_v e, entity_v a, entitytype et WHERE e.deleted = 0 and a.deleted = 0 and UPPER(a.name) = UPPER('";
        sql << databaseConnection->escapeQueryString(agentName) << "') and et.name = '";
        sql << databaseConnection->escapeQueryString(typeName) << "' and e.agentkey = a.pkey and e.pkey <> a.pkey and et.pkey = e.typekey";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2570,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2570,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2570,
                                                databaseConnection->escapeQueryString(agentName), databaseConnection->escapeQueryString(typeName));
        //      TA_ASSERT(sql.size() > 0, "getDescendantsOfAgentOfType: Get SQL Statement error");
        IEntityDataList entities;
        /*std::ostringstream parameterSQL;
        parameterSQL << "SELECT entitykey, parametername, value "
            << "from entityparametervalue_v "
            << "where entitykey in (select pkey FROM entity_v where "
            << "agentkey in (select pkey from ENTITY_V where upper(name) = upper('"
            << databaseConnection->escapeQueryString(agentName) << "') "
            << "and deleted = 0) "
            << "and typekey in (select pkey from entitytype where name = '"
            << databaseConnection->escapeQueryString(typeName)
            << "') "
            << "and pkey <> agentkey "
            << "and deleted = 0)";*/
        //      std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3068,
        //      std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3068,
        SQLStatement parameterSQL;
        databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3068,
                                                databaseConnection->escapeQueryString(agentName), databaseConnection->escapeQueryString(typeName));
        getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        FUNCTION_EXIT;
        return entities;
    }


    CorbaNameList EntityAccessFactory::getCorbaNamesOfDescendantsOfAgentOfType(const std::string& agentName,
                                                                               const std::string& typeName)
    {
        FUNCTION_ENTRY("getCorbaNamesOfDescendantsOfAgentOfType");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // Formulate the query
        /*std::ostringstream sql;
        sql << "select e.pkey, e.name, a.name, e.locationkey from entity_v e, entity_v a, entitytype et where e.deleted = 0 and a.deleted = 0 and upper(a.name) = upper('";
        sql << databaseConnection->escapeQueryString(agentName) << "') and et.NAME = '" << databaseConnection->escapeQueryString(typeName);
        sql << "' and e.agentkey = a.pkey and e.pkey <> a.pkey and e.typekey = et.pkey";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4008,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4008,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4008,
                                                databaseConnection->escapeQueryString(agentName), databaseConnection->escapeQueryString(typeName));
        //      TA_ASSERT(sql.size() > 0, "getCorbaNamesOfDescendantsOfAgentOfType: Get SQL Statement error");
        CorbaNameList corbaNames;
        getCorbaNames(databaseConnection, sql, corbaNames);
        FUNCTION_EXIT;
        return corbaNames;
    }

    std::string EntityAccessFactory::getTypeOf(const std::string& name)
    {
        FUNCTION_ENTRY("getTypeOf");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        std::string nameColumn = "NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(nameColumn);
        // Formulate the query
        /*std::ostringstream sql;
        sql << "SELECT et.name FROM entity_v e, ENTITYTYPE et WHERE e.deleted = 0 and upper(e.name) = upper('";
        sql << databaseConnection->escapeQueryString(name) << "') AND e.typekey = et.pkey";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4009,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4009,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4009,
                                                databaseConnection->escapeQueryString(name));
        //      TA_ASSERT(sql.size() > 0, "getTypeOf: Get SQL Statement error");
        IData* data = getSingularData(databaseConnection, sql, columnNames, "type of entity");
        std::string type = "";
        if(data != NULL)
        {
            type = data->getStringData(0, nameColumn);
            delete data;
        }
        if(type == "")
        {
            std::string message = "No data found for type of entity";
            //TA_THROW(DataException(message.c_str(),DataException::NO_VALUE,sql));
            std::string strExSql;
            int nExDbType = databaseConnection->getDbServerType();
            switch(nExDbType)
            {
                case enumOracleDb:
                    strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
                    break;
                case enumMysqlDb:
                    strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
                    break;
            }
            TA_THROW(DataException(message.c_str(), DataException::NO_VALUE, strExSql));
        }
        return type;
    }


    IData* EntityAccessFactory::getParameter(const std::string& entityName, const std::string& parameterName)
    {
        FUNCTION_ENTRY("getCorbaNamesOfDescendantsOfAgentOfType");
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        std::string valueColumn = "VALUE";
        std::vector<std::string> columnNames;
        columnNames.push_back(valueColumn);
        // Formulate the query
        /*std::ostringstream sql;
        sql << "select epv.value from entity_v e, entityparameter ep, entityparametervalue epv where e.deleted = 0 and upper(e.name) = upper('";
        sql << databaseConnection->escapeQueryString(entityName) << "') and ep.NAME = '" << databaseConnection->escapeQueryString(parameterName) << "' and epv.parameterkey ";
        sql << "= ep.pkey and epv.ENTITYKEY = e.pkey";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_STD_SELECT_2513,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2513,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2513,
                                                databaseConnection->escapeQueryString(entityName), databaseConnection->escapeQueryString(parameterName));
        //      TA_ASSERT(sql.size() > 0, "getParameter: Get SQL Statement error");
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // This data pointer is returned, so it is not deleted unless an exception is thrown.
        IData* data = databaseConnection->executeQuery(sql, columnNames);
        FUNCTION_EXIT;
        return data;
    }


    IConfigEntity* EntityAccessFactory::createEntity(std::string type)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "EntityAccessFactory::createEntity");
        LOG(SourceInfo, DebugUtil::FunctionExit, "EntityAccessFactory::createEntity");
        return new ConfigEntity(type);
    }


    IConfigEntity* EntityAccessFactory::copyEntity(const IConfigEntity* entityToCopy)
    {
        FUNCTION_ENTRY("copyEntity");
        TA_ASSERT(entityToCopy !=  NULL, "The entity to copy was NULL");
        const ConfigEntity* cast = dynamic_cast<const ConfigEntity*>(entityToCopy);
        TA_ASSERT(cast != NULL, "Entity passed could not be converted into a ConfigEntity");
        FUNCTION_EXIT;
        return new ConfigEntity(*cast);
    }


    void EntityAccessFactory::setUpMap()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "EntityAccessFactory::setUpMap");
        // VideoMonitor entity
        m_entityTypes.insert(EntityTypeMap::value_type(VideoMonitor::getStaticType(), VideoMonitor::clone));
        // Camera entity
        m_entityTypes.insert(EntityTypeMap::value_type(Camera::getStaticType(), Camera::clone));
        // Sequence entity
        m_entityTypes.insert(EntityTypeMap::value_type(Sequence::getStaticType(), Sequence::clone));
        // RecordingUnit entity
        m_entityTypes.insert(EntityTypeMap::value_type(RecordingUnit::getStaticType(), RecordingUnit::clone));
        // Quad entity
        m_entityTypes.insert(EntityTypeMap::value_type(Quad::getStaticType(), Quad::clone));
        // BVSStage entity
        m_entityTypes.insert(EntityTypeMap::value_type(BVSStage::getStaticType(), BVSStage::clone));
        // VideoOutputGroup entity
        m_entityTypes.insert(EntityTypeMap::value_type(VideoOutputGroup::getStaticType(), VideoOutputGroup::clone));
        // VideoSwitchAgent entity
        m_entityTypes.insert(EntityTypeMap::value_type(VideoSwitchAgent::getStaticType(), VideoSwitchAgent::clone));
        // Plan Agent entity
        m_entityTypes.insert(EntityTypeMap::value_type(PlanAgentData::getStaticType(), PlanAgentData::clone));
        // Plan Manager entity
        m_entityTypes.insert(EntityTypeMap::value_type(PlanManagerEntityData::getStaticType(), PlanManagerEntityData::clone));
        // Alarm Agent entity
        m_entityTypes.insert(EntityTypeMap::value_type(AlarmAgentEntityData::getStaticType(), AlarmAgentEntityData::clone));
        // History Viewer entity
        m_entityTypes.insert(EntityTypeMap::value_type(HistoryViewerData::getStaticType(), HistoryViewerData::clone));
        // ScadaHistory Agent entity
        m_entityTypes.insert(EntityTypeMap::value_type(ScadaHistoryAgentEntityData::getStaticType(), ScadaHistoryAgentEntityData::clone));
        // ScadaHistory Config data
        m_entityTypes.insert(EntityTypeMap::value_type(ScadaHistoryConfigData::getStaticType(), ScadaHistoryConfigData::clone));
        // NotificationAgent entity
        m_entityTypes.insert(EntityTypeMap::value_type(NotificationAgentData::getStaticType(), NotificationAgentData::clone));
        m_entityTypes.insert(EntityTypeMap::value_type(System::getStaticType(), System::clone));
        m_entityTypes.insert(EntityTypeMap::value_type(Process::getStaticType(), Process::clone));
        // Control Station entity
        m_entityTypes.insert(EntityTypeMap::value_type(ControlStation::getStaticType(), ControlStation::clone));
        // Alarm Viewer entity
        m_entityTypes.insert(EntityTypeMap::value_type(AlarmGUI::getStaticType(), AlarmGUI::clone));
        // PMSAgent entity
        m_entityTypes.insert(EntityTypeMap::value_type(PMSAgentEntityData::getStaticType(), PMSAgentEntityData::clone));
        
        // EMSAgent entity
        m_entityTypes.insert(EntityTypeMap::value_type( EMSAgentEntityData::getStaticType(), EMSAgentEntityData::clone ) );

        // PEnergyUnit entity //Liu Yong 2016-07-22 for FZL1 PEnergyUnit
        m_entityTypes.insert(EntityTypeMap::value_type(PEnergyUnitEntityData::getStaticType(), PEnergyUnitEntityData::clone));
		
        // PEnergyAgent entity //Liu Yong 2016-07-22 for FZL1 PEnergyAgent
        m_entityTypes.insert(EntityTypeMap::value_type(PEnergyAgentEntityData::getStaticType(), PEnergyAgentEntityData::clone));
		
		// SNMPAgent entity
        m_entityTypes.insert(EntityTypeMap::value_type( SNMPStatusAgentEntityData::getStaticType(), SNMPStatusAgentEntityData::clone ) );

        // VirtualDataPointAgent entity
        m_entityTypes.insert(EntityTypeMap::value_type(VirtualDataPointAgentEntityData::getStaticType(), VirtualDataPointAgentEntityData::clone));
        // StationECSAgent entity
        m_entityTypes.insert(EntityTypeMap::value_type(StationECSAgentEntityData::getStaticType(), StationECSAgentEntityData::clone));
        // MasterECSAgent entity
        m_entityTypes.insert(EntityTypeMap::value_type(MasterECSAgentEntityData::getStaticType(), MasterECSAgentEntityData::clone));
        // RTU entity
        m_entityTypes.insert(EntityTypeMap::value_type(RTUEntityData::getStaticType(), RTUEntityData::clone));
        // Station entity
        m_entityTypes.insert(EntityTypeMap::value_type(StationEntityData::getStaticType(), StationEntityData::clone));
        // StationSystem entity
        m_entityTypes.insert(EntityTypeMap::value_type(StationSystemEntityData::getStaticType(), StationSystemEntityData::clone));
        // StationSubSystem entity
        m_entityTypes.insert(EntityTypeMap::value_type(StationSubSystemEntityData::getStaticType(), StationSubSystemEntityData::clone));
        // Equipment entity
        m_entityTypes.insert(EntityTypeMap::value_type(EquipmentEntityData::getStaticType(), EquipmentEntityData::clone));
        // DataPoint entity
        m_entityTypes.insert(EntityTypeMap::value_type(DataPointEntityData::getStaticType(), DataPointEntityData::clone));
        // AlarmVolumeTestEntityType
        m_entityTypes.insert(EntityTypeMap::value_type(AlarmVolumeTestEntityType::getStaticType(), AlarmVolumeTestEntityType::clone));
        // Banner entity type
        m_entityTypes.insert(EntityTypeMap::value_type(Banner::getStaticType(), Banner::clone));
        // Rights Agent entity
        m_entityTypes.insert(EntityTypeMap::value_type(RightsAgentEntityData::getStaticType(), RightsAgentEntityData::clone));
        // Duty Agent entity
        m_entityTypes.insert(EntityTypeMap::value_type(DutyAgentEntityData::getStaticType(), DutyAgentEntityData::clone));
        // DataNode entity
        m_entityTypes.insert(EntityTypeMap::value_type(DataNodeEntityData::getStaticType(), DataNodeEntityData::clone));
        // EventViewer entity
        m_entityTypes.insert(EntityTypeMap::value_type(EventViewer::getStaticType(), EventViewer::clone));
        // DataNodeAgent entity
        m_entityTypes.insert(EntityTypeMap::value_type(DataNodeAgentEntityData::getStaticType(), DataNodeAgentEntityData::clone));
        // Configuration Editor entity
        m_entityTypes.insert(EntityTypeMap::value_type(ConfigurationEditor::getStaticType(), ConfigurationEditor::clone));
        // ECSManager Entity
        m_entityTypes.insert(EntityTypeMap::value_type(ECSManager::getStaticType(), ECSManager::clone));
        // Radio Entity
        m_entityTypes.insert(EntityTypeMap::value_type(RadioEntityData::getStaticType(), RadioEntityData::clone));
        // Radio Session Entity
        m_entityTypes.insert(EntityTypeMap::value_type(RadioSessionEntityData::getStaticType(), RadioSessionEntityData::clone));
        // Radio State Synchronisation Entity
        m_entityTypes.insert(EntityTypeMap::value_type(RadioStateSynchronisationEntityData::getStaticType(), RadioStateSynchronisationEntityData::clone));
        // TIS Agent Entity
        m_entityTypes.insert(EntityTypeMap::value_type(TISAgentEntityData::getStaticType(), TISAgentEntityData::clone));
        // STIS Entity
        m_entityTypes.insert(EntityTypeMap::value_type(STISEntityData::getStaticType(), STISEntityData::clone));
        m_entityTypes.insert(EntityTypeMap::value_type(PIDSEntityData::getStaticType(), PIDSEntityData::clone) );
        // STIS Entity
        m_entityTypes.insert(EntityTypeMap::value_type(SchedulingAgentEntityData::getStaticType(), SchedulingAgentEntityData::clone));
        // StationPAAgent entity
        m_entityTypes.insert(EntityTypeMap::value_type(StationPAAgentEntityData::getStaticType(), StationPAAgentEntityData::clone));
        // MasterPAAgent entity
        m_entityTypes.insert(EntityTypeMap::value_type(MasterPAAgentEntityData::getStaticType(), MasterPAAgentEntityData::clone));
        // WILDAgent entity
        m_entityTypes.insert(EntityTypeMap::value_type(WILDAgentEntityData::getStaticType(), WILDAgentEntityData::clone));
        // AlarmStore entity
        m_entityTypes.insert(EntityTypeMap::value_type(AlarmStoreEntityData::getStaticType(), AlarmStoreEntityData::clone));
        // System Status entity
        m_entityTypes.insert(EntityTypeMap::value_type(SystemStatusAgentEntityData::getStaticType(), SystemStatusAgentEntityData::clone));
        // AtsAgent entity
        m_entityTypes.insert(EntityTypeMap::value_type(AtsAgentEntityData::getStaticType(), AtsAgentEntityData::clone));
        // Radio Directory entity
        m_entityTypes.insert(EntityTypeMap::value_type(RadioDirectoryEntityData::getStaticType(), RadioDirectoryEntityData::clone));
        // Authentication Agent entity
        m_entityTypes.insert(EntityTypeMap::value_type(AuthenticationAgentEntityData::getStaticType(), AuthenticationAgentEntityData::clone));
        // Train Agent entity
        m_entityTypes.insert(EntityTypeMap::value_type(TrainAgentEntityData::getStaticType(), TrainAgentEntityData::clone));
        // Train Settings entity
        m_entityTypes.insert(EntityTypeMap::value_type(TrainSettingsEntityData::getStaticType(), TrainSettingsEntityData::clone));
        // Archive Manager entity
        m_entityTypes.insert(EntityTypeMap::value_type(ArchiveManagerEntityData::getStaticType(), ArchiveManagerEntityData::clone));
        // Various MMS data objects:
        m_entityTypes.insert(EntityTypeMap::value_type(MmsAgentEntityData::getStaticType(), MmsAgentEntityData::clone));
        m_entityTypes.insert(EntityTypeMap::value_type(MmsAlarmSubmitterEntityData::getStaticType(), MmsAlarmSubmitterEntityData::clone));
        m_entityTypes.insert(EntityTypeMap::value_type(MmsJobRequestSubmitterEntityData::getStaticType(), MmsJobRequestSubmitterEntityData::clone));
        m_entityTypes.insert(EntityTypeMap::value_type(MmsPeriodicEntityData::getStaticType(), MmsPeriodicEntityData::clone));
        m_entityTypes.insert(EntityTypeMap::value_type(MmsConnectiontEntityData::getStaticType(), MmsConnectiontEntityData::clone));
        // Display Manager entity
        m_entityTypes.insert(EntityTypeMap::value_type(DisplayManagerEntityData::getStaticType(), DisplayManagerEntityData::clone));
        // Region Manager entity
        m_entityTypes.insert(EntityTypeMap::value_type(RegionManagerEntityData::getStaticType(), RegionManagerEntityData::clone));
        // Duty Manager entity
        m_entityTypes.insert(EntityTypeMap::value_type(DutyManagerEntityData::getStaticType(), DutyManagerEntityData::clone));
        // TIS Log Viewer entity
        m_entityTypes.insert(EntityTypeMap::value_type(TisLogViewerEntityData::getStaticType(), TisLogViewerEntityData::clone));
        // Online Printing Agent entity
        m_entityTypes.insert(EntityTypeMap::value_type(OnlinePrintingAgentEntityData::getStaticType(), OnlinePrintingAgentEntityData::clone));
        // Train CCTV Camera entity
        m_entityTypes.insert(EntityTypeMap::value_type(TrainCctvCameraEntityData::getStaticType(), TrainCctvCameraEntityData::clone));
        // Call Banner Page entity
        m_entityTypes.insert(EntityTypeMap::value_type(CallBannerPage::getStaticType(), CallBannerPage::clone));
        // Less Agent Entity
        m_entityTypes.insert(EntityTypeMap::value_type(LessAgentEntityData::getStaticType(), LessAgentEntityData::clone));
        // Equipment Status Viewer entity
        m_entityTypes.insert(EntityTypeMap::value_type(EquipmentStatusViewerEntityData::getStaticType(), EquipmentStatusViewerEntityData::clone));
        // Radio Manager/MFT
        m_entityTypes.insert(EntityTypeMap::value_type(RadioProfileEntityData::getStaticType(), RadioProfileEntityData::clone));
        // Telephone Manager Profile entity
        m_entityTypes.insert(EntityTypeMap::value_type(TelephoneManagerProfileEntityData::getStaticType(), TelephoneManagerProfileEntityData::clone));
        // PID Log Viewer entity
        m_entityTypes.insert(EntityTypeMap::value_type(PidLogViewerEntityData::getStaticType(), PidLogViewerEntityData::clone));
        // wenching++ (TD12997)
        // Inspector Panel entity
        m_entityTypes.insert(EntityTypeMap::value_type(InspectorPanelEntityData::getStaticType(), InspectorPanelEntityData::clone));
        // ++wenching (TD12997)
        // wenching++ (TD14000)
        // Power Demand Editor entity
        m_entityTypes.insert(EntityTypeMap::value_type(PowerDemandEditorEntityData::getStaticType(), PowerDemandEditorEntityData::clone));
        // ++wenching (TD14000)
        // Raymond Pau++ (TD13367)
        // Scada Root entity
        m_entityTypes.insert(EntityTypeMap::value_type(ScadaRootEntityData::getStaticType(), ScadaRootEntityData::clone));
        // ++Raymond Pau(TD13367)
        m_entityTypes.insert(EntityTypeMap::value_type(ServerEntityData::getStaticType(), ServerEntityData::clone));
        //TD15213: add AtsTwp and AtsPsd entity
        m_entityTypes.insert(EntityTypeMap::value_type(AtsTwpEntityData::getStaticType(), AtsTwpEntityData::clone));
        m_entityTypes.insert(EntityTypeMap::value_type(AtsPsdEntityData::getStaticType(), AtsPsdEntityData::clone));
        //TD15918 (Mintao++)
        m_entityTypes.insert(EntityTypeMap::value_type(MisAgentEntityData::getStaticType(), MisAgentEntityData::clone));
        m_entityTypes.insert(EntityTypeMap::value_type(MisAlarmSubmitterEntityData::getStaticType(), MisAlarmSubmitterEntityData::clone));
        m_entityTypes.insert(EntityTypeMap::value_type(MisJobRequestSubmitterEntityData::getStaticType(), MisJobRequestSubmitterEntityData::clone));
        m_entityTypes.insert(EntityTypeMap::value_type(MisPeriodicEntityData::getStaticType(), MisPeriodicEntityData::clone));
        m_entityTypes.insert(EntityTypeMap::value_type(MisConnectiontEntityData::getStaticType(), MisConnectiontEntityData::clone));
        //lizettejl++ (TD14697)
        m_entityTypes.insert(EntityTypeMap::value_type(HierarchicalAlarmViewEntityData::getStaticType(), HierarchicalAlarmViewEntityData::clone));
        //TD17997 marc
        //Support for Radio Global entity
        m_entityTypes.insert(EntityTypeMap::value_type(RadioGlobalEntityData::getStaticType(), RadioGlobalEntityData::clone));
        // add TTIS and STIS entity (lichao++)
        m_entityTypes.insert(EntityTypeMap::value_type(TTISManagerEntityData::getStaticType(), TTISManagerEntityData::clone));
		// for GF BAS agent entity data
		m_entityTypes.insert(EntityTypeMap::value_type(StationBASAgentEntityData::getStaticType(), StationBASAgentEntityData::clone) );
		//for SOE 		 
		m_entityTypes.insert(EntityTypeMap::value_type(SOEEventViewer::getStaticType(), SOEEventViewer::clone) );
		m_entityTypes.insert(EntityTypeMap::value_type(DataLogAgentEntityData::getStaticType(), DataLogAgentEntityData::clone) );
		m_entityTypes.insert(EntityTypeMap::value_type(SNMPDeviceEntityData::getStaticType(), SNMPDeviceEntityData::clone) );
		// add for Event Agent //++ Noel
		m_entityTypes.insert(EntityTypeMap::value_type(EventAgentEntityData::getStaticType(), EventAgentEntityData::clone) );

		// add for GENA Library //++ Noel
		m_entityTypes.insert(EntityTypeMap::value_type(GenaConfigEntity::getStaticType(), GenaConfigEntity::clone) );

		// ModbusServerAgentEntityData entity
        m_entityTypes.insert(EntityTypeMap::value_type(ModbusServerAgentEntityData::getStaticType(), ModbusServerAgentEntityData::clone ) );
		
        LOG(SourceInfo, DebugUtil::FunctionExit, "EntityAccessFactory::setUpMap");
    }


    IEntityData* EntityAccessFactory::createEntityFromType(std::string type, unsigned long key)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "EntityAccessFactory::createEntityFromType");
        EntityTypeMap::iterator iter(m_entityTypes.find(type));
        if(iter == m_entityTypes.end())  // The type is not known to the system
        {
            return new DefaultEntity(key, type);
        }
        else
        {
            LOG(SourceInfo, DebugUtil::FunctionExit, "EntityAccessFactory::createEntityFromType");
            return iter->second(key);
        }
    }


    IData* EntityAccessFactory::getSingularData(IDatabase* pDatabase, SQLStatement& sql, std::vector<std::string> columnNames,
                                                std::string itemSpecification)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "EntityAccessFactory::getSingularData");
        // get a connection to the database
        /*IDatabase* databaseConnection =
        TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read)*/;
        TA_ASSERT(0 != pDatabase, "the database connection is null");
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // This data pointer is returned, so it is not deleted unless an exception is thrown.
        IData* data = pDatabase->executeQuery(sql, columnNames);
        // Need to bring in the DataException
        using TA_Base_Core::DataException;
        if(0 == data->getNumRows())  // No entry found with the specified name
        {
            // clean up the pointer
            delete data;
            data = NULL;
            std::string message = "No data found for " + itemSpecification;
            //TA_THROW(DataException(message.c_str(),DataException::NO_VALUE,sql));
            std::string strExSql;
            int nExDbType = pDatabase->getDbServerType();
            switch(nExDbType)
            {
                case enumOracleDb:
                    strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
                    break;
                case enumMysqlDb:
                    strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
                    break;
            }
            TA_THROW(DataException(message.c_str(), DataException::NO_VALUE, strExSql));
        }
        else if(1 < data->getNumRows())  // More than one entry found for the name
        {
            // clean up the pointer
            delete data;
            data = NULL;
            std::string message = "More than one entry found for " + itemSpecification;
            //TA_THROW(DataException(message.c_str(),DataException::NOT_UNIQUE, sql));
            std::string strExSql;
            int nExDbType = pDatabase->getDbServerType();
            switch(nExDbType)
            {
                case enumOracleDb:
                    strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
                    break;
                case enumMysqlDb:
                    strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
                    break;
            }
            TA_THROW(DataException(message.c_str(), DataException::NOT_UNIQUE, strExSql));
        }
        LOG(SourceInfo, DebugUtil::FunctionExit, "EntityAccessFactory::getSingularData");
        return data;
    }


    void EntityAccessFactory::getCorbaNames(IDatabase* pDatabase, const SQLStatement& sql, CorbaNameList& corbaNames)
    {
        // Set up the columnNames vector to be passed to executeQuery()
        std::string entityKeyColumn = "ENTITYKEY";
        std::string entityNameColumn = "ENTITYNAME";
        std::string agentNameColumn = "AGENTNAME";
        std::string locationKeyColumn = "LOCATIONKEY";
        std::vector<std::string> columnNames;
        columnNames.push_back(entityKeyColumn);
        columnNames.push_back(entityNameColumn);
        columnNames.push_back(agentNameColumn);
        columnNames.push_back(locationKeyColumn);
        // get a connection to the database
        /*IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);*/
        TA_ASSERT(0 != pDatabase, "the database connection is null");
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        // We use this call direct, as we *want* multiple values returned
        IData* data = pDatabase->executeQuery(sql, columnNames);
        do
        {
            for(unsigned long i = 0; i < data->getNumRows() ; i++)
            {
                // Add the returned CorbaNames to the list to return
                corbaNames.push_back(
                    CorbaName(data->getUnsignedLongData(i, entityKeyColumn),
                              data->getStringData(i, agentNameColumn),
                              data->getStringData(i, entityNameColumn),
                              data->getUnsignedLongData(i, locationKeyColumn)));
            }
            delete data;
            data = NULL;
        }
        while(pDatabase->moreData(data));
    }


    void EntityAccessFactory::getEntityData(IDatabase* pDatabase, const SQLStatement& sql, IEntityDataList& entityData, bool readWrite /*= false*/)
    {
        // Set up the columns
        std::string typeColumn = "TYPEKEY";
        std::string nameColumn = "NAME";
        std::string addressColumn = "ADDRESS";
        std::string descriptionColumn = "DESCRIPTION";
        std::string subsystemColumn = "SUBSYSTEMKEY";
        std::string subsystemNameColumn = "SUBSYSTEMNAME";
        std::string physicalSubsystemColumn = "PHYSICAL_SUBSYSTEM_KEY";
        std::string physicalSubsystemNameColumn = "PHYSICAL_SUBSYSTEM_NAME";
        std::string locationColumn = "LOCATIONKEY";
        std::string locationNameColumn = "LOCATIONNAME";
        std::string regionColumn = "SEREGI_ID";
        std::string regionNameColumn = "SEREGINAME";
        std::string parentColumn = "PARENTKEY";
        std::string parentNameColumn = "PARENTNAME";
        std::string agentColumn = "AGENTKEY";
        std::string agentNameColumn = "AGENTNAME";
        std::string dateCreatedColumn = "DATECREATED";
        std::string dateModifiedColumn = "DATEMODIFIED";
        std::string keyColumn = "PKEY";
        std::string typeNameColumn = "TYPENAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(typeColumn);
        columnNames.push_back(nameColumn);
        columnNames.push_back(addressColumn);
        columnNames.push_back(descriptionColumn);
        columnNames.push_back(subsystemColumn);
        columnNames.push_back(subsystemNameColumn);
        columnNames.push_back(physicalSubsystemColumn);
        columnNames.push_back(physicalSubsystemNameColumn);
        columnNames.push_back(locationColumn);
        columnNames.push_back(locationNameColumn);
        columnNames.push_back(regionColumn);
        columnNames.push_back(regionNameColumn);
        columnNames.push_back(parentColumn);
        columnNames.push_back(parentNameColumn);
        columnNames.push_back(agentColumn);
        columnNames.push_back(agentNameColumn);
        columnNames.push_back(dateCreatedColumn);
        columnNames.push_back(dateModifiedColumn);
        columnNames.push_back(keyColumn);
        columnNames.push_back(typeNameColumn);
        // Execute the query. The method can throw a DatabaseException.
        // We are responsible for deleting the returned IData object when we're done with it
        // We use this call direct, as we *want* multiple values returned
        // get a connection to the database
        /*IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);*/
        TA_ASSERT(0 != pDatabase, "the database connection is null");
        IData* data = pDatabase->executeQuery(sql, columnNames);
        // Create the entities
        IEntityDataList entityPointerVector;
        do
        {
            for(unsigned long i = 0; i < data->getNumRows() ; i++)
            {
                IEntityData* iEntity;
                if(! readWrite)
                {
                    iEntity = createEntityFromType(data->getStringData(i, typeNameColumn),
                                                   data->getUnsignedLongData(i, keyColumn));
                    // Cast to an entity data and assign the value
                    EntityData* entity = dynamic_cast< EntityData* >(iEntity);
                    // If the cast succeeds, assign the data
                    if(NULL != entity)
                    {
                        entity->assignDefaultData(data, i);
                    }
                }
                else // ConfigEntity
                {
                    iEntity =  new ConfigEntity(data->getUnsignedLongData(i, keyColumn),
                                                data->getStringData(i, typeNameColumn));
                    // Cast to a Config Entity data and assign the value
                    ConfigEntity* configEntity = dynamic_cast< ConfigEntity* >(iEntity);
                    // If the cast succeeds, assign the data
                    if(NULL != configEntity)
                    {
                        configEntity->assignDefaultData(data, i);
                    }
                }
                entityData.push_back(iEntity);
            }
            delete data;
            data = NULL;
        }
        while(pDatabase->moreData(data));
    }


    std::map<unsigned long, std::string> EntityAccessFactory::getEntityTypes()
    {
        FUNCTION_ENTRY("getEntityTypes");
        TA_Base_Core::ThreadGuard guard(m_entityTypesCacheLock);
        if(!m_entityTypesCache.empty())
        {
            return m_entityTypesCache;
        }
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        //      std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_STD_SELECT_2515);
        //      std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2515);
        SQLStatement strSql;
        databaseConnection->prepareSQLStatement(strSql, ENTITY_V_SELECT_2515);
        //      TA_ASSERT(strSql.size() > 0, "EntityAccessFactory::getEntityTypes(): Prepare SQL Statement error");
        //std::string sql("select pkey,name from entitytype where pkey <> 0");
        std::vector<std::string> columnNames;
        columnNames.push_back("key");
        columnNames.push_back("name");
        IData* data = databaseConnection->executeQuery(strSql, columnNames);
        do
        {
            for(int i = 0; i < data->getNumRows(); ++i)
            {
                m_entityTypesCache.insert(std::map<unsigned long, std::string>::value_type(data->getUnsignedLongData(i, "key"),
                                                                                           data->getStringData(i, "name")));
            }
            delete data;
            data = NULL;
        }
        while(databaseConnection->moreData(data));
        return m_entityTypesCache;
    }


    std::map<unsigned long, std::string> EntityAccessFactory::getChildTypesOf(const std::string& entityType)
    {
        FUNCTION_ENTRY("getChildTypesOf");
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        /*std::string sql("select p.et_key, t.name from entitytypeparent p, entitytype t where ");
        sql += "p.et_parentkey = (select pkey from entitytype where name = '";
        sql += databaseConnection->escapeQueryString(entityType);
        sql += "') and p.et_key = t.pkey";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), Entitytype_STD_SELECT_4010,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITYTYPE_SELECT_4010,
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITYTYPE_SELECT_4010,
                                                databaseConnection->escapeQueryString(entityType));
        //      TA_ASSERT(sql.size() > 0, "getChildTypesOf: Get SQL Statement error");
        std::vector<std::string> columnNames;
        columnNames.push_back("key");
        columnNames.push_back("name");
        IData* data = databaseConnection->executeQuery(sql, columnNames);
        std::map<unsigned long, std::string> entityTypes;
        do
        {
            for(int i = 0; i < data->getNumRows(); ++i)
            {
                entityTypes.insert(std::map<unsigned long, std::string>::value_type(data->getUnsignedLongData(i, "key"),
                                                                                    data->getStringData(i, "name")));
            }
            delete data;
            data = NULL;
        }
        while(databaseConnection->moreData(data));
        return entityTypes;
    }

    template<typename T>
    void EntityAccessFactory::getEntityDataWithParameterValues(IDatabase* pDatabase, const SQLStatement& sql, const SQLStatement& parameterSQL,
                                                               std::vector<T>& entityData)
    {
        FUNCTION_ENTRY("EntityAccessFactory::getEntityDataWithParameterValues");
        EntityKeyToEntityMap entityDataMap;
        EntityKeyToEntityMapIter entityDataMapIter;
        //for those entities that don't have any parameter values
        EntityKeyToEntityMap entityDataMapWithoutEPV;
        EntityKeyToEntityMapIter entityDataMapWithoutEPVIter;
        // Set up the columns
        std::string typeColumn = "TYPEKEY";
        std::string nameColumn = "NAME";
        std::string addressColumn = "ADDRESS";
        std::string descriptionColumn = "DESCRIPTION";
        std::string subsystemColumn = "SUBSYSTEMKEY";
        std::string subsystemNameColumn = "SUBSYSTEMNAME";
        std::string physicalSubsystemColumn = "PHYSICAL_SUBSYSTEM_KEY";
        std::string physicalSubsystemNameColumn = "PHYSICAL_SUBSYSTEM_NAME";
        std::string locationColumn = "LOCATIONKEY";
        std::string locationNameColumn = "LOCATIONNAME";
        std::string regionColumn = "SEREGI_ID";
        std::string regionNameColumn = "SEREGINAME";
        std::string parentColumn = "PARENTKEY";
        std::string parentNameColumn = "PARENTNAME";
        std::string agentColumn = "AGENTKEY";
        std::string agentNameColumn = "AGENTNAME";
        std::string dateCreatedColumn = "DATECREATED";
        std::string dateModifiedColumn = "DATEMODIFIED";
        std::string keyColumn = "PKEY";
        std::string typeNameColumn = "TYPENAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(typeColumn);
        columnNames.push_back(nameColumn);
        columnNames.push_back(addressColumn);
        columnNames.push_back(descriptionColumn);
        columnNames.push_back(subsystemColumn);
        columnNames.push_back(subsystemNameColumn);
        columnNames.push_back(physicalSubsystemColumn);
        columnNames.push_back(physicalSubsystemNameColumn);
        columnNames.push_back(locationColumn);
        columnNames.push_back(locationNameColumn);
        columnNames.push_back(regionColumn);
        columnNames.push_back(regionNameColumn);
        columnNames.push_back(parentColumn);
        columnNames.push_back(parentNameColumn);
        columnNames.push_back(agentColumn);
        columnNames.push_back(agentNameColumn);
        columnNames.push_back(dateCreatedColumn);
        columnNames.push_back(dateModifiedColumn);
        columnNames.push_back(keyColumn);
        columnNames.push_back(typeNameColumn);
        // Execute the query. The method can throw a DatabaseException.
        // We are responsible for deleting the returned IData object when we're done with it
        // We use this call direct, as we *want* multiple values returned
        // get a connection to the database
        /*IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);*/
        TA_ASSERT(0 != pDatabase, "the database connection is null");
        IData* data = pDatabase->executeQuery(sql, columnNames);
        unsigned long entityKey = 0;
        do
        {
            for(unsigned long i = 0; i < data->getNumRows() ; i++)
            {
                IEntityData* iEntity;
                entityKey = data->getUnsignedLongData(i, keyColumn);
                iEntity = createEntityFromType(data->getStringData(i, typeNameColumn),
                                               entityKey);
                // Cast to an entity data and assign the value
                EntityData* entity = dynamic_cast< EntityData* >(iEntity);
                // If the cast succeeds, assign the data
                if(NULL != entity)
                {
                    entity->assignDefaultData(data, i);
                    entityDataMap.insert(EntityKeyToEntityMap::value_type(entityKey, entity));
                    entityDataMapWithoutEPV.insert(EntityKeyToEntityMap::value_type(entityKey, entity));
                }
                T temp(iEntity);
                entityData.push_back(temp);
            }
            delete data;
            data = NULL;
        }
        while(pDatabase->moreData(data));
        if(entityData.size() != 0)
        {
            //get the entity parameter value
            std::string entityKeyColumn = "ENTITYKEY";
            std::string parameterNameColumn = "PARAMETERNAME";
            std::string parameterValueColumn = "VALUE";
            columnNames.clear();
            columnNames.push_back(entityKeyColumn);
            columnNames.push_back(parameterNameColumn);
            columnNames.push_back(parameterValueColumn);
            data = pDatabase->executeQuery(parameterSQL, columnNames);
            entityKey = 0;
            entityDataMapIter = entityDataMap.begin();
            do
            {
                for(unsigned long i = 0; i < data->getNumRows() ; i++)
                {
                    entityKey = data->getUnsignedLongData(i, entityKeyColumn);
                    //in most time the data with same entity key is together. following
                    //process just want to reduce some of the searching map time.
                    if((entityDataMapIter->second)->getKey() == entityKey)
                    {
                        (entityDataMapIter->second)->assignDefaultParameterValueData(data, i);
                    }
                    else
                    {
                        entityDataMapIter = entityDataMap.find(entityKey);
                        if(entityDataMapIter != entityDataMap.end())
                        {
                            (entityDataMapIter->second)->assignDefaultParameterValueData(data, i);
                        }
                        else
                        {
                            entityDataMapIter = entityDataMap.begin();
                        }
                    }
                    entityDataMapWithoutEPVIter = entityDataMapWithoutEPV.find(entityKey);
                    if(entityDataMapWithoutEPVIter != entityDataMapWithoutEPV.end())
                    {
                        entityDataMapWithoutEPV.erase(entityDataMapWithoutEPVIter);
                    }
                }
                delete data;
                data = NULL;
            }
            while(pDatabase->moreData(data));
            // for those entities that don't have any parametervalues,
            // need to assign default data to avoid loading from database.
            for(entityDataMapWithoutEPVIter = entityDataMapWithoutEPV.begin();
                    entityDataMapWithoutEPVIter != entityDataMapWithoutEPV.end();
                    ++entityDataMapWithoutEPVIter)
            {
                // to assign a dummy parameter so that all parameters will be using default values instead of loading from db again.
                (entityDataMapWithoutEPVIter->second)->assignDefaultParameterValueData("DUMMY", "DEFAULT");
            }
        }
        FUNCTION_EXIT;
    }


    std::string EntityAccessFactory::getParameterDefaultValue(const std::string& name, unsigned long typeKey)
    {
        FUNCTION_ENTRY("getParameterDefaultValue");
        if(m_entityParameterCache.empty())
        {
            createParameterCache();
        }
        std::string ret("");
        std::multimap< std::string, EntityParameter >::iterator lowerIter;
        std::multimap< std::string, EntityParameter >::iterator upperIter;
        lowerIter = m_entityParameterCache.lower_bound(name);
        upperIter = m_entityParameterCache.upper_bound(name);
        for(; lowerIter != upperIter; ++lowerIter)
        {
            if(lowerIter->second.typeKey = typeKey)
            {
                ret = lowerIter->second.defaultValue;
            }
        }
        FUNCTION_EXIT;
        return ret;
    }

    void EntityAccessFactory::createParameterCache()
    {
        FUNCTION_ENTRY("getParameterDefaultValue");
        TA_Base_Core::ThreadGuard guard(m_parameterLock);
        if(m_entityParameterCache.empty())
        {
            IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
            //          std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_STD_SELECT_2514);
            //          std::string strSql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2514);
            SQLStatement strSql;
            databaseConnection->prepareSQLStatement(strSql, ENTITY_V_SELECT_2514);
            //          TA_ASSERT(strSql.size() > 0, "EntityAccessFactory::createParameterCache(): Prepare SQL Statement error");
            //std::string sql("SELECT NAME, TYPEKEY, EP_DEFAULT FROM entityparameter");
            std::string parameterName("NAME");
            std::string typeKey("TYPEKEY");
            std::string defaultVaule("EP_DEFAULT");
            std::vector<std::string> columnNames;
            columnNames.push_back(parameterName);
            columnNames.push_back(typeKey);
            columnNames.push_back(defaultVaule);
            IData* data = databaseConnection->executeQuery(strSql, columnNames);
            EntityParameter temp;
            do
            {
                for(int i = 0; i < data->getNumRows(); ++i)
                {
                    temp.typeKey = data->getUnsignedLongData(i, typeKey);
                    temp.defaultValue = data->getStringData(i, defaultVaule);
                    m_entityParameterCache.insert(std::make_pair(data->getStringData(i, parameterName), temp));
                }
                delete data;
                data = NULL;
            }
            while(databaseConnection->moreData(data));
        }
        FUNCTION_EXIT;
    }

    IEntityDataList EntityAccessFactory::getEntities(const std::vector< std::string >& entityNameSet)
    {
        FUNCTION_ENTRY("getEntities");
        IEntityDataList entities;
        if(!entityNameSet.empty())
        {
            // create the SQL string to retrieve the entity parameters pkey, and typeName
            // get a connection to the database
            IDatabase* databaseConnection =
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
            // note: upper() is used for case insensitivity.
            std::ostringstream sqlParameter;
            std::ostringstream parameterSQLVar;
            /*sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME,";
            sql << " e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
            sql << " e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
            sql << " TO_CHAR(e.DATE_MODIFIED,'YYYYMMDDHH24MISS'),";
            sql << " e.pkey, et.name FROM entity_v e, entitytype et where e.name in (";

            parameterSQL << "SELECT entitykey, parametername, value "
                << "from entityparametervalue_v "
                << "where entitykey in (select pkey FROM entity where name in (";*/
            std::vector< std::string >::const_iterator it = entityNameSet.begin();
            sqlParameter << " '" << *it << "' ";
            parameterSQLVar << " '" << *it << "' ";
            ++it;
            for(; it != entityNameSet.end(); ++it)
            {
                sqlParameter << ", " << " '" << *it << "' ";
                parameterSQLVar << ", " << " '" << *it << "' ";
            }
            /*sql << ") and et.pkey = e.typekey and e.deleted = 0";
            parameterSQL << ") and deleted = 0)";*/
            //          std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2571,
            //          std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2571,
            SQLStatement sql;
            databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2571,
                                                    sqlParameter.str());
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3069,
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3069,
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3069,
                                                    parameterSQLVar.str());
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        FUNCTION_EXIT;
        return entities;
    }

    IEntityDataList EntityAccessFactory::getEntities(const std::vector<unsigned long>& entityKeySet)
    {
        FUNCTION_ENTRY("getEntities");
        IEntityDataList entities;
        if (!entityKeySet.empty())
        {
            // create the SQL string to retrieve the entity parameters pkey, and typeName
            // get a connection to the database
            IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
            // note: upper() is used for case insensitivity.
            std::ostringstream sqlParameter;
            std::ostringstream parameterSQLVar;
            //sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME,";
            //sql << " e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
            //sql << " e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
            //sql << " TO_CHAR(e.DATE_MODIFIED,'YYYYMMDDHH24MISS'),";
            //sql << " e.pkey, et.name FROM entity_v e, entitytype et where e.pkey in (";

            //parameterSQL << "SELECT entitykey, parametername, value "
            //    << "from entityparametervalue_v "
            //    << "where entitykey in (";

            std::stringstream keys;

            for (size_t i = 0; i < entityKeySet.size(); ++i)
            {
                keys << (0 == i ? "" : ",") << entityKeySet[i];
            }

            sqlParameter << keys.str();
            parameterSQLVar << keys.str();

            /*sql << ") and et.pkey = e.typekey and e.deleted = 0";
            parameterSQL << ") and deleted = 0)";*/
            //          std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2571,
            //          std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2586,
            SQLStatement sql;
            databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2586,
                                                    sqlParameter.str());
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3069,
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3081,
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3081,
                                                    parameterSQLVar.str());
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        FUNCTION_EXIT;
        return entities;
    }
    
    IEntityKeyToEntityMap EntityAccessFactory::getKeyToEntityMap(const std::vector<unsigned long>& entityKeySet)
    {
        FUNCTION_ENTRY("getEntities");

        IEntityKeyToEntityMap keyToEntityMap;
        IEntityDataList entities;

        if (!entityKeySet.empty())
        {
            // create the SQL string to retrieve the entity parameters pkey, and typeName
            // get a connection to the database
            IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
            // note: upper() is used for case insensitivity.
            std::ostringstream sqlParameter;
            std::ostringstream parameterSQLVar;
            //sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME,";
            //sql << " e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
            //sql << " e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
            //sql << " TO_CHAR(e.DATE_MODIFIED,'YYYYMMDDHH24MISS'),";
            //sql << " e.pkey, et.name FROM entity_v e, entitytype et where e.pkey in (";

            //parameterSQL << "SELECT entitykey, parametername, value "
            //    << "from entityparametervalue_v "
            //    << "where entitykey in (";

            std::stringstream keys;

            for (size_t i = 0; i < entityKeySet.size(); ++i)
            {
                keys << (0 == i ? "" : ",") << entityKeySet[i];
            }

            sqlParameter << keys.str();
            parameterSQLVar << keys.str();

            /*sql << ") and et.pkey = e.typekey and e.deleted = 0";
            parameterSQL << ") and deleted = 0)";*/
            //          std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2571,
            //          std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2586,
            SQLStatement sql;
            databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2586,
                                                    sqlParameter.str());
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entityparametervalue_v_Oracle_SELECT_3069,
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3081,
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3081,
                                                    parameterSQLVar.str());
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);

            BOOST_FOREACH(unsigned long key, entityKeySet)
            {
                BOOST_FOREACH(IEntityData* entity, entities)
                {
                    if (entity->getKey() == key)
                    {
                        keyToEntityMap[key] = entity;
                        break;
                    }
                }
            }
        }

        FUNCTION_EXIT;
        return keyToEntityMap;
    }

    std::vector<std::string> EntityAccessFactory::getAgentNamesOfTypeAtLocation(const std::string entityType, unsigned long locationKey)
    {
        FUNCTION_ENTRY("getAgentNamesOfTypeAtLocation");
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        //std::string sql("");
        SQLStatement sql;
        if(0 == locationKey)
        {
            //          sql = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2519, databaseConnection->escapeQueryString(entityType));
            databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2519, databaseConnection->escapeQueryString(entityType));
        }
        else
        {
            //          sql = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2520, databaseConnection->escapeQueryString(entityType), locationKey);
            databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2520, databaseConnection->escapeQueryString(entityType), locationKey);
        }
        //      TA_ASSERT(sql.size() > 0, "getAgentNamesOfTypeAtLocation: Get SQL Statement error");
        std::vector<std::string> agentNames;
        std::vector<std::string> columnNames;
        columnNames.push_back("AgentName");
        IData* data = databaseConnection->executeQuery(sql, columnNames);
        do
        {
            for(int i = 0; i < data->getNumRows(); ++i)
            {
                agentNames.push_back(data->getStringData(i, "AgentName"));
            }
            delete data;
            data = NULL;
        }
        while(databaseConnection->moreData(data));
        FUNCTION_EXIT;
        return agentNames;
    }

    SharedIEntityDataList EntityAccessFactory::getEntitiesByName(const std::vector<std::string>& entityNameList)
    {
        FUNCTION_ENTRY("getEntitiesByName");
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        SharedIEntityDataList entities;
        if(entityNameList.empty())
        {
            FUNCTION_EXIT;
            return entities;
        }
        // now SQL in () can only handle 256 entries
        // so lets be smart, and make multiple in () clauses if we need to
        // YUCK!
        std::stringstream nameClauseString;
        std::vector<std::string>::const_iterator iter = entityNameList.begin();
        int i = 0;
        nameClauseString << " ( upper(e.name) in ( ";
        while(iter != entityNameList.end())
        {
            // add the entity name
            nameClauseString << "upper('" << databaseConnection->escapeQueryString(*iter) << "')";
            ++iter;
            ++i;
            // if 256 items are now inside the in ()
            if(i >= 256)
            {
                // close it off and start a new one
                nameClauseString << " ) OR upper(e.name) in ( ";
                i = 0;
            }
            else if(iter != entityNameList.end())
            {
                // put a comma for the next
                nameClauseString << ", ";
            }
        }
        nameClauseString << " ) ) ";
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME,";
        sql << " e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << " e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS'),";
        sql << " TO_CHAR(e.DATE_MODIFIED,'YYYYMMDDHH24MISS'),";
        sql << " e.pkey, et.name FROM entity_v e, entitytype et where ";
        sql <<  nameClauseString.str() << " and et.pkey = e.typekey and e.deleted = 0";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2572,
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2572, nameClauseString.str());
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2572, nameClauseString.str());
        //      TA_ASSERT(sql.size() > 0, "getEntitiesByName: Get SQL Statement error");
        // note the "entity_v e" so the same name sql can be used for both queries
        /*std::ostringstream parameterSQL;
        parameterSQL << "SELECT entitykey, parametername, value "
                     << "from entityparametervalue_v "
                     << "where entitykey in (select pkey FROM entity_v e where "
                     << nameClauseString.str() << " and deleted = 0 )";*/
        //      std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(),            entityparametervalue_v_Oracle_SELECT_3070, nameClauseString.str());
        //      std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3070, nameClauseString.str());
        SQLStatement parameterSQL;
        databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3070, nameClauseString.str());
        // NOTE, it is assumed that entities is not cleared in this function
        getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        FUNCTION_EXIT;
        return entities;
    }

    //liqiang++
    IEntityDataList EntityAccessFactory::getEntityWithNameInToken(const std::string& name, const bool readWrite /* = false */)
    {
        FUNCTION_ENTRY("getEntityWithNameInToken");
        // create the SQL string to retrieve the entity parameters pkey, and typeName
        // get a connection to the database
        IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);
        // note: upper() is used for case insensitivity.
        /*std::ostringstream sql;
        sql << "SELECT e.TYPEKEY, e.NAME, e.ADDRESS, e.DESCRIPTION, e.SUBSYSTEMKEY, e.SUBSYSTEMNAME,";
        sql << " e.PHYSICAL_SUBSYSTEM_KEY, e.PHYSICAL_SUBSYSTEM_NAME, e.LOCATIONKEY, e.LOCATIONNAME, e.SEREGI_ID, e.SEREGINAME, ";
        sql << " e.PARENTKEY, e.PARENTNAME, e.AGENTKEY, e.AGENTNAME, TO_CHAR(e.DATE_CREATED, 'YYYYMMDDHH24MISS') as DATE_CREATED,";
        sql << " TO_CHAR(e.DATE_MODIFIED,'YYYYMMDDHH24MISS') as DATE_MODIFIED,";
        sql << " e.pkey, et.name FROM entity_v e, entitytype et where upper(e.name) in (";
        sql <<  name << ") and et.pkey = e.typekey and e.deleted = 0";*/
        //      std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), entity_v_Oracle_SELECT_2573, name);
        //      std::string sql  = databaseConnection->prepareSQLStatement(ENTITY_V_SELECT_2573, name);
        SQLStatement sql;
        databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2573, name);
        //      TA_ASSERT(sql.size() > 0, "getEntityWithNameInToken: Get SQL Statement error");
        try
        {
            std::string strExSql;
            int nExDbType = databaseConnection->getDbServerType();
            switch(nExDbType)
            {
                case enumOracleDb:
                    strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
                    break;
                case enumMysqlDb:
                    strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
                    break;
            }
            TA_Base_Core::DebugUtil::getInstance().logLargeString(SourceInfo, DebugUtil::DebugDebug, strExSql);
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "WRITELOG", "Error while write logLargeString() of EntityAccessFactory::getEntityWithNameInToken() ");
        }
        IEntityDataList entities;
        if(readWrite == false)
        {
            /*std::ostringstream parameterSQL;
            parameterSQL << "SELECT entitykey, parametername, value "
                << "from entityparametervalue_v "
                << "where entitykey in (select pkey FROM entity_v where upper(name) in ("
                << name << ") and deleted = 0 )";*/
            //          std::string parameterSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(),                entityparametervalue_v_Oracle_SELECT_3071, name);
            //          std::string parameterSQL  = databaseConnection->prepareSQLStatement(ENTITYPARAMETERVALUE_V_SELECT_3071, name);
            SQLStatement parameterSQL;
            databaseConnection->prepareSQLStatement(parameterSQL, ENTITYPARAMETERVALUE_V_SELECT_3071, name);
            try
            {
                std::string strExSql;
                int nExDbType = databaseConnection->getDbServerType();
                switch(nExDbType)
                {
                    case enumOracleDb:
                        strExSql = (parameterSQL.strCommon.empty() ? parameterSQL.strOracle : parameterSQL.strCommon);
                        break;
                    case enumMysqlDb:
                        strExSql = (parameterSQL.strCommon.empty() ? parameterSQL.strMySQL : parameterSQL.strCommon);
                        break;
                }
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Follow string is the parameterSQL readWrite = false", "");
                TA_Base_Core::DebugUtil::getInstance().logLargeString(SourceInfo, DebugUtil::DebugDebug, strExSql);
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "WRITELOG", "Error while write logLargeString() of EntityAccessFactory::getEntityWithNameInToken() ");
            }
            getEntityDataWithParameterValues(databaseConnection, sql, parameterSQL, entities);
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, " readWrite = true", "");
            getEntityData(databaseConnection, sql, entities, readWrite);
        }
        // Check to see if there were any values returned
        if(entities.size() == 0)
        {
            std::string message = "No data found for " + name;
            std::string strExSql;
            int nExDbType = databaseConnection->getDbServerType();
            switch(nExDbType)
            {
                case enumOracleDb:
                    strExSql = (sql.strCommon.empty() ? sql.strOracle : sql.strCommon);
                    break;
                case enumMysqlDb:
                    strExSql = (sql.strCommon.empty() ? sql.strMySQL : sql.strCommon);
                    break;
            }
            TA_THROW(DataException(message.c_str(), DataException::NO_VALUE, strExSql));
        }
        // entity names are unique, therefore if we get to this point
        // there should be one and only one element in the vector
        //TA_ASSERT(1 == entities.size(), "Entity name unique constraint violated");
        FUNCTION_EXIT;
        return entities;
    }
    //++liqiang
    TA_Base_Core::IEntityDataList EntityAccessFactory::getFasAlarmEntities()
    {
        FUNCTION_ENTRY( "getEntityWithNameInToken" );

        // create the SQL string to retrieve the entity parameters pkey, and typeName
        // get a connection to the database
        IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(OnlineUpdatable_Cd, Read);

        /*std::ostringstream sql;
        sql << "select TYPEKEY, NAME, ADDRESS, DESCRIPTION, SUBSYSTEMKEY, SUBSYSTEMNAME,";
        sql << " PHYSICAL_SUBSYSTEM_KEY, PHYSICAL_SUBSYSTEM_NAME, LOCATIONKEY, LOCATIONNAME, SEREGI_ID, SEREGINAME, ";
        sql << " PARENTKEY, PARENTNAME, AGENTKEY, AGENTNAME, TO_CHAR(DATE_CREATED, 'YYYYMMDDHH24MISS') as DATE_CREATED,";
        sql << " TO_CHAR(DATE_MODIFIED,'YYYYMMDDHH24MISS') as DATE_MODIFIED, pkey, 'DataPoint' as TYPENAME";
        sql << " from entity_v where pkey in ";
        sql << "(select unique (entitykey) from entityparametervalue where parameterkey = ";
        sql << "( select pkey from entityparameter where name = 'SpecialAlarmType' ) and value = 'FireAlarm' )";*/

        IEntityDataList entities;
        //getEntityData( sql.str(), entities );

		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ENTITY_V_SELECT_2585);

		getEntityData(databaseConnection, sql, entities );

        FUNCTION_EXIT;
        return entities;
    }

} // closes TA_Base_Core



