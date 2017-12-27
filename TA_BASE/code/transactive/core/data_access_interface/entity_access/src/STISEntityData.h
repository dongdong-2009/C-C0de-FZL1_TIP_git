/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/STISEntityData.h $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * STISEntityData is a concrete implementation of ISTISEntityData.
  * It proves read only access to STIS entity data
  *
  */

#ifndef STIS_ENTITY_DATA_H_INCLUDED
#define STIS_ENTITY_DATA_H_INCLUDED

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/ISTISEntityData.h"

#if defined(_MSC_VER)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

namespace TA_Base_Core
{
    class STISEntityData : public ISTISEntityData, public EntityData
    {

    public:
        /**
         * TISAgentEntityData (constructor)
         *
         * Constructs a new TISAgentEntityData object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        STISEntityData() {};

        /**
         * TISAgent (constructor)
         *
         * Constructs a new TISAgent object with the specified key.
         *
         * @param key The key to this Console in the database.
         */
        STISEntityData(unsigned long key);

        virtual ~STISEntityData();

    public:

       /**
         * getServerIPAddress
         *
         * Returns IP address of the local STIS server
         *
         * @return A string representing the IP address
         */
        virtual std::string getServerIPAddress();

        virtual std::string getCCTIPAddress();

        virtual int getCCTPort();


		// sftp related parameters
		virtual std::string getStisSftpIPAddress();
		virtual std::string getStisSftpUserName();
		virtual std::string getStisSftpPassword();
		virtual int getStisSftpPort();

		virtual std::string getCCTSftpIPAddress();
		virtual std::string getCCTSftpUserName();
		virtual std::string getCCTSftpPassword();
		virtual int getCCTSftpPort();

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
         * Returns the number of times a message should be sent to the STIS server
		 * without receiving a response, before giving up
         *
         * @return Integer number of retries
         */
        virtual int getMessageRetries();

      /**
         * getServerPort
         *
         * Returns the TCP port the STIS server is listening
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
        STISEntityData( const STISEntityData& theSTISData) {};            
		STISEntityData& operator=(const STISEntityData &) {};

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
		static const std::string CCT_IP_ADDRESS;
		static const std::string CCT_PORT;
		// sftp related parameters name
		static const std::string STIS_SFTP_IP_ADDRESS;
		static const std::string STIS_SFTP_PORT;
		static const std::string STIS_SFTP_USER_NAME;
		static const std::string STIS_SFTP_PASSWORD;

		static const std::string CCT_SFTP_IP_ADDRESS;
		static const std::string CCT_SFTP_PORT;
		static const std::string CCT_SFTP_USER_NAME;
		static const std::string CCT_SFTP_PASSWORD;
    };

    
    typedef boost::shared_ptr<STISEntityData> STISEntityDataPtr;
} //namespace TA_Base_Core

#endif // #ifndef STIS_ENTITY_DATA_H_INCLUDED
