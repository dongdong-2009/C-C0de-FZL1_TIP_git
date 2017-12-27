/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/ModbusServerAgentEntityData.h $
  * @author HoaVu
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2016/05/13 17:20:00 $
  * Last modified by: $Author: dhanshri $
  * 
  * 
  */

#ifndef MODBUS_SERVER_AGENT_ENTITY_DATA_H
#define MODBUS_SERVER_AGENT_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class ModbusServerAgentEntityData : public virtual EntityData
    {
	public:

		ModbusServerAgentEntityData();
        ModbusServerAgentEntityData( unsigned long key );

        virtual ~ModbusServerAgentEntityData();

		//
		// operations required by IEntityData
		//
   
        /**
         * clone
         *
         * Returns a pointer to a new RTUEntityData object that has been initialised with the 
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

		std::string getServicePort();
		std::string getServiceStartWord();
		std::string getServiceWordLength();


    private:
		          
        ModbusServerAgentEntityData( const ModbusServerAgentEntityData & obj );
		ModbusServerAgentEntityData & operator= ( const ModbusServerAgentEntityData &);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;

    public:
		static const std::string SERVICE_PORT;
		static const std::string SERVICE_START_WORD;
		static const std::string SERVICE_WORD_LENGTH;
    };
    
    typedef boost::shared_ptr<ModbusServerAgentEntityData> ModbusServerAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif
