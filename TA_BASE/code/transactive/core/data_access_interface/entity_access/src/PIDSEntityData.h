/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/PIDSEntityData.h $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by:  $Author: grace.koh $
  * 
  * 
  * PIDSEntityData is a concrete implementation of IPIDSEntityData.
  * It proves read only access to PIDS entity data
  *
  */

#ifndef PIDS_ENTITY_DATA_H_INCLUDED
#define PIDS_ENTITY_DATA_H_INCLUDED

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IPIDSEntityData.h"

#if defined(_MSC_VER)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

namespace TA_Base_Core
{
    class PIDSEntityData : public IPIDSEntityData, public EntityData
    {

    public:
        /**
         * TISAgentEntityData (constructor)
         *
         * Constructs a new TISAgentEntityData object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        PIDSEntityData() {};

        /**
         * TISAgent (constructor)
         *
         * Constructs a new TISAgent object with the specified key.
         *
         * @param key The key to this Console in the database.
         */
        PIDSEntityData(unsigned long key);

        virtual ~PIDSEntityData();

    public:

       /**
         * getServerIPAddress
         *
         * Returns IP address of the local PIDS server
         *
         * @return A string representing the IP address
         */
        virtual std::string getServerIPAddress();

	   /**
         * getMessageTimeout
         *
         * Returns the length of time (in milliseconds) the TISAgent should wait after 
		 * sending a message before it decides that a timeout condition has occurred
         *
         * @return Integer value of the timeout
         */
        virtual int getMessageTimeout();
  
	   /**
         * getStationLibrarySynchronisaionTimeout
         *
         * Returns the length of time (in minutes) the TISAgent should wait
		 * while attempting to synchronise the predefined message library
		 * on all Station servers
         *
         * @return Integer value of the timeout
         */
        virtual int getStationLibrarySynchronisationTimeout();

	   /**
         * getMessageRetries
         *
         * Returns the number of times a message should be sent to the PIDS server
		 * without receiving a response, before giving up
         *
         * @return Integer number of retries
         */
        virtual int getMessageRetries();

      /**
         * getServerPort
         *
         * Returns the TCP port the PIDS server is listening
		 * on
         *
         * @return An representing the TCP port
         */
        virtual int getServerPort();

       /**
         * getLocalDBConnectionString
         *
         * Returns the Oracle service name of the local DB instance
		 * 
         *
         * @return std::string
         */
        virtual std::string getLocalDBConnectionString();


       /**
         * getVetting
         *
         * Returns the vetting state
		 * 
         *
         * @return the vetting state
         */
        virtual bool getVetting();


        /**
         * getVetting
         *
         * Sets the vetting state
		 * 
         */
        virtual void setVetting(const bool vetting);


        /**
         * clone
         *
         * Returns a pointer to a new Console object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the Console object.
         */
        static IEntityData* clone(unsigned long key);

        /**
         * getType
         *
         * Returns the console type of this console (i.e. "Console").
         *
         * @return The console type of this console as a std::string
         */
        virtual std::string getType();
		static std::string getStaticType();

        virtual std::string getMultiByteMsgContentEncoding();
        /**
         * invalidate
         *
         * Mark the data contained by this console as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:
        //
        // Copy constructor and assignment operator are not used
        //
        PIDSEntityData( const PIDSEntityData& thePIDSData) {};            
		PIDSEntityData& operator=(const PIDSEntityData &) {};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string SERVER_IP_ADDRESS;
        static const std::string MESSAGE_TIMEOUT;
		static const std::string STATION_LIBRARY_SYNCHRONISATION_TIMEOUT;
		static const std::string SERVER_PORT;
		static const std::string MESSAGE_RETRIES;
		static const std::string STATION_DB_CONNECTION_STRING;
		static const std::string VETTING_STRING;
		static const std::string PARAMETER_TRUE;
		static const std::string PARAMETER_FALSE;
        static const std::string MULTIBYTE_MSGCONTENT_ENCODING;

    };

    
    typedef boost::shared_ptr<PIDSEntityData> PIDSEntityDataPtr;
} //namespace TA_Base_Core

#endif // #ifndef PIDS_ENTITY_DATA_H_INCLUDED
