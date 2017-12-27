/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/AlarmData.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2016/01/18 15:18:43 $
 * Last modified by:  $Author: Ouyang $
 *
 * IAlarmData is an interface to the Alarm table. It provides both read and write access
 * for agents and user applicaitons.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "ace/OS.h"

#include "core/data_access_interface/src/AlarmData.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
//#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/IAlarmSeverityData.h"
#include "core/data_access_interface/src/AlarmSeverityAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include <time.h>

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
	/**
	 *
	 * Const Definitions of Plan Status
	 *
	 */
	const std::string  IAlarmData::NO_ASSOC_PLAN ="NoAssociatedPlan";
	const std::string  IAlarmData::AUTO_PLAN_NOT_RUN="AutoPlansNotRun";
	const std::string  IAlarmData::SEMIAUTO_PLAN_NOT_RUN="SemiAutoPlansNotRun";
	const std::string  IAlarmData::PLAN_RUN="PlansRun";
	const std::string  IAlarmData::PLAN_POSTPONED="PlanPostponed";

	AlarmData::AlarmData( const std::string& key, const std::string& paramString,
		const timeb sourceTime,
		const timeb transactiveTime,
		const time_t ackTime,
        const time_t closeTime,
		unsigned long severityKey, 
		unsigned long typeKey,
		const std::string& context,
		EAlarmState state,
		EMmsState mmsStateType,
		const std::string& acknowledgedBy, 
		unsigned long entityKey, 
		const std::string& comment,
        const std::string& assetName,
        const std::string& planStatus,
        unsigned long subsystemKeyOfAssociatedEntity,
        unsigned long locationKeyOfAssociatedEntity,
        unsigned long typeOfAssociatedEntity,
        const std::string& operatorNameForAcknowledgedBySessionKey,
		const bool isUnique,
		const std::string& avlHeadId,
		EAlarmAVLStatus avlStatus)
		: m_isValidData( true ), m_key( key ), m_paramString( paramString ),
		  m_sourceTime( sourceTime ), m_transactiveTime( transactiveTime ), m_ackTime( ackTime ), m_closeTime( closeTime ), 
          m_severityKey( severityKey ), m_typeKey( typeKey ), m_context( context ), m_state( state ), m_mmsStateType( mmsStateType ), m_acknowledgedBy( acknowledgedBy ), 
          m_entityKey( entityKey ), m_comment( comment ), m_assetName(assetName), m_planStatus(planStatus), m_subsystemKeyOfAssociatedEntity( subsystemKeyOfAssociatedEntity ),
          m_locationKeyOfAssociatedEntity( locationKeyOfAssociatedEntity ), 
          m_typeOfAssociatedEntity( typeOfAssociatedEntity ), m_operatorNameForAcknowledgedBySessionKey( operatorNameForAcknowledgedBySessionKey ), m_avlStatus( avlStatus ), m_isUnique( isUnique ),
		  m_isSourceTimeModified( false ), m_isTransactiveTimeModified( false ), m_isAckTimeModified( false ), m_isCloseTimeModified( false ),
		  m_isParamListModified( false ), m_isSeverityKeyModified( false ), m_isTypeModified( false ),
		  m_isContextModified( false ), m_isStateModified( false ), m_isMmsStateTypeModified( false ), m_isAcknowledgedByModified( false ),
		  m_isEntityKeyModified( false ), m_isCommentModified( false ), m_isAssetNameModified( false ), m_isPlanStatusModified( false ), m_avlHeadId(avlHeadId), m_isAVLStatusModified( false ), m_isIsUniqueModified( false )
	{	
		if ( m_acknowledgedBy.empty() )
		{
            m_operatorNameForAcknowledgedBySessionKey = "";
		}
	}


    std::string AlarmData::getKey()
    {
        TA_ASSERT(std::string("") != m_key, "A newly created alarm must be written to the database before the key can be retrieved");

        return(m_key);
    }


    void AlarmData::setKey(const std::string& key)
    {
        TA_ASSERT(true == m_isNew,"Cannot set the key for an existing Alarm");
        m_key = key;
    }


	/*
    std::string AlarmData::getDescription()
    {
        if (m_isValidData || std::string("") == m_key)
        {
                TA_ASSERT(std::string("") != m_description,"The description must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_description;
    }
	*/


	/*
    void AlarmData::setDescription(const std::string& description)
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_description = description;
		m_isDescriptionModified = true;
    }
	*/


    std::string AlarmData::getParamList()
    {
        if ( ! m_isValidData && std::string("") != m_key )
        {
            reload();
        }
        return m_paramString;
    }


    void AlarmData::setParamList(const std::string& paramString)
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_paramString = paramString;
		m_isParamListModified = true;
    }


    timeb AlarmData::getSourceTime()
    {
        if (m_isValidData || std::string("") == m_key)
        {
            TA_ASSERT(0 != m_sourceTime.time,"The create time must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_sourceTime;
    }


    void AlarmData::setSourceTime(const timeb sourceTime)
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_sourceTime = sourceTime;
		m_isSourceTimeModified = true;
    }


    timeb AlarmData::getTransactiveTime()
    {
        if (m_isValidData || std::string("") == m_key)
        {
            TA_ASSERT(0 != m_transactiveTime.time,"The log time must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_transactiveTime;
    }


    void AlarmData::setTransactiveTime(const timeb transactiveTime)
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_transactiveTime = transactiveTime;
		m_isTransactiveTimeModified = true;
    }
	
	
	time_t AlarmData::getAckTime()
    {
        if ( ! m_isValidData && std::string("") != m_key )
        {
            reload();
        }
        return m_ackTime;
    }


    void AlarmData::setAckTime(const time_t ackTime)
    {
		if ( ! m_isValidData && !( std::string("") == m_key ) )
        {
            reload();
        }
        m_ackTime = ackTime;
		m_isAckTimeModified = true;
    }


	time_t AlarmData::getCloseTime()
    {
        if ( ! m_isValidData && std::string("") != m_key )
        {
            reload();
        }
        return m_closeTime;
    }


    void AlarmData::setCloseTime(const time_t closeTime)
    {
        if ( ! m_isValidData && !( "" == m_key ) )
        {
            reload();
        }
        m_closeTime = closeTime;
		m_isCloseTimeModified = true;
    }


    unsigned long AlarmData::getSeverity()
    {
        if (m_isValidData || std::string("") == m_key)
        {
            TA_ASSERT(ULONG_MAX != m_severityKey,"The severity key must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_severityKey;
    }

    void AlarmData::setSeverity(const unsigned long severity)
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_severityKey = severity;
		m_isSeverityKeyModified = true;
    }


    unsigned long AlarmData::getTypeKey()
    {
        if (m_isValidData || std::string("") == m_key)
        {
                TA_ASSERT(0 != m_typeKey,"The type must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_typeKey;
    }


    std::string AlarmData::getContext()
    {
        if (m_isValidData || std::string("") == m_key)
        {
                TA_ASSERT(std::string("") != m_context,"The context must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_context;
    }


    void AlarmData::setTypeKey(const unsigned long typeKey)
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_typeKey = typeKey;
		m_isTypeModified = true;
    }


    void AlarmData::setContext(const std::string& context)
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_context = context;
		m_isContextModified = true;
    }


    IAlarmData::EAlarmState AlarmData::getState()
    {
        if (m_isValidData || std::string("") == m_key)
        {
            TA_ASSERT(UINT_MAX != m_state,"The state must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_state;
    }


    void AlarmData::setState(IAlarmData::EAlarmState state)
    {
        TA_ASSERT(3 > state,"State must be between 0 and 2");
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_state = state;
		m_isStateModified = true;
    }


    IAlarmData::EMmsState AlarmData::getMmsStateType()
    {
        if (m_isValidData || std::string("") == m_key)
        {
            TA_ASSERT(UINT_MAX != m_mmsStateType,"The mms state type must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_mmsStateType;
    }


    void AlarmData::setMmsStateType(IAlarmData::EMmsState mmsStateType)
    {
        TA_ASSERT(6 >= mmsStateType,"Mms State Type must be between 0 and 6");
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_mmsStateType = mmsStateType;
		m_isMmsStateTypeModified = true;
    }


	std::string AlarmData::getAcknowledgedBy()
    {
        if ( ! m_isValidData && std::string("") != m_key )
        {
            reload();
        }
        return m_acknowledgedBy;
    }


    void AlarmData::setAcknowledgedBy( const std::string& sessionID )
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_acknowledgedBy = sessionID;
		m_isAcknowledgedByModified = true;
    }

	std::string AlarmData::getOperatorName()
	{
		if ( ! m_isValidData && std::string("") != m_key )
        {
            reload();
        }
        return m_operatorNameForAcknowledgedBySessionKey;
	}
	
	void AlarmData::setOperatorName( const std::string& operatorName )
	{
		if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
		m_operatorNameForAcknowledgedBySessionKey=operatorName;
		//m_isAcknowledgedByModified = true;
	}



    unsigned long AlarmData::getEntityKey()
    {
        if (m_isValidData || std::string("") == m_key)
        {
            TA_ASSERT(ULONG_MAX != m_entityKey,"The entity key must be set before it can be retrieved");
        }
        else
        {
            reload();
        }
        return m_entityKey;
    }


    void AlarmData::setEntityKey(const unsigned long key)
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_entityKey = key;
		m_isEntityKeyModified = true;
    }


    std::string AlarmData::getComments()
    {
        if ( ! m_isValidData && std::string("") != m_key )
        {
            reload();
        }
        return m_comment;
    }


    void AlarmData::addComment(const std::string& comment)
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_comment = comment;
		m_isCommentModified = true;
    }

    
    std::string AlarmData::getAssetName()
    {
        if ( ! m_isValidData && std::string("") != m_key )
        {
            reload();
        }
        return m_assetName;
    }


    void AlarmData::setAssetName(const std::string& assetName)
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_assetName = assetName;
		m_isAssetNameModified = true;
    }


    std::string AlarmData::getPlanStatus()
    {
        if ( ! m_isValidData && std::string("") != m_key )
        {
            reload();
        }
        return m_planStatus;
    }


    void AlarmData::setPlanStatus(const std::string& planStatus)
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_planStatus = planStatus;
		m_isPlanStatusModified = true;
    }


    unsigned long AlarmData::getSubsystemKeyOfAssociatedEntity()
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        return m_subsystemKeyOfAssociatedEntity;
    }

    unsigned long AlarmData::getLocationKeyOfAssociatedEntity()
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        return m_locationKeyOfAssociatedEntity;
    }

	/*
    std::string AlarmData::getNameOfAssociatedEntity()
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        return m_nameOfAssociatedEntity;
    }
	*/

    unsigned long AlarmData::getTypeOfAssociatedEntity()
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        return m_typeOfAssociatedEntity;
    }

    std::string AlarmData::getOperatorNameForAcknowledgedBySessionKey()
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        return m_operatorNameForAcknowledgedBySessionKey;
    }

    std::string AlarmData::getAVLHeadId()
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        return m_avlHeadId;
    }

    void AlarmData::setAVLHeadId(const std::string& avlHeadId)
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_avlHeadId = avlHeadId;
		m_isAVLHeadIdModified = true;
    }

	IAlarmData::EAlarmAVLStatus AlarmData::getAVLStatus()
	{
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        return m_avlStatus;
	}

	void AlarmData::setAVLStatus(const IAlarmData::EAlarmAVLStatus avlStatus)
	{
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_avlStatus = avlStatus;
		m_isAVLStatusModified = true;
	}

	bool AlarmData::getIsUnique()
    {
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        return m_isUnique;
    }

	void AlarmData::setIsUnique(const bool isUnique)
	{
        if (!m_isValidData && !(std::string("") == m_key))
        {
            reload();
        }
        m_isUnique = isUnique;
		m_isIsUniqueModified = true;
	}

    void AlarmData::invalidate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "AlarmData::invalidate" );
        // This method cannot be called until the key has been set
        TA_ASSERT(std::string("") != m_key,"Attempted to call invalidate() on a new alarm");

        m_isValidData = false;

		m_isSourceTimeModified = true;
		m_isTransactiveTimeModified = true;
		m_isAckTimeModified = true;
		m_isCloseTimeModified = true;
		//m_isDescriptionModified = true;
		m_isParamListModified = true;
		m_isSeverityKeyModified = true;
		m_isTypeModified = true;
		m_isContextModified = true;
		m_isStateModified = true;
		m_isMmsStateTypeModified = true;
		m_isAcknowledgedByModified = true;
		m_isEntityKeyModified = true;
		m_isCommentModified = true;
		m_isAssetNameModified = true;
		m_isPlanStatusModified = true;
		m_isAVLStatusModified = true;
		m_isAVLHeadIdModified = true;
		m_isIsUniqueModified = true;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "AlarmData::invalidate" );
    }

    std::string AlarmData::getLocalDatabaseName()
    {
        if (m_localDatabase.empty())
        {
            // have to find out what is the local db name
            // this should be the first in the connection list so grab it
            if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
            {
                throw DatabaseException("db-connection-file not set");
            }
            try
            {
				std::string strDbConnFile(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				m_localDatabase = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strDbConnFile, Alarm_Sd, Write);

                /*DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
                DataConnections connections = connectionStrings.getConnectionList(Alarm_Sd, Write);
                m_localDatabase = connections[0].first;  */
            }
            catch(...) // most likely a file not found error
            {
                throw DatabaseException("Unable to find database connection infomation");
            }

        }
        return m_localDatabase;
    }

    void AlarmData::reload()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "AlarmData::reload" );
            TA_ASSERT(std::string("") != m_key, "This alarm has not yet been written to the database, therefore it cannot be reloaded");
        
		// get a connection to the database
        IDatabase* databaseConnection =
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Read);
//		std::string sql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARM_Oracle_SELECT_3553, m_key);
//		std::string sql  = databaseConnection->prepareSQLStatement(ALARM_SELECT_3553, m_key);
		SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, ALARM_SELECT_3553, m_key);
//		TA_ASSERT(sql.size() > 0, "AlarmData::reload(): Prepare SQL Statement error");

		// Wenguang++ Alarm Message Size Reduction
        /*std::string sql = "select TO_CHAR(a.SOURCETIME,'YYYYMMDDHH24MISSFF3'),";
        sql += "TO_CHAR(a.TRANSACTIVETIME,'YYYYMMDDHH24MISSFF3'),a.PARAM_LIST,a.SEVERITY,a.TYPE,a.CONTEXT,a.STATE, a.MMS_STATE_TYPE,";
        sql += "a.ACKNOWLEDGED_BY, a.ENTITYKEY, a.COMMENTS, a.ASSET_NAME, a.PLANSTATUS, a.AVL_HEAD_ID, a.AVL_STATUS, a.IS_UNIQUE, ";
        sql += "TO_CHAR(a.ACK_TIME,'YYYYMMDDHH24MISS'), TO_CHAR(a.CLOSE_TIME,'YYYYMMDDHH24MISS'), e.SUBSYSTEMKEY, e.LOCATIONKEY, e.TYPEKEY, a.OPERATOR_NAME ";
        sql += "from alarm a, ENTITY e ";  //e, TA_SESSION s, operator o
        sql += "where alarm_id = '" + m_key + "' ";
        sql += "AND e.PKEY = a.ENTITYKEY ";
        //sql += "AND o.pkey (+) = s.operatorkey ";
        //sql += "AND s.PKEY (+) = a.ACKNOWLEDGED_BY"; 
		*/

        // Set up the columnNames vector to be passed to executeQuery()
        std::string sourceTimeColumn = "a.SOURCETIME";
        std::string transactiveTimeColumn = "a.TRANSACTIVETIME";
        std::string paramColumn = "a.PARAMLIST";
        std::string severityColumn = "a.SEVERITY";
        std::string typeColumn = "a.TYPE";
        std::string contextColumn = "a.CONTEXT";
        std::string stateColumn = "a.STATE";
		std::string mmsStateTypeColumn = "a.MMS_STATE_TYPE";
        std::string acknowledgedColumn = "a.ACKNOWLEDGED_BY";
        std::string entityKeyColumn = "a.ENTITYKEY";
        std::string commentsColumn = "a.COMMENTS";
        std::string assetNameColumn = "a.ASSET_NAME";
        std::string planStatusColumn = "a.PLANSTATUS";
		std::string avlHeadIdColumn = "a.AVL_HEAD_ID";
		std::string avlStatusColumn = "a.AVL_STATUS";
		std::string isUniqueColumn = "a.IS_UNIQUE";
		std::string ackTimeColumn = "a.ACK_TIME";
		std::string closeTimeColumn = "a.CLOSE_TIME";
        std::string subsystemKeyOfAssociatedEntityColumn = "e.SUBSYSTEMKEY";
        std::string locationKeyOfAssociatedEntityColumn = "e.LOCATIONKEY";
        std::string typeOfAssociatedEntityColumn = "e.TYPEKEY";
        std::string operatorNameForAcknowledgedBySessionKeyColumn = "a.OPERATOR_NAME";
        std::vector<std::string> columnNames;
        columnNames.push_back(sourceTimeColumn);
        columnNames.push_back(transactiveTimeColumn);
        columnNames.push_back(paramColumn);
        columnNames.push_back(severityColumn);
        columnNames.push_back(typeColumn);
        columnNames.push_back(contextColumn);
        columnNames.push_back(stateColumn);
        columnNames.push_back(mmsStateTypeColumn);
        columnNames.push_back(acknowledgedColumn);
        columnNames.push_back(entityKeyColumn);
        columnNames.push_back(commentsColumn);
        columnNames.push_back(assetNameColumn);
        columnNames.push_back(planStatusColumn);
		columnNames.push_back(avlHeadIdColumn);
        columnNames.push_back(avlStatusColumn);
        columnNames.push_back(isUniqueColumn);
        columnNames.push_back(ackTimeColumn);
        columnNames.push_back(closeTimeColumn);
        columnNames.push_back(subsystemKeyOfAssociatedEntityColumn);
        columnNames.push_back(locationKeyOfAssociatedEntityColumn);
        columnNames.push_back(typeOfAssociatedEntityColumn);
        columnNames.push_back(operatorNameForAcknowledgedBySessionKeyColumn);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (NULL == data || 0 == data->getNumRows()) // No entry found with the specified alarm_id
        {
            delete data;
            data = NULL;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No alarm found for alarm_id = %s",m_key.c_str());
            throw DataException(reasonMessage,DataException::NO_VALUE,"");
        }
        else if (1 < data->getNumRows()) // More than one entry found for the alarm_id
        {
            delete data;
            data = NULL;
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one alarm found for alarm_id = %s",m_key.c_str());
            throw DataException(reasonMessage,DataException::NOT_UNIQUE,"");
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_sourceTime = data->getTimestampData(0,sourceTimeColumn);
        m_transactiveTime = data->getTimestampData(0,transactiveTimeColumn);
        m_paramString = data->getStringData(0,paramColumn);
        m_severityKey = data->getUnsignedLongData(0,severityColumn);
        m_typeKey = data->getUnsignedLongData(0,typeColumn);
        m_context = data->getStringData(0, contextColumn);
        m_state = static_cast<IAlarmData::EAlarmState>(data->getIntegerData(0,stateColumn));
		m_mmsStateType = static_cast<IAlarmData::EMmsState>(data->getIntegerData(0,mmsStateTypeColumn));
		m_acknowledgedBy = data->getStringData( 0, acknowledgedColumn );
		m_entityKey = data->getUnsignedLongData(0,entityKeyColumn);
        m_comment = data->getStringData(0,commentsColumn);
		m_assetName = data->getStringData(0,assetNameColumn);
        m_planStatus = data->getStringData(0,planStatusColumn);
		if ( m_planStatus.empty() )
		{
			m_planStatus = NO_ASSOC_PLAN;
		}
        m_avlHeadId = data->getStringData(0,avlHeadIdColumn);

		m_avlStatus = static_cast<IAlarmData::EAlarmAVLStatus>(data->getIntegerData(0,avlStatusColumn));
		m_isUnique = data->getBooleanData(0,isUniqueColumn);

		m_ackTime = data->getDateData( 0, ackTimeColumn );
		m_closeTime = data->getDateData( 0, closeTimeColumn );

        m_subsystemKeyOfAssociatedEntity = data->getUnsignedLongData(0, subsystemKeyOfAssociatedEntityColumn);
        m_locationKeyOfAssociatedEntity = data->getUnsignedLongData(0, locationKeyOfAssociatedEntityColumn);
        m_typeOfAssociatedEntity = data->getUnsignedLongData(0, typeOfAssociatedEntityColumn);
        m_operatorNameForAcknowledgedBySessionKey = data->getStringData(0, operatorNameForAcknowledgedBySessionKeyColumn);
        
        // Now that we're done with the IData object, it's our responsibility to delete it
        delete data;
        data = NULL;

        // Need to record that we now have valid data
        m_isValidData = true;

		m_isSourceTimeModified = false;
		m_isTransactiveTimeModified = false;
		m_isAckTimeModified = false;
		m_isCloseTimeModified = false;
		// m_isDescriptionModified = false;
		m_isParamListModified = false;
		m_isSeverityKeyModified = false;
		m_isTypeModified = false;
		m_isContextModified = false;
		m_isStateModified = false;
		m_isMmsStateTypeModified = false;
		m_isAcknowledgedByModified = false;
		m_isEntityKeyModified = false;
		m_isCommentModified = false;
		m_isAssetNameModified = false;
		m_isPlanStatusModified = false;
		m_isAVLStatusModified = false;
		m_isAVLHeadIdModified = false;
		m_isIsUniqueModified = false;

        LOG ( SourceInfo, DebugUtil::FunctionExit, "AlarmData::reload" );
    }

    void AlarmData::applyChanges()
    {
        applyChanges(m_key);
    }

    void AlarmData::applyChanges(const std::string& key)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "AlarmData::applyChanges" );
        // Set up the boolean variables that specify if something is wrong.
		// Wenguang++ Alarm Message Size Reduction
        // bool descriptionValid = std::string("") != m_description;
        bool paramStringValid = true; //std::string("") != m_paramString;
        bool sourceTimeValid = ULONG_MAX != m_sourceTime.time;
        bool transactiveTimeValid = ULONG_MAX != m_transactiveTime.time;
        bool ackTimeValid = ULONG_MAX != m_ackTime;
        bool closeTimeValid = ULONG_MAX != m_closeTime;
        bool severityKeyValid = ULONG_MAX != m_severityKey;
        bool stateValid = UINT_MAX != m_state;
		bool mmsStateValid = UINT_MAX != m_mmsStateType;
		bool avlStatusValid = UINT_MAX != m_avlStatus;
        bool typeValid = (0 != m_typeKey);
        bool keyValid = std::string("") != key;
        // don't need to check type or key - the key will never be "invalid", while type is
        // hardcoded, and not read from the database.

        // Now need to determine which of the above tests failed, and add each failure
        // to a vector of failed field names. Unfortunately, this leads to a lot of if
        // statements, but there's no way around it.
        std::vector<std::string> fieldNames;

		// Wenguang++ Alarm Message Reduction
		/*
        if (!descriptionValid)
        {
            fieldNames.push_back("DESCRIPTION");
        }
		*/
        if (!paramStringValid)
        {
            fieldNames.push_back("PARAMLIST");
        }
        if (!sourceTimeValid)
        {
            fieldNames.push_back("SOURCETIME");
        }
        if (!ackTimeValid)
        {
            fieldNames.push_back("ACK_TIME");
        }
        if (!closeTimeValid)
        {
            fieldNames.push_back("CLOSE_TIME");
        }
        if (!transactiveTimeValid)
        {
            fieldNames.push_back("TRANSACTIVETIME");
        }
        if (!severityKeyValid)
        {
            fieldNames.push_back("SEVERITYKEY");
        }
        if (!stateValid)
        {
            fieldNames.push_back("STATE");
        }
        if (!mmsStateValid)
        {
            fieldNames.push_back("MMS_STATE_TYPE");
        }
		if (!avlStatusValid)
		{
            fieldNames.push_back("AVL_STATUS");
		}
        if (!typeValid)
        {
            fieldNames.push_back("TYPE");
        }
        if (!keyValid)
        {
            fieldNames.push_back("KEY");
        }
        // Now, if we have at least one entry in the vector, something is not right.
        if (0 < fieldNames.size())
        {
                TA_THROW( TA_Base_Core::DataConfigurationException("Alarm data not fully specified. Entity cannot be written to database"
                                                                ,fieldNames));
        }
        // Write the alarm to the database

        // get a connection to the database
        IDatabase* databaseConnection =
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Write);

        // Need to check if this is a new alarm or an existing alarm -
        // as a new alarm will be inserted, while an existing alarm will be updated
        if (m_isNew) // This is a new entity
        {
            m_key = key;
            createNewAlarm();
        }
        else // This is an existing entity
        {
            // Ensure this method has been called with the correct key (cannot change
            // an existing key)
            TA_ASSERT(key == m_key,"Cannot set the key for an existing Alarm");
            updateExistingAlarm();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "AlarmData::applyChanges" );
    }

    void AlarmData::createNewAlarm()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "AlarmData::createNewAlarm");

        // Now we can get on with writing the entity to the database.
        // Convert non-string elements to char*'s
		char entityKeyString[50] = {0};
		char severityKeyString[50] = {0};
		char typeKeyString[50] = {0};
        char stateString[2] = {0};
        char mmsStateString[2] = {0};
		char avlStatusString[2] = {0};
		std::string isUniqueString;
		
		sprintf(entityKeyString, "%lu", m_entityKey);
		sprintf(severityKeyString, "%lu", m_severityKey);
		sprintf(typeKeyString, "%lu", m_typeKey);
        sprintf(stateString, "%d", m_state);
        sprintf(mmsStateString, "%d", m_mmsStateType);
		sprintf(avlStatusString, "%d", m_avlStatus);
		m_isUnique? isUniqueString="1": isUniqueString="0";

        // Convert time data into formatted strings
        struct tm sourceTimeStruct;
        // Note for code review: Which one of these should we use? Currently getting
        // the local time. Could get GMT instead.
        //struct tm t;
        //ACE_OS::gmtime_r(&currentTime, &t);

        // extract the time from the struct and make it into a string
        char sourceTimeString[24] = {0};

		LOGBEFORECOTS("ACE_OS::localtime_r" );
	 
		
        if (ACE_OS::localtime_r(&m_sourceTime.time, &sourceTimeStruct) != NULL)
        {
	        sprintf(sourceTimeString, "%02d/%02d/%04d %02d:%02d:%02d.%03d", sourceTimeStruct.tm_mday, sourceTimeStruct.tm_mon + 1,
		        sourceTimeStruct.tm_year + 1900, sourceTimeStruct.tm_hour, sourceTimeStruct.tm_min, sourceTimeStruct.tm_sec,
                m_sourceTime.millitm);
        }
        LOGAFTERCOTS("ACE_OS::localtime_r");
        // do it again with the transactiveTime
        struct tm transactiveTimeStruct;
        char transactiveTimeString[24] = {0};
        if (ACE_OS::localtime_r(&m_transactiveTime.time, &transactiveTimeStruct) != NULL)
        {

	        sprintf(transactiveTimeString, "%02d/%02d/%04d %02d:%02d:%02d.%03d", transactiveTimeStruct.tm_mday, transactiveTimeStruct.tm_mon + 1,
		        transactiveTimeStruct.tm_year + 1900, transactiveTimeStruct.tm_hour, transactiveTimeStruct.tm_min, transactiveTimeStruct.tm_sec, 
                m_transactiveTime.millitm);
        }

		struct tm ackTimeStruct;
		char ackTimeString[24] = {0};
        if (ACE_OS::localtime_r(&m_ackTime, &ackTimeStruct) != NULL)
        {
	        sprintf(ackTimeString, "%02d/%02d/%04d %02d:%02d:%02d",
				ackTimeStruct.tm_mday,
				ackTimeStruct.tm_mon + 1,
		        ackTimeStruct.tm_year + 1900,
				ackTimeStruct.tm_hour,
				ackTimeStruct.tm_min,
				ackTimeStruct.tm_sec);
        }

		struct tm closeTimeStruct;
		char closeTimeString[24] = {0};
        if (ACE_OS::localtime_r(&m_closeTime, &closeTimeStruct) != NULL)
        {
	        sprintf(closeTimeString, "%02d/%02d/%04d %02d:%02d:%02d",
				closeTimeStruct.tm_mday,
				closeTimeStruct.tm_mon + 1,
		        closeTimeStruct.tm_year + 1900,
				closeTimeStruct.tm_hour,
				closeTimeStruct.tm_min,
				closeTimeStruct.tm_sec);
        }

        // Get the database connection
        IDatabase* databaseConnection =
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Write);

        // generate the SQL string to write the entity (reuse the string)
        /*std::string formatSQL = "BEGIN audit_data_queue_pkg.enqueue_audit_data('";
		formatSQL += getLocalDatabaseName();
		formatSQL += "','GROUP','";
		formatSQL += "insert into ALARM (alarm_id,SOURCETIME,TRANSACTIVETIME,";
        formatSQL += "PARAM_LIST,SEVERITY,TYPE,CONTEXT,STATE,MMS_STATE_TYPE,ACKNOWLEDGED_BY,OPERATOR_NAME,ENTITYKEY,COMMENTS,ASSET_NAME,PLANSTATUS,AVL_HEAD_ID,AVL_STATUS,IS_UNIQUE,ACK_TIME,CLOSE_TIME) ";
        formatSQL += "values (''" + m_key + "'',TO_TIMESTAMP(''";
        formatSQL += std::string(sourceTimeString) + "'',''dd/mm/yyyy hh24:mi:ss.ff3''),";
        formatSQL += "TO_TIMESTAMP(''" + std::string(transactiveTimeString) + "'',''dd/mm/yyyy hh24:mi:ss.ff3''),''";
        formatSQL += databaseConnection->escapeAQSQLString(m_paramString) + "'',";
        formatSQL += std::string(severityKeyString) + ",";
		formatSQL += std::string(typeKeyString) + ", ''";
		formatSQL += databaseConnection->escapeAQSQLString(m_context) + "'',";
        formatSQL += std::string(stateString) + ",";
        formatSQL += std::string(mmsStateString) + ",''";
        formatSQL += databaseConnection->escapeAQSQLString( m_acknowledgedBy ) + "'',''";
		//m_operatorNameForAcknowledgedBySessionKey
		formatSQL += databaseConnection->escapeAQSQLString( m_operatorNameForAcknowledgedBySessionKey ) + "'',";
        formatSQL += std::string(entityKeyString) + ",''";
		formatSQL += databaseConnection->escapeAQSQLString(m_comment) + "'', ''";
		formatSQL += databaseConnection->escapeAQSQLString(m_assetName) + "'', ''";
		formatSQL += databaseConnection->escapeAQSQLString(m_planStatus) + "'', ''";
		formatSQL += databaseConnection->escapeAQSQLString(m_avlHeadId) + "'', ";
		formatSQL += std::string(avlStatusString) + ", ";
		formatSQL += std::string(isUniqueString) + ", ";
        formatSQL += "TO_DATE(''" + std::string(ackTimeString) + "'',''dd/mm/yyyy hh24:mi:ss''), ";
		formatSQL += "TO_DATE(''" + std::string(closeTimeString) +  "'',''dd/mm/yyyy hh24:mi:ss'') )'); END;";*/
//		std::string formatSQL = defPrepareSQLStatement(databaseConnection->GetDbServerType(), ALARM_Oracle_INSERT_3851,
//		std::string formatSQL  = databaseConnection->prepareSQLStatement(ALARM_INSERT_3851,
		SQLStatement formatSQL;
		databaseConnection->prepareSQLStatement(formatSQL, ALARM_INSERT_3851,
			getLocalDatabaseName(), m_key, std::string(sourceTimeString), std::string(transactiveTimeString),
			databaseConnection->escapeAQSQLString(m_paramString), std::string(severityKeyString), std::string(typeKeyString),
			databaseConnection->escapeAQSQLString(m_context), std::string(stateString), std::string(mmsStateString),
			databaseConnection->escapeAQSQLString( m_acknowledgedBy ), std::string(entityKeyString), 
			databaseConnection->escapeAQSQLString(m_comment), databaseConnection->escapeAQSQLString(m_assetName),
			databaseConnection->escapeAQSQLString(m_planStatus), databaseConnection->escapeAQSQLString(m_avlHeadId),
			std::string(avlStatusString), std::string(isUniqueString), std::string(ackTimeString), std::string(closeTimeString));
		//TA_ASSERT(formatSQL.size() > 0, "createNewAlarm: Get SQL Statement error");

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugTrace,
			 "severity key is %lu, severity key string is %s",
			 m_severityKey, severityKeyString);

        databaseConnection->executeModification(formatSQL);

		m_isSourceTimeModified = false;
		m_isTransactiveTimeModified = false;
		m_isAckTimeModified = false;
		m_isCloseTimeModified = false;
		m_isParamListModified = false;
		m_isSeverityKeyModified = false;
		m_isTypeModified = false;
		m_isContextModified = false;
		m_isStateModified = false;
		m_isMmsStateTypeModified = false;
		m_isAcknowledgedByModified = false;
		m_isEntityKeyModified = false;
		m_isCommentModified = false;
		m_isAssetNameModified = false;
		m_isPlanStatusModified = false;
		m_isAVLStatusModified = false;
		m_isAVLHeadIdModified = false;
		m_isIsUniqueModified = false;
		
		m_isNew = false;
        LOG ( SourceInfo, DebugUtil::FunctionExit, "AlarmData::createNewAlarm");
    }

    void AlarmData::updateExistingAlarm()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "AlarmData::updateExistingAlarm");

		// TD #11720
		// make sure at least one field has actually been modified before we make the SQL query
		if( m_isSourceTimeModified || m_isTransactiveTimeModified || m_isAckTimeModified || m_isCloseTimeModified || 
		    m_isParamListModified || m_isSeverityKeyModified || m_isTypeModified ||
			m_isContextModified || m_isStateModified || m_isMmsStateTypeModified || m_isAcknowledgedByModified ||
			m_isEntityKeyModified || m_isCommentModified || m_isAssetNameModified || m_isPlanStatusModified || m_isAVLStatusModified || m_isAVLHeadIdModified || m_isIsUniqueModified )
		{
			// Get the database connection
			IDatabase* databaseConnection =
					TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarm_Sd, Write);
			//int nDBType = databaseConnection->getDbServerType();

			bool firstField = true;
			std::ostringstream formatSQL;
            /*formatSQL << "BEGIN ";
            formatSQL << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','GROUP', '";
			formatSQL << "update ALARM set ";*/

			// SOURCETIME
			//int nDBType = enumBeginDbType;
			SQLStatement strSql;
			SQLTypeAdapter sqlParams;
			/*while (++nDBType != enumEndDbType)
			{*/
				formatSQL.str("");
				firstField = true;
				if(m_isSourceTimeModified)
				{
				// Convert time data into formatted strings
				struct tm sourceTimeStruct;
				// Note for code review: Which one of these should we use? Currently getting
				// the local time. Could get GMT instead.
				//struct tm t;
				//ACE_OS::gmtime_r(&currentTime, &t);

				// extract the time from the struct and make it into a string
				char sourceTimeString[24] = {0};
				if (ACE_OS::localtime_r(&m_sourceTime.time, &sourceTimeStruct) != NULL)
				{
					sprintf(sourceTimeString, "%02d/%02d/%04d %02d:%02d:%02d.%03d", sourceTimeStruct.tm_mday, sourceTimeStruct.tm_mon + 1,
						sourceTimeStruct.tm_year + 1900, sourceTimeStruct.tm_hour, sourceTimeStruct.tm_min, sourceTimeStruct.tm_sec,
						m_sourceTime.millitm);
				}
			/*if (enumOracleDb == nDBType)
				formatSQL << "SOURCETIME = TO_TIMESTAMP(''" << std::string(sourceTimeString) << "'',''dd/mm/yyyy hh24:mi:ss.ff3'')";				
			else
				formatSQL << "SOURCETIME = STR_TO_DATE(''" << std::string(sourceTimeString) << "'',''%d/%m/%Y %H:%i:%S.%f'')";*/

			sqlParams.add(std::string("SOURCETIME"), std::string(sourceTimeString), enumDateFormat_1, enumFuncFormat_1, enumOperKey_EQUAL, true);
				firstField = false;		
			}

			// TRANSACTIVETIME
			if(m_isTransactiveTimeModified)
			{
				struct tm transactiveTimeStruct;
				char transactiveTimeString[24] = {0};
				if (ACE_OS::localtime_r(&m_transactiveTime.time, &transactiveTimeStruct) != NULL)
				{

					sprintf(transactiveTimeString, "%02d/%02d/%04d %02d:%02d:%02d.%03d", transactiveTimeStruct.tm_mday, transactiveTimeStruct.tm_mon + 1,
						transactiveTimeStruct.tm_year + 1900, transactiveTimeStruct.tm_hour, transactiveTimeStruct.tm_min, transactiveTimeStruct.tm_sec,
						m_transactiveTime.millitm);
				}
				/*if (enumOracleDb == nDBType)
					formatSQL << (firstField ? "" : ", ") << "TRANSACTIVETIME = TO_TIMESTAMP(''" << std::string(transactiveTimeString) << "'',''dd/mm/yyyy hh24:mi:ss.ff3'')";
				else
					formatSQL << (firstField ? "" : ", ") << "TRANSACTIVETIME = STR_TO_DATE(''" << std::string(transactiveTimeString) << "'',''%d/%m/%Y %H:%i:%S.%f'')";*/

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("TRANSACTIVETIME"), std::string(transactiveTimeString), enumDateFormat_1, enumFuncFormat_1, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// ACK_TIME
			if(m_isAckTimeModified)
			{
				struct tm ackTimeStruct;
				char ackTimeString[24] = {0};
				if (ACE_OS::localtime_r(&m_ackTime, &ackTimeStruct) != NULL)
				{
					sprintf(ackTimeString, "%02d/%02d/%04d %02d:%02d:%02d",
						ackTimeStruct.tm_mday,
						ackTimeStruct.tm_mon + 1,
						ackTimeStruct.tm_year + 1900,
						ackTimeStruct.tm_hour,
						ackTimeStruct.tm_min,
						ackTimeStruct.tm_sec);
				}
				/*if (enumOracleDb == nDBType)
					formatSQL << (firstField ? "" : ", ") << "ACK_TIME = TO_DATE(''" << std::string(ackTimeString) << "'',''dd/mm/yyyy hh24:mi:ss'')";
				else
					formatSQL << (firstField ? "" : ", ") << "ACK_TIME = STR_TO_DATE(''" << std::string(ackTimeString) << "'',''%d/%m/%Y %H:%i:%S.%f'')";*/

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("ACK_TIME"), std::string(ackTimeString), enumDateFormat_2, enumFuncFormat_2, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// CLOSE_TIME
			if(m_isCloseTimeModified)
			{
				struct tm closeTimeStruct;
				char closeTimeString[24] = {0};
				if (ACE_OS::localtime_r(&m_closeTime, &closeTimeStruct) != NULL)
				{
					sprintf(closeTimeString, "%02d/%02d/%04d %02d:%02d:%02d",
						closeTimeStruct.tm_mday,
						closeTimeStruct.tm_mon + 1,
						closeTimeStruct.tm_year + 1900,
						closeTimeStruct.tm_hour,
						closeTimeStruct.tm_min,
						closeTimeStruct.tm_sec);
				}
				/*if (enumOracleDb == nDBType)
					formatSQL << (firstField ? "" : ", ") << "CLOSE_TIME = TO_DATE(''" << std::string(closeTimeString) << "'',''dd/mm/yyyy hh24:mi:ss'')";
				else
					formatSQL << (firstField ? "" : ", ") << "CLOSE_TIME = STR_TO_DATE(''" << std::string(closeTimeString) << "'',''%d/%m/%Y %H:%i:%S.%f'')";*/

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("CLOSE_TIME"), std::string(closeTimeString), enumDateFormat_2, enumFuncFormat_2, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// DESCRIPTION
			/*
			if(m_isDescriptionModified)
			{
				formatSQL << (firstField ? "" : ", ") << "DESCRIPTION = ''" << databaseConnection->escapeAQSQLString(m_description) << "''";
				firstField = false;
			}
			*/

			// PARAM_LIST
			if(m_isParamListModified)
			{
				/*formatSQL << (firstField ? "" : ", ") << "PARAM_LIST = ''" << databaseConnection->escapeAQSQLString(m_paramString) << "''";*/

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("PARAM_LIST"), databaseConnection->escapeAQSQLString(m_paramString), enumValType_STR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// SEVERITY
			if(m_isSeverityKeyModified)
			{
				char severityKeyString[50] = {0};
				sprintf(severityKeyString, "%lu", m_severityKey);

				/*formatSQL << (firstField ? "" : ", ") << "SEVERITY = " << std::string(severityKeyString);*/

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("SEVERITY"), std::string(severityKeyString), enumValType_NONSTR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// TYPE
			if(m_isTypeModified)
			{
				char typeKeyString[50] = {0};
				sprintf(typeKeyString, "%lu", m_typeKey);
				/*formatSQL << (firstField ? "" : ", ") << "TYPE = ''" << std::string(typeKeyString) << "''";*/

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("TYPE"), std::string(typeKeyString), enumValType_STR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// CONTEXT
			if(m_isContextModified)
			{
				/*formatSQL << (firstField ? "" : ", ") << "CONTEXT = ''" << databaseConnection->escapeAQSQLString(m_context) << "''";*/

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("CONTEXT"), databaseConnection->escapeAQSQLString(m_context), enumValType_STR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// STATE
			if(m_isStateModified)
			{
		        char stateString[2] = {0};
		        sprintf(stateString, "%d", m_state);

				/*formatSQL << (firstField ? "" : ", ") << "STATE = " << std::string(stateString);*/

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("STATE"), std::string(stateString), enumValType_NONSTR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// MMS_STATE_TYPE
			if(m_isMmsStateTypeModified)
			{
				char mmsStateString[2] = {0};
				sprintf(mmsStateString, "%d", m_mmsStateType);

				/*formatSQL << (firstField ? "" : ", ") << "MMS_STATE_TYPE = " << std::string(mmsStateString);*/

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("MMS_STATE_TYPE"), std::string(mmsStateString), enumValType_NONSTR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// ACKNOWLEDGED_BY
			if(m_isAcknowledgedByModified)
			{
				/*formatSQL << (firstField ? "" : ", ") << "ACKNOWLEDGED_BY = ''" << databaseConnection->escapeAQSQLString(m_acknowledgedBy) << "''";
				firstField = false;
				formatSQL << (firstField ? "" : ", ") << "OPERATOR_NAME = ''" << databaseConnection->escapeAQSQLString(m_operatorNameForAcknowledgedBySessionKey) << "''";*/			 				

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("ACKNOWLEDGED_BY"), databaseConnection->escapeAQSQLString(m_acknowledgedBy), enumValType_STR, enumOperKey_EQUAL, true);
				firstField = false;
				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("OPERATOR_NAME"), databaseConnection->escapeAQSQLString(m_operatorNameForAcknowledgedBySessionKey), enumValType_STR, enumOperKey_EQUAL, true);
			}

			// ENTITY_KEY
			if(m_isEntityKeyModified)
			{
		        // Convert non-string elements to char*'s
				char entityKeyString[50] = {0};
				sprintf(entityKeyString, "%lu", m_entityKey);

				/*formatSQL << (firstField ? "" : ", ") << "ENTITYKEY = " << std::string(entityKeyString);*/

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("ENTITYKEY"), std::string(entityKeyString), enumValType_NONSTR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// COMMENTS
			if(m_isCommentModified)
			{
				/*formatSQL << (firstField ? "" : ", ") << "COMMENTS = ''" << databaseConnection->escapeAQSQLString(m_comment) << "''";*/

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("COMMENTS"), databaseConnection->escapeAQSQLString(m_comment), enumValType_STR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// ASSET_NAME
			if(m_isAssetNameModified)
			{
				/*formatSQL << (firstField ? "" : ", ") << "ASSET_NAME = ''" << databaseConnection->escapeAQSQLString(m_assetName) << "''";*/

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("ASSET_NAME"), databaseConnection->escapeAQSQLString(m_assetName), enumValType_STR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// PLANSTATUS
			if(m_isPlanStatusModified)
			{
				/*formatSQL << (firstField ? "" : ", ") << "PLANSTATUS = ''" << databaseConnection->escapeAQSQLString(m_planStatus) << "''";*/

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("PLANSTATUS"), databaseConnection->escapeAQSQLString(m_planStatus), enumValType_STR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// AVL_STATUS
			if (m_isAVLStatusModified)
			{
				char avlStatusString[2] = {0};
				sprintf(avlStatusString, "%d", m_avlStatus);

				/*formatSQL << (firstField ? "" : ", ") << "AVL_STATUS = " << std::string(avlStatusString);*/

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("AVL_STATUS"), std::string(avlStatusString), enumValType_NONSTR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// AVL_HEAD_ID
			if(m_isAVLHeadIdModified)
			{
				/*formatSQL << (firstField ? "" : ", ") << "AVL_HEAD_ID = ''" << databaseConnection->escapeAQSQLString(m_avlHeadId) << "''";*/

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("AVL_HEAD_ID"), databaseConnection->escapeAQSQLString(m_avlHeadId), enumValType_STR, enumOperKey_EQUAL, true);
				firstField = false;
			}

			// IS_UNIQUE
			if(m_isIsUniqueModified)
			{
				std::string isUniqueString;
				m_isUnique? isUniqueString="1": isUniqueString="0";

				/*formatSQL << (firstField ? "" : ", ") << "IS_UNIQUE = " << std::string(isUniqueString);*/

				sqlParams.add(std::string((firstField ? "" : ", ")));
				sqlParams.add(std::string("IS_UNIQUE"), std::string(isUniqueString), enumValType_NONSTR, enumOperKey_EQUAL, true);
				firstField = false;
			}



//				strSql = databaseConnection->prepareSQLStatement(ALARM_UPDATE_3701, nDBType, strSql, getLocalDatabaseName(), formatSQL.str(), m_key);
				//dynsql delete by lin
				//databaseConnection->prepareSQLStatement(strSql, ALARM_UPDATE_3701, nDBType, strSql, getLocalDatabaseName(), formatSQL.str(), m_key);
			//}  // end of while
			
			//formatSQL << " where alarm_id = ''" << m_key << "'''); END;";
//			TA_ASSERT(strSql.size() > 0, "updateExistingAlarm: Get SQL Statement error");

			databaseConnection->prepareSQLStatement(strSql, ALARM_UPDATE_3701, getLocalDatabaseName(), sqlParams, m_key);
			databaseConnection->executeModification(strSql);

			m_isSourceTimeModified = false;
			m_isTransactiveTimeModified = false;
			m_isAckTimeModified = false;
			m_isCloseTimeModified = false;
			//m_isDescriptionModified = false;
			m_isParamListModified = false;
			m_isSeverityKeyModified = false;
			m_isTypeModified = false;
			m_isContextModified = false;
			m_isStateModified = false;
			m_isMmsStateTypeModified = false;
			m_isAcknowledgedByModified = false;
			m_isEntityKeyModified = false;
			m_isCommentModified = false;
			m_isAssetNameModified = false;
			m_isPlanStatusModified = false;
			m_isAVLStatusModified = false;
			m_isAVLHeadIdModified = false;
			m_isIsUniqueModified = false;
		}

        // nothing more to be done here
        LOG ( SourceInfo, DebugUtil::FunctionExit, "AlarmData::updateExistingAlarm");
    }

} // closes TA_Base_Core
