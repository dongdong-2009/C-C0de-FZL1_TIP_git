/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RadioDirectoryEntityData.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * 
 * RadioDirectoryEntityData is an implementation of IEntityData. It proves read-only access to Radio directory data,
 * and a specific framework for Radio directory parameters.
 *
 */

#if !defined(Radio_Directory_Entity_Data_INCLUDED_)
#define Radio_Directory_Entity_Data_INCLUDED_

#include "core/data_access_interface/entity_access/src/EntityData.h"

#if defined(_MSC_VER)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

namespace TA_Base_Core
{
    class RadioDirectoryEntityData : public EntityData
    {

    public:
        /**
         * RadioDirectoryEntityData (constructor)
         *
         * Constructs a new RadioDirectory object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        RadioDirectoryEntityData() {};

        /**
         * Radio (constructor)
         *
         * Constructs a new RadioDirectory object with the specified key.
         *
         * @param key The key to this Radio Directory in the database.
         */
        RadioDirectoryEntityData(unsigned long key);

        virtual ~RadioDirectoryEntityData();

    public:

		//*****************
		// Radio Directory Specific Methods
		//*****************

        /**
         * getSynchronisationFrequency
         *
         * Returns the frequency at which the radio system will attempt to synchronise subscriber data with the
         * subscriber system.
         *
         * @return    The associated console key
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */ 
        virtual unsigned long getSynchronisationFrequency();

        /**
         * getLastUpdateTime
         *
         * Returns the time at which the Radio Directory was last updated. Will return 0 if the parameter
         * has not been set (0 is "reload all")
         *
         * @return The time at which the RadioDirectory was last updated.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */
        virtual time_t getLastUpdateTime();

        /**
         * setLastUpdateTime
         *
         * Returns the time at which the Radio Directory was last updated. Will return 0 if the parameter
         * has not been set (0 is "reload all")
         *
         * @return The time at which the RadioDirectory was last updated.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */
        virtual void setLastUpdateTime(time_t updateTime);


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

    private:
        //
        // Copy constructor and assignment operator are not used
        //
        RadioDirectoryEntityData( const RadioDirectoryEntityData& theRadioSession) {};            
		RadioDirectoryEntityData& operator=(const RadioDirectoryEntityData &) {};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string SYNCHRONISATION_FREQUENCY;
        static const std::string LAST_UPDATE_TIME;
    };
    
    typedef boost::shared_ptr<RadioDirectoryEntityData> RadioDirectoryEntityDataPtr;
} //close namespace TA_Base_Core

#endif // !defined(Radio_Directory_Entity_Data_INCLUDED_)
