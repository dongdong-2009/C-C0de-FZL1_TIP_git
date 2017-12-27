/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RadioEntityData.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * RadioEntityData is a concrete implementation of IRadioEntityData, which is in turn an implementation of
  * IEntityData. It proves read-only access to Radio data, and a specific framework
  * for Radio parameters.
  *
  */

#if !defined(Radio_Entity_Data_INCLUDED_)
#define Radio_Entity_Data_INCLUDED_

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IRadioEntityData.h"

#if defined(_MSC_VER)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

namespace TA_Base_Core
{
    class RadioEntityData : public IRadioEntityData, public EntityData
    {

    public:
        /**
         * Radio (constructor)
         *
         * Constructs a new Radio object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        RadioEntityData() {};

        /**
         * Radio (constructor)
         *
         * Constructs a new Radio object with the specified key.
         *
         * @param key The key to this Console in the database.
         */
        RadioEntityData(unsigned long key);

        virtual ~RadioEntityData();

    public:

		//*****************
		// IRadio Methods *
		//*****************

        /**
         * getPrimaryTCPServers
         *
         * Returns the IP address(es) (or machine name(s)) of the primary radio TCPServer(s).
		 * They are returned in the format <name>,<name> when more than 1 is specified.
		 *                            e.g. 123.234.567.432,123.234.567.433
		 *                                 localhost,NTServer
         *
         * @return    IP address of the primary radio TCPServer(s)
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */ 
        virtual std::string getPrimaryTCPServers();

        /**
         * getSecondaryTCPServers
         *
         * Returns the IP address(es) (or machine name(s)) of the secondary radio TCPServer(s).
		 * They are returned in the format <name>,<name> when more than 1 is specified.
		 *                            e.g. 123.234.567.432,123.234.567.433
		 *                                 localhost,NTServer
         *
         * @return    IP address of the secondary radio TCPServer(s)
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */ 
        virtual std::string getSecondaryTCPServers();


		
        /**
         * getXONXOFFTimeout
         *
         * Returns the time interval to be used for timing out an XON signal from TIMS.
         *
         * @return    Time interval (in seconds) as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */ 
        virtual unsigned long getXONXOFFTimeout();

        /**
         * getDefaultVolume
         *
         * Returns the Default volume to be used for every call.
         *
         * @return    volume (0-15) as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */ 
        virtual unsigned long getDefaultVolume();

        virtual std::string getAssetName();



		//**********************
		// Entity Data Methods *
		//**********************

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


		// Add for PDS Dbase Access functionalities
		// implement IRadioEntityData
	
		virtual std::string getUDPServer ();
		virtual std::string getUDPServicePort ();
		virtual long getTis5Timer ();
		virtual long getTis7Timer ();

		/**
		 * getSDSLinkDpName
		 *
		 * Returns the SDS Link Datapoint Name
		 *
		 */
		virtual std::string getSDSLinkDpName();

    private:
        //
        // Copy constructor and assignment operator are not used
        //
        RadioEntityData( const RadioEntityData& theRadio) {};            
		RadioEntityData& operator=(const RadioEntityData &) {};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string PRIMARY_SERVERS;
        static const std::string SECONDARY_SERVERS;
        static const std::string XON_XOFF_TIMEOUT;
		static const std::string ASSET_NAME;
		//TD10963 marc_bugfix support for default volume parameter
		static const std::string DEFAULT_VOLUME;
		
		// add for PDS dbase access functionalities
		static const std::string UDP_SERVER;
		static const std::string UDP_SERVICE_PORT;
		static const std::string TIS5_TIMER;
		static const std::string TIS7_TIMER;
		static const std::string SDS_LINK_DP_NAME;
    };
    
    typedef boost::shared_ptr<RadioEntityData> RadioEntityDataPtr;
} //close namespace TA_Base_Core

#endif // !defined(Radio_Entity_Data_INCLUDED_)
