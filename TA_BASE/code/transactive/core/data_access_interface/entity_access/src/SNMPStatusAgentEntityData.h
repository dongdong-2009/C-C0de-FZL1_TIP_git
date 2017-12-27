
#ifndef SNMP_STATUS_AGENT_ENTITY_DATA_H
#define SNMP_STATUS_AGENT_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class SNMPStatusAgentEntityData : public virtual EntityData
    {
	public:

		SNMPStatusAgentEntityData();
        SNMPStatusAgentEntityData( unsigned long key );

        virtual ~SNMPStatusAgentEntityData();

		//
		// operations required by IEntityData
		//
   
        /**
         * clone
         *
         * Returns a pointer to a new SNMPStatusAgentEntityData object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the object.
         */
        static IEntityData* clone ( unsigned long key);

        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

        /**
         * getType
         *
         * Returns the entity type of this entity (i.e. "Camera").
         *
         * @return The entity type of this entity as a std::string
         */
        std::string getType();
		static std::string getStaticType();

		//SNMPStatusAgentEntityData
		int SNMPStatusAgentEntityData::getSNMPTrapListenPort();

    private:
		          
        SNMPStatusAgentEntityData( const SNMPStatusAgentEntityData & obj );
		SNMPStatusAgentEntityData & operator= ( const SNMPStatusAgentEntityData &);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const std::string SNMP_TRAP_LISTEN_PORT;
    };
    
    typedef boost::shared_ptr<SNMPStatusAgentEntityData> SNMPStatusAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif
