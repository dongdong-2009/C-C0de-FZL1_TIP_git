/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_agent/src/IEntity.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last mofified by:  $Author: CM $
  *
  * This interface should be implemented by all Entities within TransActive.
  */
#if !defined(IENTITY)
#define IENTITY

#include <vector>
#include <map>

namespace TA_Base_Core
{
    class ConfigUpdateDetails;
}

namespace TA_Base_Bus
{
    class IEntity
    {
    public:

        /**
          * Constructor
          */
        IEntity(){}

        /**
          * Destructor
          */
        virtual ~IEntity(){}

        /**
          * isValid
          *
          * This is a pure vitual method that must be implemented by 
          * all enitity classes. It will return true if the entity is 
          * valid. Otherwise it will return false.
          *
          * @return TRUE if the entity is valid, FALSE otherwise 
          */
        virtual bool isValid() = 0;

        /**
          * start
          *
          * This is a pure vitual method that must be implemented by 
          * all enitity classes. It will start any object specific tasks,
          * such as creating its own thread.
          */
        virtual void start() = 0;

        /**
          * stop
          *
          * This is a pure vitual method that must be implemented by all
          * enitity classes. It will stop any object specific tasks, such 
          * as stopping its own thread. 
          */
        virtual void stop() = 0;

        /**
          * update
          *
          * This method is called by the agent controlling the entity to ask 
          * the entity to update its configuration. Called after an update 
          * configuration event is received.
          */
        virtual void update(const TA_Base_Core::ConfigUpdateDetails& updateEvent) = 0;

        /**
          * remove
          *
          * When a configuration update event is received to delete an entity 
          * from an agent, the agent can call this method to tell the entity 
          * to commence any clean up opperations.
          * 
          * The agent then still has to remove the object from its vector of entities.
          */
        virtual void remove() = 0;

        /**
          * setToControlMode
          *
          * This method can be called by the agent responsible for the entity in
          * the event of its mode of operation changing to control. This enables the
          * entity to perform tasks such as connecting to any field devices it is
          * responsible for communicating with.
          */
        virtual void setToControlMode() = 0;

        /**
          * setToMonitorMode
          *
          * This method can be called by the agent responsible for the entity in
          * the event of its mode of operation changing to monitor. This enables the
          * entity to perform tasks such as disconnecting from any field devices it was
          * communicating with.
          */
        virtual void setToMonitorMode() = 0;

		/**
          * setToStandbyMode
          *
          * This method can be called by the agent responsible for the entity in
          * the event of its mode of operation changing to standby. This enables the
          * entity to perform tasks such as disconnecting from any field devices it was
          * communicating with.
          */
		virtual void setToStandbyMode(){}
    };

    typedef std::vector< std::pair<unsigned long, IEntity*> > IEntityList;
    typedef std::map<unsigned long, IEntity*> IEntityMap;
}

#endif // !defined(IENTITY)
