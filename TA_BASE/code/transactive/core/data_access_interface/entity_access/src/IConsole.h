/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/IConsole.h $
  * @author:  Karen Graham
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2017/09/26 09:05:01 $
  * Last modified by:  $Author: limin.zhu $
  * 
  * IConsole is an interface to a Console object. It allows the Console object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IConsole_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IConsole_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{

    class IConsole : public virtual IEntityData
    {

    public:
        virtual ~IConsole() { };

        /**
         * getAccessLevel
         *
         * Returns the Access Level for this Console
         *
         * @return The Access Level for the Console as an unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong amount of data is retrieved.
         */ 
        virtual unsigned long getAccessLevel() =0;

        
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
        virtual unsigned long getPagingConsoleSourceId() =0;

        
        /**
         * getPagingConsoleZoneId
         *
         * Returns the PagingConsoleZoneId for PA live broadcasts for this Console
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
        virtual unsigned long getPagingConsoleZoneId() =0;

		/**
		*
		* gets the origin value to send to the train in train commands 
		*/
		virtual unsigned long getTrainOrigin() = 0; 

		/**
		*
		* gets the PA backup key entity key 
		*/
		virtual unsigned long getPABackEntityKey() = 0; 

		// get CCTV related parameter value, may return empty string
		virtual std::string getCCTVServerIPAddress() = 0;
		virtual unsigned long getCCTVServerPort() = 0;
		virtual std::string getCCTVServerUserName() = 0;
		virtual std::string getCCTVServerPassword() = 0;
		virtual std::string getCCTVSiteID() = 0;
		virtual std::string getCCTVClientIP() = 0;
    };

    typedef boost::shared_ptr<IConsole> IConsolePtr;
    typedef std::vector<IConsolePtr> IConsolePtrList;
} //close namespace TA_Base_Core

#endif // !defined(IConsole_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
