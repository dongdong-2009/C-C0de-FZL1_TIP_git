/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RadioProfileEntityData.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * 
 * RadioProfileEntityData is an implementation of IEntityData. It proves read-only access to data,
 * and a specific framework for Radio Manager/MFT agent parameters.
 *
 */

#if !defined(Radio_Profile_Entity_Data_INCLUDED_)
#define Radio_Profile_Entity_Data_INCLUDED_

#include "core/data_access_interface/entity_access/src/EntityData.h"

#if defined(_MSC_VER)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

namespace TA_Base_Core
{
    class RadioProfileEntityData : public EntityData
    {

    public:
        /**
         * RadioProfileEntityData (constructor)
         *
         * Constructs a new RadioProfile object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        RadioProfileEntityData() {};

        /**
         * Radio (constructor)
         *
         * Constructs a new RadioProfile object with the specified key.
         *
         * @param key The key to this Radio Profile in the database.
         */
        RadioProfileEntityData(unsigned long key);

        virtual ~RadioProfileEntityData();

    public:

		//*****************
		// Radio Profile Specific Methods
		//*****************

        /**
         * getEventTimeout
         *
         * Returns the timeout at which the RadioMFTAgent handle will expire for each transaction with
         * the Console API
         *
         * @return    timeout value in seconds
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */ 
        unsigned long getEventTimeout();

        /**
         * getHistoryLimit
         *
         * Returns the number of data for the RadioHistoryPage,which provides a recent history of
		 * operator's activities within the radio system
         *
         * @return limit number
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */
        unsigned long getHistoryLimit();

        /**
         * getProfileButtons
         *
         * @return the number of profile-based ResourceButtons
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */
        unsigned short getProfileButtons();

        /**
         * getUserDefinedButtons
         *
         * @return the number of operator-based ResourceButtons
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */
        unsigned short getUserDefinedButtons();

        /**
         * getDialpadMaxNumber
         *
         * @return the maximum length of dialed number
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */
        unsigned short getDialpadMaxNumber();

        /**
         * getDialpadMaxHistory
         *
         * @return the maximum number of recently dialed number in Dialpad
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */
        unsigned short getDialpadMaxHistory();

        /**
         * getFlashButtonInterval
         *
         * @return the number in milliseconds on for blink interval of ResourceStatus buttons
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */
        unsigned short getFlashButtonInterval();

        /**
         * getBIMResourceID
         *
         * @return RID of the BIM resource
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */
		unsigned short getBIMResourceID();

        /**
         * getSPIResourceID
         *
         * @return RID of the SPI resource
         */
		unsigned short getSPIResourceID(); // TD16770

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
        RadioProfileEntityData( const RadioProfileEntityData& theRadioSession) {};            
		RadioProfileEntityData& operator=(const RadioProfileEntityData &) {};


        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;

	public:
		
        static const std::string CONSOLE_EVENT_TIMEOUT;
        static const std::string HISTORY_LIMIT;
		static const std::string PROFILE_BUTTONS;
		static const std::string USER_BUTTONS;
		static const std::string DIALPAD_MAXNUMBER_LENGTH;
		static const std::string DIALPAD_HISTORY_LENGTH;
		static const std::string FLASH_BUTTON_INTERVAL;
		static const std::string BIM_RESOURCE_ID;
		static const std::string SPI_RESOURCE_ID;

    };	
    
    typedef boost::shared_ptr<RadioProfileEntityData> RadioProfileEntityDataPtr;
} //close namespace TA_Base_Core

#endif // !defined(Radio_Profile_Entity_Data_INCLUDED_)
