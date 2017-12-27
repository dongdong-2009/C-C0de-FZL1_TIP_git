#if !defined(CONFIGDATA_H_INCLUDED)
#define CONFIGDATA_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/online_printing_agent/src/ConfigData.h $
  * @author:   Bart Golab
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class stores configuration data required by the agent.
  * If queries are made on data that is currently not cached, 
  * the database is looked up and added to the storage.
  * ConfigData subscribes to online updates.
  * 
  */

namespace TA_Base_App
{
    class ConfigData : public TA_Base_Core::IOnlineUpdatable
    {
    // Operations
    public:
        static ConfigData& getInstance();
        static void destroyInstance();
        ~ConfigData();

        /**
         * registerObserver/deregisterObserver
         *
         * Subscribe/unsubscribe the specified observer for config data changes.
         */
        void registerObserver(IConfigDataObserver& configDataObserver);
        void deregisterObserver(IConfigDataObserver& configDataObserver);

        /**
         * getAgentEntityKey
         *
         * Returns the pkey of the agent entity.
         */
        unsigned long getAgentEntityKey();

        /**
         * getAgentEntityTypeKey
         *
         * Returns the pkey of the agent entity type.
         */
        unsigned long getAgentEntityTypeKey();

        /**
         * getAgentEntityName
         *
         * Returns the name of the agent entity.
         */
        std::string getAgentEntityName();

        /**
         * getAgentEntitySubsystemKey
         *
         * Returns the pkey of the subsystem the agent entity belongs to.
         */
        unsigned long getAgentEntitySubsystemKey();

        /**
         * getAgentEntityLocationKey
         *
         * Returns the pkey of the location the agent entity belongs to.
         */
        unsigned long getAgentEntityLocationKey();

		std::string getAgentEntityAssetName();

        /**
         * getPrintCommand
         *
         * Returns the OS command used to submit print requests to the spooler.
         */
        const std::string& getPrintCommand();

        /**
         * getColumns
         *
         * Returns column specification for print output.
         */
        const ColumnList& getColumns();

        /**
         * getTablePollFrequency
         *
         * How often to look up the event table for new events (in seconds).
         */
        unsigned long getTablePollFrequency();

        /**
         * isPrintingEnabled
         *
         * Returns true if printing is on, false if it's off.
         */
        bool isPrintingEnabled();
  
        /**
         * printAlarmEventsOnly
         *
         * Returns true if only alarm related events should be printing, false for all events.
         */
        bool printAlarmEventsOnly();

        /**
         * processUpdate
         *
         * Implements IOnlineUpdatable::processUpdate().
         */        
        void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

    private:
        ConfigData();
        ConfigData(const ConfigData&);
        ConfigData& operator=(const ConfigData&);


    // Attributes
    private:
        static ConfigData* s_configData;
        static TA_Base_Core::NonReEntrantThreadLockable s_singletonLock;

        typedef std::set<IConfigDataObserver*> ConfigDataObserverSet;
        TA_Base_Core::NonReEntrantThreadLockable m_observerLock;
        ConfigDataObserverSet m_observers;
              
        TA_Base_Core::NonReEntrantThreadLockable m_configDataLock;
        TA_Base_Core::OnlinePrintingAgentEntityData* m_opaEntityData;
        std::string m_printCommand;
        ColumnList m_columns;
    };
}

#endif // !defined(CONFIGDATA_H_INCLUDED)
