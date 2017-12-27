#ifndef LESSAGENTENTITYDATA_H_INCLUDED
#define LESSAGENTENTITYDATA_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/LessAgentEntityData.h $
  * @author Gregg Kirkpatrick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  *
  */

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class LessAgentEntityData : public EntityData
    {

    public:
		static const std::string PRIMARY_OPC_MACHINE_NAME;
		static const std::string PRIMARY_OPC_SERVER_NAME;
		static const std::string SECONDARY_OPC_MACHINE_NAME;
		static const std::string SECONDARY_OPC_SERVER_NAME;
		static const std::string LESS_OPC_GUID;
		static const std::string LESS_OPC_SERVER_NAME;
		static const std::string LESS_OPC_SERVER_DESCRIPTION;
		static const std::string LESS_OPC_SERVER_EXECUTABLE_PATH;
		static const std::string LESS_OPC_SERVER_RATE_IN_MS;
		static const std::string LESS_OPC_SERVER_QUALIFIER;

        LessAgentEntityData();
        LessAgentEntityData(unsigned long key);

        virtual ~LessAgentEntityData();

    public:

        /**
         * getType
         *
         * Returns the type of this entity. This method must be callable when the object
         * is constructed using the default constructor. The entity type string should be
         * stored as a static const variable in the concrete class. The entity type should
         * not be loaded from the database (note that this method defines no exceptions).
         *
         * @return The entity type of this entity as a std::string
         */
        virtual std::string getType();
		static std::string getStaticType();

		/**
         * getPrimaryOpcMachineName
         *
		 * The name of the machine running the primary external OPC server. 
         */
		std::string getPrimaryOpcMachineName();

		/**
         * getPrimaryOpcServerName
         *
		 * The name of the primary external OPC server. 
         */
		std::string getPrimaryOpcServerName();

		/**
         * getSecondaryOpcMachineName
         *
		 * The name of the machine running the secondary external OPC server. 
         */
		std::string getSecondaryOpcMachineName();

		/**
         * getSecondaryOpcServerName
         *
		 * The name of the secondary external OPC server. 
         */
		std::string getSecondaryOpcServerName();

		/**
         * getLessOpcGuid
         *
		 * The GUID of the Less OPC server.
         */
		std::string getLessOpcGuid();

		/**
         * getLessOpcServerName
         *
		 * The name of the Less OPC server.
         */
		std::string getLessOpcServerName();

		/**
         * getLessOpcServerDescription
         *
		 * The description of the Less OPC server.
         */
		std::string getLessOpcServerDescription();

		/**
         * getLessOpcServerExecutablePath
         *
		 * The executable path of the Less OPC server.
         */
		std::string getLessOpcServerExecutablePath();

		/**
         * getLessOpcServerRateInMs
         *
		 * The refresh rate in milliseconds of the Less OPC server.
         */
		unsigned long getLessOpcServerRateInMs();

		/**
         * getLessOpcServerQualifier
         *
		 * The nested tag qualifier of the Less OPC server.
         */
		char getLessOpcServerQualifier();
		

        /**
         * clone
         *
         * Returns a pointer to a new LessAgentEntityData object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the LessAgentEntityData object.
         */
        static IEntityData* clone(unsigned long key);
        
        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

    protected:

        /**
         * reload
         *
         * (Re)loads the parameter data from the database.
         *
         * @exception DatabaseException If there is a database error.
         * @exception DataException If a parameter value cannot be converted to the required
         *            type
         */
        void reload();

	private:

        LessAgentEntityData( const LessAgentEntityData&);            
		LessAgentEntityData& operator=(const LessAgentEntityData&);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const char DEFAULT_QUALIFIER;
		static const unsigned long DEFAULT_RATE;

    };
    
    typedef boost::shared_ptr<LessAgentEntityData> LessAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif // !defined(Camera_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
