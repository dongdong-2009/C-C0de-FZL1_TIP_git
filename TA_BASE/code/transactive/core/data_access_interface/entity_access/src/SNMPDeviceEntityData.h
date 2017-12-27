/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/SNMPDeviceEntityData.h $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * SNMPDeviceEntityData implements the interface ISNMPDeviceEntityData to provide read-only access
  * to RTU entity data.
  */

#ifndef SNMP_DEVICE_ENTITY_DATA_H
#define SNMP_DEVICE_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class SNMPDeviceEntityData : public EntityData
    {

	public:

		SNMPDeviceEntityData();
        SNMPDeviceEntityData ( unsigned long key );

        virtual ~SNMPDeviceEntityData();

		//
		// operations required by IEntityData
		//
   
        /**
         * clone
         *
         * Returns a pointer to a new SNMPDeviceEntityData object that has been initialised with the 
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
		// operations required by the SNMPDeviceEntityData
		//

        /**
         * getScanInterval
         *
         * Returns the scan interval of the SNMP device
         *
         * @return The integer value presenting the scan interva 
         */
        virtual int getScanInterval();

        /**
         * getSNMPGetPort
         *
         * Returns get data port of SNMP device
         *
         * @return The unsigned long value to indicate the get data port
         */
        virtual unsigned long getSNMPGetPort();

        /**
         * getSNMPGetTimeout
         *
         * Returns the get data timeout in secs for SNMP device
         *
         * @return The integer value presenting the get data timeout in secs
         */
        virtual int getSNMPGetTimeout();

        /**
         * getSNMPGetRetryTimes
         *
         * Returns the retry times when get data for SNMP device
         *
         * @return The retry times when get data
         */
        virtual unsigned long getSNMPGetRetryTimes();

        /**
         * getSNMPSetPort
         *
         * Returns set data port of SNMP device
         *
         * @return The unsigned long value to indicate the set data port
         */
        virtual unsigned long getSNMPSetPort();

        /**
         * getSNMPSetTimeout
         *
         * Returns the set data timeout in secs for SNMP device
         *
         * @return The integer value presenting the set data timeout in secs
         */
        virtual int getSNMPSetTimeout();

        /**
         * getSNMPSetRetryTimes
         *
         * Returns the retry times when set data for SNMP device
         *
         * @return The retry times when set data
         */
        virtual unsigned long getSNMPSetRetryTimes();
		
		/**
         * getSNMPVersion
         *
         * Returns the SNMP version of SNMP device
         *
         * @return The string value presenting the SNMP version
         */
		virtual std::string getSNMPVersion();

		/**
         * getSNMPCommunity
         *
         * Returns the SNMP community of SNMP device
         *
         * @return The string value presenting the SNMP community
         */
		virtual std::string getSNMPCommunity();

        		
    private:

		std::string getStringData ( const std::string & parameterName );
		bool getBooleanData ( const std::string & parameterName );
		int getIntegerData ( const std::string & parameterName );
		unsigned long getUnsignedLongData ( const std::string & parameterName );
		double getRealData  ( const std::string & parameterName );

    private:
		          
        SNMPDeviceEntityData ( const SNMPDeviceEntityData & obj ){};            
		SNMPDeviceEntityData & operator= ( const SNMPDeviceEntityData &){};
        // 
        // The entity type is fixed for all instances of this class.
        // 

	public:

        static const std::string SNMP_ENTITY_TYPE;
		static const std::string SNMP_SCAN_INTERVAL;	//ms
		static const std::string SNMP_GET_PORT;
		static const std::string SNMP_GET_TIMEOUT;		//ms
		static const std::string SNMP_GET_RETRY_TIMES;
		static const std::string SNMP_SET_PORT;
		static const std::string SNMP_SET_TIMEOUT;
		static const std::string SNMP_SET_RETRY_TIMES;

		static const std::string SNMP_VERSION;	//1, 2C, 3
		static const std::string SNMP_COMMUNITY;
		static const std::string SNMP_SECURITY_LEVEL;
		static const std::string SNMP_USER_NAME;
		static const std::string SNMP_AUTH_PROTOCOL;
		static const std::string SNMP_AUTH_PASS;
		static const std::string SNMP_PRIV_PROTOCOL;
		static const std::string SNMP_PRIV_PASS;
    };
    
    typedef boost::shared_ptr<SNMPDeviceEntityData> SNMPDeviceEntityDataPtr;
} //close namespace TA_Base_Core

#endif
