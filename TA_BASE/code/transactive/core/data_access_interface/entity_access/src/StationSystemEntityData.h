/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/StationSystemEntityData.h $
  * @author HoaVu
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2015/01/22 10:48:00 $
  * Last modified by: $Author: haijun.li $
  * 
  * StationSystemEntityData implements the interface IStationSystemEntityData to provide
  * read-only access to Station System entity data.
  */

#ifndef STATIONSYSTEM_ENTITY_DATA_H
#define STATIONSYSTEM_ENTITY_DATA_H

#include <vector>
#include <map>
#include "core/data_access_interface/entity_access/src/IStationSystemEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"


namespace TA_Base_Core
{
    class StationSystemEntityData : public EntityData
    {
	public:
		StationSystemEntityData();
        StationSystemEntityData ( unsigned long key );

        virtual ~StationSystemEntityData();

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
		// operations required by the IStationSystemEntityData
		//

        /**
         * getSlaveID
         *
         * Returns the slave ID of the Station System entity
         *
         * @return The integer value presenting the slave ID 
         */
        virtual int getSlaveID();

		/**
		* getStatusWordNumber
		*
		* Returns the status word index of the Station System entity
		*
		* @return The integer value presenting the status word number 
		*/
		virtual int getStatusWordNumber();

        /**
         * getOutputStartBitAddress
         *
         * Returns the start of the output address (bit format) of this Station System
         *
         * @return The start of the output address
         */
        virtual unsigned long getOutputStartBitAddress();


        /**
         * getOutputEndBitAddress
         *
         * Returns the end of the output address (bit format) of this Station System
         *
         * @return The end of the output address
         */
        virtual unsigned long getOutputEndBitAddress();

        /**
         * getOutputStartWordAddress
         *
         * Returns the start of the output address (word format) of this Station System
         *
         * @return The start of the output address
         */
        virtual unsigned long getOutputStartWordAddress();


        /**
         * getOutputEndWordAddress
         *
         * Returns the end of the output address (word format) of this Station System
         *
         * @return The end of the output address
         */
        virtual unsigned long getOutputEndWordAddress();

        /**
         * getPollingStartAddress
         *
         * Returns the start of the polling address of this Station System
         *
         * @return The start of the polling address
         */
        virtual unsigned long getPollingStartAddress();


        /**
         * getPollingEndAddress
         *
         * Returns the end of the polling address of this Station System
         *
         * @return The end of the polling address
         */
        virtual unsigned long getPollingEndAddress();


		// TD16182
		/**
         * getBlockAddresses
         *
         * Returns the start of the block1 address of this Station System
         *
         * @return The start of the block1 address
         */
        virtual const std::vector<std::pair<unsigned long, unsigned long> >& getBlockAddresses(); 


        /**
         * getOutputServiceNumber
         *
         * Returns the service number for outputing data to this Station System
         *
         * @return The service number for outputing data
         */
        virtual const std::string getOutputServiceNumber();
		/**
         * getDigitalWriteFunCode
         *
         * Returns modbus function code used to write digital data 
         *
         * @returns modbus function code
         */
		virtual const std::string getDigitalWriteFunCode();
		virtual  int  getGroupID();
		virtual  int  getIntegerData ( const std::string & parameterName );
        
    private:
		          
        StationSystemEntityData ( const StationSystemEntityData & obj ){};
		StationSystemEntityData & operator= ( const StationSystemEntityData &){};

		int getIntParameterValue(const std::string& parameterName);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const std::string STATIONSYSTEM_SLAVE_ID;
		static const std::string STATUS_WORD_NUMBER;
		static const std::string POLLING_START_ADDRESS;
		static const std::string POLLING_END_ADDRESS;
		static const std::string OUTPUT_START_BIT_ADDRESS;
		static const std::string OUTPUT_END_BIT_ADDRESS;
		static const std::string OUTPUT_START_WORD_ADDRESS;
		static const std::string OUTPUT_END_WORD_ADDRESS;
		static const std::string OUTPUT_SERVICE_NUMBER;
		static const std::string BLOCK_START_ADDRESS; // TD16182
		static const std::string BLOCK_END_ADDRESS;   // TD16182
		static const std::string STATIONSYSTEM_GROUP_ID;   // TD16182
		static const std::string DIGITAL_WRITE_FUN_CODE;
		std::vector<std::pair<unsigned long, unsigned long> > m_blockAddresses; //wenching
    };
    
    typedef boost::shared_ptr<StationSystemEntityData> StationSystemEntityDataPtr;
} //close namespace TA_Base_Core

#endif
