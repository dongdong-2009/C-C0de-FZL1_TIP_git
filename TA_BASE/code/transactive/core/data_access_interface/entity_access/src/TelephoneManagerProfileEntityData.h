/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/TelephoneManagerProfileEntityData.h $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * 
 * TelephoneManagerProfileEntityData is an implementation of IEntityData. It proves read-only access to data,
 * and a specific framework for Telephone Manager parameters.
 *
 */

#if !defined(Telephone_Manager_Profile_Entity_Data_INCLUDED_)
#define Telephone_Manager_Profile_Entity_Data_INCLUDED_

#include "core/data_access_interface/entity_access/src/EntityData.h"

#if defined(_MSC_VER)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

namespace TA_Base_Core
{
    class TelephoneManagerProfileEntityData : public EntityData
    {

    public:
        /**
         * TelephoneManagerProfileEntityData (constructor)
         *
         * Constructs a new RadioProfile object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        TelephoneManagerProfileEntityData() {};

        /**
         * Radio (constructor)
         *
         * Constructs a new RadioProfile object with the specified key.
         *
         * @param key The key to this Radio Profile in the database.
         */
        TelephoneManagerProfileEntityData(unsigned long key);

        virtual ~TelephoneManagerProfileEntityData();

    public:

		//*****************
		// Telephone Profile Specific Methods
		//*****************

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
         * getRadioPatchNumber
         *
         * @return the extension number to be used for patching DLTS-Radio
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */
        unsigned short getRadioPatchNumber();

        /**
         * getEPAXPatchNumber
         *
         * @return the extension number to be used for patching DLTS-EPAX
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */
        unsigned short getEPAXPatchNumber();

        /**
         * getMFTExtensionNumber
         *
         * @return the extension number configured for this MFT
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */
        unsigned short getMFTExtensionNumber();

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
        TelephoneManagerProfileEntityData( const TelephoneManagerProfileEntityData& theRadioSession) {};            
		TelephoneManagerProfileEntityData& operator=(const TelephoneManagerProfileEntityData &) {};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
	public:
		
        static const std::string HISTORY_LIMIT;
		static const std::string PROFILE_BUTTONS;
		static const std::string USER_BUTTONS;
		static const std::string DIALPAD_MAXNUMBER_LENGTH;
		static const std::string DIALPAD_HISTORY_LENGTH;
		static const std::string RADIO_PATCH_NUMBER;
		static const std::string EPAX_PATCH_NUMBER;
		static const std::string MFT_EXTNUMBER;

    };	
    
    typedef boost::shared_ptr<TelephoneManagerProfileEntityData> TelephoneManagerProfileEntityDataPtr;
} //close namespace TA_Base_Core

#endif // !defined(Telephone_Manager_Profile_Entity_Data_INCLUDED_)
