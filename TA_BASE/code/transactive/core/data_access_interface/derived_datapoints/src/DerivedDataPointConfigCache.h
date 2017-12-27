#ifndef DERIVED_DATAPOINT_CONFIG_CACHE_H
#define DERIVED_DATAPOINT_CONFIG_CACHE_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/derived_datapoints/src/DerivedDataPointConfigCache.h $
  * @author Bart Golab
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2017/05/26 20:08:22 $
  * Last modified by: $Author: yong.liu $
  *
  * Singleton class used to access and cache derived datapoint configuration data items.
  * Supported currently are derived states and input/output associations.
  * This class is a replacement for the static utility DerivedDataPointHelperAccessFactory
  * (formerly in DerivedDataPointHelper.h).
  *
  */

#include <map>
#include <vector>
#include <string>
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "ace/RW_Mutex.h"
#include "ace/Guard_T.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"

namespace TA_Base_Core
{
    struct DerivedState
    {
        unsigned long stateKey;
        int stateValue;
        std::string stateDescription;
        std::string stateReturnCondition;
        bool isAlarmEnabled;
        int alarmSeverity;
        int alarmDelay;
        std::string alarmMessage;
        std::string alarmMms;        
    };

    struct InputAssociation
    {
        unsigned long inputAssociationKey;
        unsigned long inputDataPointKey;
        int inputCalculationOrder;
        double inputValue;    
    };

    struct OutputAssociation
    {
        unsigned long outputAssociationKey;
        unsigned long outputDataPointKey;
        int outputValue;    
    };


    typedef std::vector<DerivedState> DerivedStateList;
    typedef std::vector<InputAssociation> InputAssociationList;
    typedef std::vector<OutputAssociation> OutputAssociationList;


    /**
      * Maps a key to a set of associated config data items. This is
      * the base for derived state and input/output association maps.
      *
      */
    template <typename ConfigItemType>
    class DdpConfigData
    {
    public:
        typedef std::vector<ConfigItemType> ConfigItemList;
        typedef std::map<unsigned long, ConfigItemList> ConfigItemMap;

    public:
        DdpConfigData();
        virtual ~DdpConfigData();

        ConfigItemList getData(unsigned long key);

		/**
		 * bulkLoadByAgentKey
		 * add by: $liuym $
		 * 2008/5/5                                                               
		 */
		virtual void bulkLoadByAgentKey(unsigned long agentKey) = 0;
        virtual void bulkLoadByEquipmentDataNodeKey(unsigned long dataNodeKey) = 0;
    protected:
        virtual void populate(ConfigItemMap& dataMap) = 0;
		virtual ConfigItemList populateData(unsigned long key) = 0;

    private:
        DdpConfigData(const DdpConfigData&);
        DdpConfigData& operator=(const DdpConfigData&);

    protected:
        volatile bool m_initialised;
        volatile bool m_hasBulkLoaded;
		//std::map< unsigned long, bool > m_ddpLoaded;
        ConfigItemMap m_dataMap;
		ACE_RW_Mutex  m_rwMutex;
        //NonReEntrantThreadLockable m_dataMapLock;
    };
    

    /**
      * Maps derived data point entity keys to a set of derived states.
      *
      */
    class DerivedStateData : public DdpConfigData<DerivedState>
    {
    public:
        DerivedStateData() {};
        virtual ~DerivedStateData() {};

		/**
		 * bulkLoadByAgentKey
		 * add by: $liuym $
		 * 2008/5/5                                                               
		 */
		virtual void bulkLoadByAgentKey(unsigned long agentKey);
        virtual void bulkLoadByEquipmentDataNodeKey(unsigned long dataNodeKey);

    protected:
        virtual void populate(ConfigItemMap& dataMap);
		virtual DdpConfigData<DerivedState>::ConfigItemList populateData(unsigned long key);
        virtual std::string trim(const std::string& text) const; // returns empty string if text has white spaces only
        
    private:
        DerivedStateData(const DerivedStateData&);
        DerivedStateData& operator=(const DerivedStateData&);
    };


    /**
      * Maps derived data point entity keys to a set of input associations.
      *
      */
    class InputAssociationData : public DdpConfigData<InputAssociation>
    {
    public:
        InputAssociationData() {};
        virtual ~InputAssociationData() {};

		/**
		 * bulkLoadByAgentKey
		 * add by: $liuym $
		 * 2008/5/5                                                               
		 */
		virtual void bulkLoadByAgentKey(unsigned long agentKey);
        virtual void bulkLoadByEquipmentDataNodeKey(unsigned long dataNodeKey);

    protected:
        virtual void populate(ConfigItemMap& dataMap);
		virtual DdpConfigData<InputAssociation>::ConfigItemList populateData(unsigned long key);

    private:
        InputAssociationData(const InputAssociationData&);
        InputAssociationData& operator=(const InputAssociationData&);
    };


    /**
      * Maps derived data state keys to a set of output associations.
      *
      */
    class OutputAssociationData : public DdpConfigData<OutputAssociation>
    {
    public:
        OutputAssociationData() {};
        virtual ~OutputAssociationData() {};

		/**
		 * bulkLoadByAgentKey
		 * add by: $liuym $
		 * 2008/5/5                                                               
		 */
		virtual void bulkLoadByAgentKey(unsigned long agentKey);
        virtual void bulkLoadByEquipmentDataNodeKey(unsigned long dataNodeKey);

    protected:
        virtual void populate(ConfigItemMap& dataMap);
		virtual DdpConfigData<OutputAssociation>::ConfigItemList populateData(unsigned long key);

    private:
        OutputAssociationData(const OutputAssociationData&);
        OutputAssociationData& operator=(const OutputAssociationData&);
    };


    class DerivedDataPointConfigCache
    {
    public:
        virtual ~DerivedDataPointConfigCache() {};

        /**
          * getInstance
          *
          * Returns the single instance of this class.
          *
          * @return A reference to an instance of DerivedDataPointConfigCache.
          */
        static DerivedDataPointConfigCache& getInstance();

        /**
          * getDerivedStates
          *
          * Returns derived state data for the specified derived data point.
          *
          * @return A vector of DerivedState data objects. Empty if the data cannot be accessed.
          */
        DerivedStateList getDerivedStates(unsigned long derivedDataPointKey);

        /**
          * getInputAssociations
          *
          * Returns input association data for the specified derived data point.
          *
          * @return A vector of InputAssociation data objects. Empty if the data cannot be accessed.
          */
        InputAssociationList getInputAssociations(unsigned long derivedDataPointKey);

        /**
          * getOutputAssociations
          *
          * Returns derived output  association data for the specified derived state.
          *
          * @return A vector of OutputAssociation data objects. Empty if the data cannot be accessed.
          */
        OutputAssociationList getOutputAssociations(unsigned long derivedStateKey);

		/**
		 * bulkLoadByAgentKey
		 * add by: $liuym $
		 * 2008/5/5                                                               
		 */
		void bulkLoadByAgentKey(unsigned long agentKey);

        void bulkLoadByEquipmentDataNodeKey(unsigned long dataNodeKey);

    private:
        DerivedDataPointConfigCache() {};
        DerivedDataPointConfigCache(const DerivedDataPointConfigCache &);
        DerivedDataPointConfigCache& operator=(const DerivedDataPointConfigCache &);

    private:
        static DerivedDataPointConfigCache* s_instance;
        static NonReEntrantThreadLockable s_instanceLock;

        DerivedStateData m_derivedStates;
        InputAssociationData m_inputAssociations;
        OutputAssociationData m_outputAssociations;
    };



    //--------------------------------------------------------------------------------------
    // DdpConfigData Implementation
    //--------------------------------------------------------------------------------------

    template <typename ConfigItemType>
    DdpConfigData<ConfigItemType>::DdpConfigData() : m_initialised(false), m_hasBulkLoaded(false)
    {
    }


    template <typename ConfigItemType>
    DdpConfigData<ConfigItemType>::~DdpConfigData()
    {
        //ThreadGuard guard(m_dataMapLock);
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_rwMutex);
        m_dataMap.clear();
        m_initialised = false;
    }
    

    template <typename ConfigItemType>
    typename DdpConfigData<ConfigItemType>::ConfigItemList DdpConfigData<ConfigItemType>::getData(unsigned long key)
    {
        /*ThreadGuard guard(m_dataMapLock);

        // The map is populated the first time this function is called.
        // From there on the cached results are returned.
//        if (!m_initialised)
//        {
//            populate(m_dataMap);
//            m_initialised = true;
//        }

		//std::map< unsigned long, bool >::iterator it = m_ddpLoaded.find(key);

		//if(it == m_ddpLoaded.end())
//		{
            // Only try to populate data for this entity if it's not hold by the agent itself.
            // we don't want to load db each time if this entity actually don't have any data.
//            TA_Base_Core::IEntityDataPtr entityData = DataNodeAccessFactory::getInstance()->getEntityData(key);
//            if (entityData.get() == NULL)
//            {
//                return populateData(key);
//            }
//		}
		

		
        typename ConfigItemMap::const_iterator dataMapIt(m_dataMap.find(key));
        if (dataMapIt == m_dataMap.end())
        {
//            if (!m_hasBulkLoaded)
//            {
                // had not been bulk loaded yet.
                return populateData(key);
//            }

//            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "%s: No data for key %lu", typeid(ConfigItemType).name(), key);
//            return ConfigItemList();
        }

        return dataMapIt->second;*/

		{
			ACE_Read_Guard<ACE_RW_Mutex> guard(m_rwMutex);
			typename ConfigItemMap::const_iterator dataMapIt(m_dataMap.find(key));
			if (dataMapIt != m_dataMap.end())
			{
				return dataMapIt->second;
			}
		}

		{
			ACE_Write_Guard<ACE_RW_Mutex> guard(m_rwMutex);
			typename ConfigItemMap::const_iterator dataMapIt(m_dataMap.find(key));
			if(dataMapIt != m_dataMap.end())
			{
				return dataMapIt->second;
			}
			else
			{
				return populateData(key);
			}
		}
    }

} // namespace TA_Base_Core

#endif // !defined(DERIVED_DATAPOINT_CONFIG_CACHE_H)

