/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/SchedulingAgentEntityData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * ISchedulingAgentEntityData provides read-only access to the
  * Scheduling Agent's entity data.
  */

#ifndef SCHEDULING_AGENT_ENTITY_DATA_H
#define SCHEDULING_AGENT_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/ISchedulingAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class SchedulingAgentEntityData :
        public virtual ISchedulingAgentEntityData,
        public virtual EntityData
    {
    private:
		          
        SchedulingAgentEntityData ( const SchedulingAgentEntityData & obj );
		SchedulingAgentEntityData & operator= ( const SchedulingAgentEntityData &);

	public:

        static const std::string SCHEDULING_AGENT_ENTITY_TYPE;

		SchedulingAgentEntityData();
        SchedulingAgentEntityData ( unsigned long key );

        virtual ~SchedulingAgentEntityData();

		//
		// operations required by IEntityData
		//
   
        /**
         * clone
         *
         * Returns a pointer to a new SchedulingAgentEntityData object that has
         * been initialised with the specified key.
         *
         * @param key The key with which to construct the object.
         */
        static IEntityData* clone ( unsigned long key);

        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid.
         * The next call to get...() following a call to invalidate()
         * will cause all the data to be reloaded from the database.
         */
        void invalidate();

        /**
         * getType
         *
         * Returns the entity type of this entity (eg "Camera").
         *
         * @return The entity type of this entity as a std::string
         */
        std::string getType();
        static std::string getStaticType();

		//
		// operations required by the ISchedulingAgentEntityData
		//

        /**
         *  getOperatorID
         *
         *  Returns the operator ID used by the Scheduling Agent.
         *
         *  @exception DatabaseException
         *  If there is a database error.
         *
         *  @exception DataException
         *  If a parameter value cannot be converted to the required type
         */
        virtual unsigned long getOperatorID();

        /**
         *  getPassword
         *
         *  Returns the password used by the Scheduling Agent.
         *
         *  @exception DatabaseException
         *  If there is a database error.
         *
         *  @exception DataException
         *  If a parameter value cannot be converted to the required type
         */
        virtual std::string getPassword();
        
        /**
         *  getPlanAgentLocationKey
         *
         *  Returns the location ID of the Plan Agent that the
         *  Scheduling Agent communicates with.
         *
         *  @exception DatabaseException
         *  If there is a database error.
         *
         *  @exception DataException
         *  If a parameter value cannot be converted to the required type
         */
        virtual unsigned long getPlanAgentLocationKey();

        /**
         *  getPlanAgentSubsystemKey
         *
         *  Returns the subsystem ID used of the Plan Agent that the
         *  Scheduling Agent communicates with.
         *
         *  @exception DatabaseException
         *  If there is a database error.
         *
         *  @exception DataException
         *  If a parameter value cannot be converted to the required type
         */
        virtual unsigned long getPlanAgentSubsystemKey();

        /**
         *  getPlanAgentEntityKey
         *
         *  Returns the entity key of the Plan Agent that the
         *  Scheduling Agent communicates with.
         *
         *  @exception DatabaseException
         *  If there is a database error.
         *
         *  @exception DataException
         *  If a parameter value cannot be converted to the required type
         */
        virtual unsigned long getPlanAgentEntityKey();

		virtual std::string getAssetName();

    private:
        unsigned long  m_operatorID;
		std::string    m_password;
        unsigned long  m_planAgentLocationKey;
        unsigned long  m_planAgentEntityKey;
        unsigned long  m_planAgentSubsystemKey;

        static const std::string OPERATOR_NAME_PARAM;
        static const std::string PLAN_AGENT_LOCATION_NAME_PARAM;
		static const std::string ASSET_NAME;
    };
    
	typedef boost::shared_ptr<SchedulingAgentEntityData> SchedulingAgentEntityDataPtr;

} //close namespace TA_Base_Core

#endif // SCHEDULING_AGENT_ENTITY_DATA_H
