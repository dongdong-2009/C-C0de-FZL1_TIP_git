/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/PEnergyUnitEntityData.h $
  * @author Liu Yong
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/22 10:48:00 $
  * Last modified by: $Author: haijun.li $
  * 
  * PEnergyUnitEntityData  to provide read-only access to PEnergyUnit entity data.
  */

#ifndef PENERGY_UNIT_ENTITY_DATA_H
#define PENERGY_UNIT_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class PEnergyUnitEntityData : public EntityData
    {

	public:

		PEnergyUnitEntityData();
        PEnergyUnitEntityData ( unsigned long key );

        virtual ~PEnergyUnitEntityData();

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

		//
		// operations required by the RTUEntityData
		//

        virtual int getPort();
        virtual int getRemoteAddress();
		
        virtual const std::string getEnergyDataStartAddr();
        virtual const std::string getEnergyDataEndAddr();
        virtual int getEnergyDataPollInSec();
        virtual int getEnergyDataFrequency();
		
        virtual const std::string getMeasureDataStartAddr();
        virtual const std::string getMeasureDataEndAddr();
        virtual int getMeasureDataPollInSec();
        virtual int getMeasureDataFrequency();

		
        virtual int getIEC102SendRetryTimes();
        virtual int getIEC102InvalidDataRetryTimes();
        virtual int getIEC102MessageResponseTimeOutInMs();		

	private:

		std::string getStringData ( const std::string & parameterName );
		bool getBooleanData ( const std::string & parameterName );
		int getIntegerData ( const std::string & parameterName );
		unsigned long getUnsignedLongData ( const std::string & parameterName );
		double getRealData  ( const std::string & parameterName );

    private:
		          
        PEnergyUnitEntityData ( const PEnergyUnitEntityData & obj ){};            
		PEnergyUnitEntityData & operator= ( const PEnergyUnitEntityData &){};
        // 
        // The entity type is fixed for all instances of this class.
        // 

	public:

        static const std::string ENTITY_TYPE;
		static const std::string PORT;
		static const std::string REMOTE_ADDRESS;
		static const std::string ENERGY_DATA_START_ADDRESS;
		static const std::string ENERGY_DATA_END_ADDRESS;
		static const std::string ENERGY_DATA_POLLING_INSEC;
		static const std::string ENERGY_DATA_FREQUENCY;
		static const std::string MEASURE_DATA_START_ADDRESS;
		static const std::string MEASURE_DATA_END_ADDRESS;
		static const std::string MEASURE_DATA_POLLING_INSEC;
		static const std::string MEASURE_DATA_FREQUENCY;
		
		static const std::string IEC102_SEND_RETRY_TIMES;
		static const std::string IEC102_INVALID_DATA_RETRY_TIMES;
 		static const std::string IEC102_MSG_RESPONSE_TIMEOUT_INMS;
    };
    
    typedef boost::shared_ptr<PEnergyUnitEntityData> PEnergyUnitEntityDataPtr;
} //close namespace TA_Base_Core

#endif
