/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/ConfiguredDerivedState.h $
  * @author HoaVu
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  *
  * ConfigConfiguredDerivedState provides read-only parameters that are specific
  * to a Derived DataPoint derived state in the database
  */


#ifndef CONFIGURED_DERIVED_STATE_H
#define CONFIGURED_DERIVED_STATE_H

#include <sstream>
#include <string>
#include <vector>

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedOutputAssociation.h"

namespace TA_Base_Core
{
	class ConfiguredDerivedState
	{
	public:

		ConfiguredDerivedState ( unsigned long derivedDataPointKey,
					  int stateValue,
					  const std::string & stateDescription,
					  bool alarmEnabled,
					  int alarmDelay,
					  int alarmSeverity,
					  const std::string & alarmMessage)
		:
		m_derivedDataPointKey ( derivedDataPointKey ),
		m_stateValue ( stateValue ),
		m_stateDescription ( stateDescription ),
		m_stateReturnCondition ( "" ),
		m_alarmEnabled ( alarmEnabled ),
		m_alarmDelay ( alarmDelay ),
		m_alarmSeverity ( alarmSeverity ),
		m_alarmMessage ( alarmMessage ),
		m_alarmMMS ( "" )
        {
			m_outputEntities.clear();
        };


		ConfiguredDerivedState ( unsigned long derivedDataPointKey,
					  int stateValue,
					  const std::string & stateDescription,
					  bool alarmEnabled,
					  int alarmDelay,
					  int alarmSeverity,
					  const std::string & alarmMessage,
					  std::vector < DerivedOutputAssociation * > & outputEntities,
					  const std::string & stateReturnCondition, //TD15516
					  const std::string & alarmMMS )
		:
		m_derivedDataPointKey ( derivedDataPointKey ),
		m_stateValue ( stateValue ),
		m_stateDescription ( stateDescription ),
		m_stateReturnCondition ( stateReturnCondition ),
		m_alarmEnabled ( alarmEnabled ),
		m_alarmDelay ( alarmDelay ),
		m_alarmSeverity ( alarmSeverity ),
		m_alarmMessage ( alarmMessage ),	
		m_alarmMMS ( alarmMMS )
        {
			// take ownership of the associated objects passed in via the list
			m_outputEntities.clear();
			std::vector < DerivedOutputAssociation * >::iterator itr;
			for ( itr = outputEntities.begin();
				  itr != outputEntities.end();
				  itr++ )
			{
				m_outputEntities.push_back ( *itr );
			}
			outputEntities.clear();
        };


		virtual ~ConfiguredDerivedState()
        {
			// clean up output entities associated with this DerivedState
			std::vector < DerivedOutputAssociation * >::iterator itr;
			for ( itr = m_outputEntities.begin();
				  itr != m_outputEntities.end();
				  itr++ )
			{
				delete (*itr);
				*itr = NULL;
			}
			m_outputEntities.clear();
        }


        /**
         * getDerivedDataPointEntityKey
         *
         * Get the primary key of the Derived DataPoint entity
         *
         */
		virtual unsigned long getDerivedDataPointEntityKey() const
		{
			return m_derivedDataPointKey;
		};


        /**
         * getStateValue
         *
         * Get the configured value of the derived state associated with the
		 * Derived DataPoint entity
         *
         */
		virtual int getStateValue() const
		{
			return m_stateValue;
		};


        /**
         * getAlarmDelay
         *
         * Get the alarm delay of the derived state associated with the
		 * Derived DataPoint entity
         *
         */
		virtual int getAlarmDelay() const
		{
			return m_alarmDelay;
		};


        /**
         * getAlarmSeverity
         *
         * Get the alarm severity of the derived state associated with the
		 * Derived DataPoint entity
         *
         */
		virtual int getAlarmSeverity() const
		{
			return m_alarmSeverity;
		};


        /**
         * getStateDescription
         *
         * Get the description of the derived state associated with the
		 * Derived DataPoint entity
         *
         */
		virtual const std::string & getStateDescription() const
		{
			return m_stateDescription;
		};


        /**
         * getAlarmMMS
         *
         * Get the MMS alarm condition associated with this derived state
         *
         */
		virtual const std::string & getAlarmMMS() const
		{
			return m_alarmMMS;
		};


		/*
		 * setAlarmMMS
		 * 
		 * @param newCondition 	the new MMS alarm condition associated with this DerivedState
		 *
		 */
		virtual void setAlarmMMS ( const std::string & newCondition )
		{
			// only proceed if different
			if ( 0 != m_alarmMMS.compare ( newCondition ) )
			{
				// save the new condition
				m_alarmMMS = newCondition;

				//
				// update database
				//

				// get handle to the database
				IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);

				//delete by lin
				// Update existing item
				//std::ostringstream sql;
				//sql << "update SC_DERIVED_STATE set"
				//	<< " ALARM_MMS = '" << newCondition << "'"
				//	<< " where DERIVED_DP_PKEY = " << m_derivedDataPointKey
				//	<< " and STATE_VALUE = " << m_stateValue;

				//databaseConnection->executeModification ( sql.str() );
				
				//waitforcheck delete by lin
				SQLStatement strSql;
				databaseConnection->prepareSQLStatement(strSql, SC_DERIVED_STATE_UPDATE_25652,
					newCondition, m_derivedDataPointKey, m_stateValue);
				databaseConnection->executeModification (strSql);

			}
		}


        /**
         * getStateReturnCondition
         *
         * Get the Return Condition associated with the derived state
         *
         */
		virtual const std::string & getStateReturnCondition() const
		{
			return m_stateReturnCondition;
		};


		/*
		 * setStateReturnCondition()
		 * 
		 * @param newExpression 	the new Return Condition expression associated
		 *							with the DerivedState
		 *
		 */
		virtual void setStateReturnCondition( const std::string & newExpression )
		{
			// only proceed if different
			if ( 0 != m_stateReturnCondition.compare ( newExpression ) )
			{
				// save the new expression
				m_stateReturnCondition = newExpression;

				//
				// update database
				//

				// get handle to the database
				IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);

				//delete by lin
				// Update existing item
				//std::ostringstream sql;
				//sql << "update SC_DERIVED_STATE set"
				//	<< " RETURN_CONDITION = '" << newExpression << "'"
				//	<< " where DERIVED_DP_PKEY = " << m_derivedDataPointKey
				//	<< " and STATE_VALUE = " << m_stateValue;

				//databaseConnection->executeModification ( sql.str() );
				
				//waitforcheck delete by lin
				SQLStatement strSql;
				databaseConnection->prepareSQLStatement(strSql, SC_DERIVED_STATE_UPDATE_25653,
					 newExpression, m_derivedDataPointKey, m_stateValue);
				databaseConnection->executeModification (strSql);

			}
		}


        /**
         * getAlarmMessage
         *
         * Get the alarm message of the derived state associated with the
		 * Derived DataPoint entity
         *
         */
		virtual const std::string & getAlarmMessage() const
		{
			return m_alarmMessage;
		};


        /**
         * getAlarmEnabled
         *
         * Get the indication if the alarm is enabled for the derived state
		 * associated with the Derived DataPoint entity
         *
         */
		virtual bool getAlarmEnabled() const
		{
			return m_alarmEnabled;
		};

		
		/*
		 * setAlarmEnabled()
		 * 
		 * @param newEnabled 	the new enabled setting
		 *
		 */
		virtual void setAlarmEnabled( bool newEnabled )
		{
			// update alarm enabled attribute and database if different
			if ( newEnabled != m_alarmEnabled )
			{
				m_alarmEnabled = newEnabled;

				// get handle to the database
				IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);

				//delete by lin
				// Update existing item
				//std::ostringstream sql;
				//sql << "update SC_DERIVED_STATE set"
				//	<< " ALARM_ENABLED = " << m_alarmEnabled
				//	<< " where DERIVED_DP_PKEY = " << m_derivedDataPointKey
				//	<< " and STATE_VALUE = " << m_stateValue;

				//databaseConnection->executeModification ( sql.str() );
				
				//waitforcheck delete by lin
				SQLStatement strSql;
				databaseConnection->prepareSQLStatement(strSql, SC_DERIVED_STATE_UPDATE_25654,
					 m_alarmEnabled, m_derivedDataPointKey, m_stateValue);
				
				databaseConnection->executeModification (strSql);

			}
		}

		virtual void setAlarmDelay( int newDelay )
		{
			// update alarm delay attribute and database if different
			if ( newDelay != m_alarmDelay )
			{
				m_alarmDelay = newDelay;

				// get handle to the database
				IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);

				//delete by lin
				// Update existing item
				//std::ostringstream sql;
				//sql << "update SC_DERIVED_STATE set"
				//	<< " ALARM_DELAY = " << m_alarmDelay
				//	<< " where DERIVED_DP_PKEY = " << m_derivedDataPointKey
				//	<< " and STATE_VALUE = " << m_stateValue;

				//databaseConnection->executeModification ( sql.str() );
				
				//waitforcheck delete by lin
				SQLStatement strSql;				
				databaseConnection->prepareSQLStatement(strSql, SC_DERIVED_STATE_UPDATE_25655,
					m_alarmDelay, m_derivedDataPointKey, m_stateValue);
				databaseConnection->executeModification (strSql);

			}			
		}

		virtual void setAlarmSeverity( int newSeverity )
		{
			// update alarm severity attribute and database if different
			if ( newSeverity != m_alarmSeverity )
			{
				m_alarmSeverity = newSeverity;

				// get handle to the database
				IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);

				//delete by lin
				// Update existing item
				//std::ostringstream sql;
				//sql << "update SC_DERIVED_STATE set"
				//	<< " ALARM_SEVERITY = " << m_alarmSeverity
				//	<< " where DERIVED_DP_PKEY = " << m_derivedDataPointKey
				//	<< " and STATE_VALUE = " << m_stateValue;

				//databaseConnection->executeModification ( sql.str() );
				
				//waitforcheck delete by lin
				SQLStatement strSql;			
				databaseConnection->prepareSQLStatement(strSql, SC_DERIVED_STATE_UPDATE_25656,
					  m_alarmSeverity, m_derivedDataPointKey, m_stateValue);
				databaseConnection->executeModification (strSql);
			}			
		}

		virtual void setAlarmMessage( const std::string & newMessage )
		{
			// update alarm message attribute and database if different
			if ( newMessage != m_alarmMessage )
			{
				m_alarmMessage = newMessage;

				// get handle to the database
				IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Scada_Cd, Write);

				//delete by lin
				// Update existing item
				//std::ostringstream sql;
				//sql << "update SC_DERIVED_STATE set"
				//	<< " ALARM_MESSAGE = '" << m_alarmMessage
				//	<< "' where DERIVED_DP_PKEY = " << m_derivedDataPointKey
				//	<< " and STATE_VALUE = " << m_stateValue;

				//databaseConnection->executeModification ( sql.str() );
				
				//waitforcheck delete by lin
				SQLStatement strSql;		
				databaseConnection->prepareSQLStatement(strSql, SC_DERIVED_STATE_UPDATE_25657,
					 m_alarmMessage, m_derivedDataPointKey, m_stateValue);
				databaseConnection->executeModification (strSql);
			}			
		}


        /**
         * getAssociatedOutputEntities
         *
         * Get the reference to the output entities associated with this derived state.
		 * Caller of this operation is responsible for the lifecycle of the returned
		 * objects.
         *
		 * @param	outputEntities		The list of the output entities associated
		 *								with this DerivedState.
		 *
         */
		virtual void getAssociatedOutputEntities ( std::vector < DerivedOutputAssociation * > & lisOfOutputEntities )
		{
			lisOfOutputEntities.clear();

			// for each of the output entities associated with this DerivedState
			std::vector < DerivedOutputAssociation * >::iterator itr;
			for ( itr = m_outputEntities.begin();
				  itr != m_outputEntities.end();
				  itr++ )
			{
				// create a copy and insert it to the returned list
				lisOfOutputEntities.push_back ( new DerivedOutputAssociation ( **itr )  );
			}
		};


	protected:

        // Hidden as these are not used
        ConfiguredDerivedState() {};
        ConfiguredDerivedState( ConfiguredDerivedState&) {};
        ConfiguredDerivedState& operator=(const ConfiguredDerivedState&);

		unsigned long m_derivedDataPointKey;
		int m_stateValue;
		std::string m_stateDescription;
		std::string m_stateReturnCondition;
		bool m_alarmEnabled;
		int m_alarmDelay;
		int m_alarmSeverity;
		std::string m_alarmMessage;
		std::string m_alarmMMS;

		std::vector < DerivedOutputAssociation * > m_outputEntities;
	};


}

#endif //CONFIGURED_DERIVED_STATE_H

