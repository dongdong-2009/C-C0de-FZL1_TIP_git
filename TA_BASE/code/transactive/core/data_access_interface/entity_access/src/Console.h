/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/Console.h $
  * @author:  Karen Graham
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2017/09/14 14:55:48 $
  * Last modified by:  $Author: huang.wenbo $
  * 
  * 
  * Console is a concrete implementation of IConsole, which is in turn an implementation of
  * IEntityData. It proves read-only access to Console data, and a specific framework
  * for Console parameters.
  *
  */

#if !defined(Console_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
#define Console_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"

#if defined(_MSC_VER)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

namespace TA_Base_Core
{
    class Console : public IConsole, public EntityData
    {

    public:
        /**
         * Console (constructor)
         *
         * Constructs a new Console object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        Console() {};

        /**
         * Console (constructor)
         *
         * Constructs a new Console object with the specified key.
         *
         * @param key The key to this Console in the database.
         */
        Console(unsigned long key);

        virtual ~Console();

    public:
        /**
         * getAccessLevel
         *
         * Returns an unsigned long value indicating the access level of this console
         *
         * @return unsigned long - The access level of the console.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if a particular console parameter cannot be 
         *            converted to the required data type.
         */
        virtual unsigned long getAccessLevel();

        /**
         * getPagingConsoleSourceId
         *
         * Returns the PagingConsoleSourceId for PA live broadcasts for this Console
         *
         * @return The PagingConsoleSourceId for the Console as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */ 
        virtual unsigned long getPagingConsoleSourceId();

        
        /**
         * getPagingConsoleZoneId
         *
         * Returns the PagingConsoleZoneId for PA DVA message playback for this Console
         *
         * @return The PagingConsoleZoneId for the Console as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */ 
        virtual unsigned long getPagingConsoleZoneId();

        /**
         * getPABackEntityKey
         *
         * Returns the PABackEntityKey for PA backup key status for this Console
         *
         * @return The PABackEntityKey for the Console as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */ 
        virtual unsigned long getPABackEntityKey();

		// get CCTV related parameter value, may return empty string
		virtual std::string getCCTVServerIPAddress();
		virtual unsigned long getCCTVServerPort();
		virtual std::string getCCTVServerUserName();
		virtual std::string getCCTVServerPassword();
		virtual std::string getCCTVSiteID();
		virtual std::string getCCTVClientIP();

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
		*
		* gets the origin value to send to the train in train commands 
		*/
		virtual unsigned long getTrainOrigin();

        /**
         * invalidate
         *
         * Mark the data contained by this console as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    protected:


    private:
        //
        // Copy constructor and assignment operator are not used
        //
        Console( const Console& theConsole) {};            
		Console& operator=(const Console &) {};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string ACCESSLEVEL;
        static const std::string PAGING_CONSOLE_SOURCE_ID;
        static const std::string PAGING_CONSOLE_ZONE_ID;
        static const std::string PA_BACK_ENTITY_KEY;

	 	static const std::string TRAIN_ORIGIN;

		static const std::string CCTV_SERVER_IP_ADDRESS;
		static const std::string CCTV_SERVER_PORT;
		static const std::string CCTV_SERVER_USER_NAME;
		static const std::string CCTV_SERVER_PASSWORD;
		static const std::string CCTV_SITE_ID;
		static const std::string CCTV_CLIENT_IP;

    };
    
    typedef boost::shared_ptr<Console> ConsolePtr;
} //close namespace TA_Base_Core

#endif // !defined(Console_C5285B2B_25EE_4db4_BF0E_D80FB033E228__INCLUDED_)
