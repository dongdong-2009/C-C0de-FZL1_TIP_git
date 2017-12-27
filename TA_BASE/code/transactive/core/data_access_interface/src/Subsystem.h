/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/Subsystem.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 12:15:10 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * Subsystem is an implementation of ISubsystem. It holds the data specific to an Subsystem entry
  * in the database, and allows read-only access to that data.
  *
  */

#if !defined(Subsystem_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define Subsystem_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/src/ISubsystem.h"
#include "core/data_access_interface/src/SubsystemHelper.h"

namespace TA_Base_Core
{
    class Subsystem : public ISubsystem
    {

    public:

       /**
         * Constructor
         *
         * Construct a complete Subsystem, this will set isValidData to false
         *
         * @param key The key of this Subsystem in the database
         */
        Subsystem( const unsigned long key );

		/**
		 * Constructor
		 *
		 * Construct a Subsystem class based around dataset
		 */
		Subsystem(const unsigned long row, TA_Base_Core::IData& data);

        /**
         * Destructor
         */
        virtual ~Subsystem();


        /**
         * getKey
         *
         * Returns the key for this Subsystem.
         *
         * @return The key for this Subsystem as an unsigned long.
         */
        virtual unsigned long getKey();

        /**
         * getSystemKey
         *
         * Returns the key for this system.
         *
         * @return The key for this system as an unsigned long.
         */
        virtual unsigned long getSystemKey();

        /**
         * getName
         *
         * Returns the name of this Subsystem. If this is the first time data for this Subsystem
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this Subsystem as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();		

		/**
		* getDisplayName
		*
		* Returns the display name of this Subsystem. If this is the first time data for this Subsystem
		* has been requested, all the data is loaded from the database.
		*
		* @return The display name for this Subsystem as a std::string
		*
		* @exception DatabaseException A DatabaseException will be thrown if there is a
		*            problem establishing a connection with the database, or if an
		*            error is encountered while retrieving data.
		* @exception DataException A DataException will be thrown if the data cannot be 
		*            converted to the required format (e.g. ACCESSLEVEL should be an 
		*            unsigned long), or if the wrong ammount of data is retrieved.
		*/
		virtual std::string getDisplayName();	

        /** 
         * isPhysical
         *
         * This returns whether this is a physical subsystem or not.
         *
         * @return bool - True if this is a physical subsystem, false otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual bool isPhysical();

		/**
		  * See ISubsystem.h for a description of this method.
		  */
		virtual bool isExclusive();


		/**
		  * See ISubsystem.h for a description of this method.
		  */
		virtual bool isLocationExclusive();
        /**
         * getDateCreated
         *
         * Returns the date created for this subsystem.
         *
         * @return The date created for this subsystem as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */
        virtual time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this subsystem.
         *
         * @return The date modified for this subsystem as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */
        virtual time_t getDateModified();

        
        /**
         * invalidate
         *
         * Make the data contained by this Subsystem as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

    private:
		
        // Assignment operator not used so it is made private
		Subsystem& operator=(const Subsystem&);

        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the SubsystemHelper
        // has only been written for ConfigSubsystem objects and will not copy the Subsystem helper
        // for a read-only object (ie this one).
        Subsystem( const Subsystem& theSubsystem);



        SubsystemHelper* m_helper;
    };
} // closes TA_Base_Core

#endif // !defined(Subsystem_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
