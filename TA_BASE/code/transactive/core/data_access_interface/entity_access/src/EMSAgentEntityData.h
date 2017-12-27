/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/EMSAgentEntityData.h $
  * @author Dhanshri
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by: $Author: grace.koh $
  * 
  * EMSAgentEntityData implements the interface IEMSAgentEntityData to provide read-only access
  * to EMSAgent entity data.
  */

#ifndef EMSAgent_ENTITY_DATA_H
#define EMSAgent_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IEMSAgentEntityData.h"

namespace TA_Base_Core
{
    class EMSAgentEntityData : public virtual IEMSAgentEntityData, public virtual EntityData
    {
	public:

		EMSAgentEntityData();
        EMSAgentEntityData( unsigned long key );

        virtual ~EMSAgentEntityData();

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

		static  const std::string getStaticType();

		//
		// operations required by the IEMSAgentEntityData
		//

        /**
         * getIntervalBetweenModbusTransaction
         *
         * Returns the IntervalBetweenModbusTransaction in msecs for EMSAgent entity
         *
         * @return The integer value presenting the time in msecs
         */
        virtual  const int getIntervalBetweenModbusTransaction();

		/**
         * getPollTimeout
         *
         * Returns the PollTimeout in msecs for EMSAgent entity
         *
         * @return The integer value presenting the time in msecs
         */
        virtual  const int getPollTimeout();

        /**
         * getOverlapModbusRequest
         *
         * Returns true if the OverlapModbusRequest of RTU is set yes in EMSAgent,
		 * false ortherwise
         *
         */
		virtual  const bool getOverlapModbusRequest();

		/**
         * getStandardModbusLengthLimit
         *
         * Returns true if the StandardModbusLengthLimit of RTU is set yes in EMSAgent,
		 * false ortherwise
         *
         */
		virtual  const bool getStandardModbusLengthLimit();

        /**
         * getStationName
         *
         * Returns the name of the Station the EMSAgent entity is configured to monitor
         *
         * @return The name of the Station the EMSAgent entity is configured to monitor
         */
        virtual const std::string getStationName();
        
        /**
         * getMaxModbusLength
         *
         * Returns the getMaxModbusLength for the EMSAgent entity 
         *
         */
        virtual  const int getMaxModbusLength();

		virtual  const std::string getAssetName();

		virtual std::string getPollingDiStartAddress();
		virtual std::string getPollingAiStartAddress();
		virtual std::string getPollingMiStartAddress();

		virtual std::string getPollingDiEndAddress();
		virtual std::string getPollingAiEndAddress();
		virtual std::string getPollingMiEndAddress();

		virtual int getPollingDiTimeOut();
		virtual int getPollingAiTimeOut();
		virtual int getPollingMiTimeOut();

		virtual int getPollingDiType();
		virtual int getPollingAiType();
		virtual int getPollingMiType();
		
		virtual std::string getDPProcessorsStartAddresses();
		virtual std::string getDPProcessorsEndAddresses();
		virtual int getAnalogueDpUpdateInterval();
		
    private:
		          
        EMSAgentEntityData( const EMSAgentEntityData & obj );
		EMSAgentEntityData & operator= ( const EMSAgentEntityData &);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;

    public:
        static const std::string EMSAgent_ENTITY_TYPE;
		static const std::string OVERLAPREQUEST;
		static const std::string INTERVAL_BETWN_MODBUSTRX;
		static const std::string STATIONNAME;
		static const std::string MAX_MODBUS_LEN;
		static const std::string ASSET_NAME;
		static const std::string STD_MODBUS_LEN_LMT; 
		static const std::string POLL_TIMEOUT;
		
		static const std::string POLLING_DI_START_ADDRESS;
		static const std::string POLLING_DI_END_ADDRESS;
		static const std::string POLLING_DI_TIMEOUT;
		static const std::string POLLING_DI_TYPE;

		static const std::string POLLING_AI_START_ADDRESS;
		static const std::string POLLING_AI_END_ADDRESS;
		static const std::string POLLING_AI_TIMEOUT;
		static const std::string POLLING_AI_TYPE;

		static const std::string POLLING_MI_START_ADDRESS;
		static const std::string POLLING_MI_END_ADDRESS;
		static const std::string POLLING_MI_TIMEOUT;
		static const std::string POLLING_MI_TYPE;
		
		static const std::string DP_PROCESSORS_START_ADDRESSES;
		static const std::string DP_PROCESSORS_END_ADDRESSES;
		static const std::string DP_PROCESSORS_ANA_DP_UPD_INTERVL;
    };
    
    typedef boost::shared_ptr<EMSAgentEntityData> EMSAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif
