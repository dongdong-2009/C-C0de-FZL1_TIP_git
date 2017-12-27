#if !defined(CtaRuntime_E8414979_D878_4acc_8D4E_01C2899E178E__INCLUDED_)
#define CtaRuntime_E8414979_D878_4acc_8D4E_01C2899E178E__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/process_management/src/CtaRuntime.h $
  * @author:  K. Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Implementation of ItaRuntime
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/process_management/public_interfaces/ItaRuntime.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{
    class IEntityData;  // Forward Declaration

    class CtaRuntime : public virtual ItaRuntime 
    {
    public:
        /**
         * Constructor
         *
         * @param entityData - The entity data object for the application (ie agent) that is
         *                     raising alarms.
         *
         * @exception DataException - If the application data cannot be loaded from the entity passed
         *                            in then this exception will be thrown.
         * @exception DatabaseException - If the application data cannot be loaded from the database
         *                                then this exception will be thrown.
         */
	    CtaRuntime(IEntityDataPtr agentConfiguration);


        /**
         * Destructor
         */
	    virtual ~CtaRuntime();


	    /**
         * getConfigParameterValue
         *
         * This returns the value for a stored parameter
         * Pre: This method should only be called if the IEntityData passed into the constructor
         * can be cast to a default entity. If it cannot then the agent will need to provide a specific
         * interface to provide the configuration values.
         *
	     * @param name The name of the parameter to retrieve
         *
         * @return string The value for the parameter
	     * 
         * @exception CtaRuntimeEntityParameterNotFoundException if the parameter is not found
	     */
	    virtual std::string getConfigParameterValue(const std::string& name);


        /**
         * getMySessionId
         *
         * @return string The current session id
         */
	    virtual std::string getMySessionId() const;


        /**
         * getMyProcessLocationKey
         *
         * @return unsigned long The key of the location where the application/agent is running
         */
	    virtual unsigned long getMyProcessLocationKey() const;


        /**
         * getMyEntityKey
         *
         * @return unsigned long The key of the agent/application entity from the database
         */
	    virtual unsigned long getMyEntityKey() const
        {
            return m_entityKey;
        }


        /**
         * getMyEntityName
         *
         * @return string The name of the agent/application entity from the database
         */
        virtual std::string getMyEntityName() const
        {
            return m_entityName;
        }


        /**
         * getMyEntityType
         *
         * @return string The name of the agent/application entity type from the database
         */
	    virtual std::string getMyEntityType() const
        {
            return m_entityType;
        }


        /**
         * getMyEntityTypeKey
         *
         * @return unsigned long The key of the agent/application entity type from the database
         */
	    virtual unsigned long getMyEntityTypeKey() const
        {
            return m_entityTypeKey;
        }

        
        /**
         * getMyEntityLocationKey
         *
         * @return unsigned long The key of the agent/application location from the database
         */
	    virtual unsigned long getMyEntityLocationKey() const
        {
            return m_entityLocation;
        }


        /**
         * getMyEntitySubsystemKey
         *
         * @return unsigned long The key of the agent/application subsystem from the database
         */
	    virtual unsigned long getMyEntitySubsystemKey() const
        {
            return m_entitySubsystem;
        }

    private:
        /**
         * Disable copy Constructor.
         */
        CtaRuntime(const CtaRuntime& rhs);


        /**
         * Disable assignment operator
         */
        const CtaRuntime& operator=(const CtaRuntime& rhs);


        /** 
         * The entity information to cache on construction and return when requested. We cache it so
         * there is no possibility that exceptions can be thrown.
         */
        std::string m_entityName;
        unsigned long m_entityKey;
        unsigned long m_entityTypeKey;
        std::string m_entityType;
        unsigned long m_entityLocation;
        unsigned long m_entitySubsystem;


        /**
         * The entity database object to use when retrieving configuration parameters
         */
        IEntityDataPtr m_entityDatabaseObject;

    };
}
#endif // !defined(CtaRuntime_E8414979_D878_4acc_8D4E_01C2899E178E__INCLUDED_)
