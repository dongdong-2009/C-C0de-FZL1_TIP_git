/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RadioSessionEntityData.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * RadioSessionEntityData is a concrete implementation of IRadioSessionEntityData, which is in turn an implementation of
  * IEntityData. It proves read-only access to Radio session data, and a specific framework
  * for Radio session parameters.
  *
  */

#if !defined(Radio_Session_Entity_Data_INCLUDED_)
#define Radio_Session_Entity_Data_INCLUDED_

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IRadioSessionEntityData.h"

#if defined(_MSC_VER)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

namespace TA_Base_Core
{
    class RadioSessionEntityData : public IRadioSessionEntityData, public EntityData
    {

    public:
        /**
         * Radio (constructor)
         *
         * Constructs a new Radio object that contains only the static values. This
         * includes type, so the getType() method can be called without any further action.
         */
        RadioSessionEntityData() {};

        /**
         * Radio (constructor)
         *
         * Constructs a new Radio object with the specified key.
         *
         * @param key The key to this Console in the database.
         */
        RadioSessionEntityData(unsigned long key);

        virtual ~RadioSessionEntityData();

    public:

		//*****************
		// IRadio Methods *
		//*****************

        /**
         * getAssociatedConsole
         *
         * Returns the key of the associated console that will be using this radio session
		 * Some sessions, which do not use audio, do not have a console key in which case 
		 * 0 is returned.
         *
         * @return    The associated console key
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */ 
        virtual unsigned long getAssociatedConsoleKey();


        /**
         * getAudioLinkId
         *
         * Returns the radio audio link to be associated with this radio session. 0 indicates
		 * that no audio link is required.
         *
         * @return    The audio link
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */ 
        virtual unsigned long getAudioLinkId();

		/**
         * getSwitchUserWaitPeriod
         *
         * Returns the radio SwitchUserWaitPeriod to be associated with this radio session. 
         *
         * @return    The SwitchUserWaitPeriod
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */ 
		virtual unsigned long getSwitchUserWaitPeriod();


        /**
         * getAudioLinkServerId
         *
         * Returns the index of the server (CRS/SCN-TX) with which to associate
         * the radio audio link to be associated with this radio session.
		 * (Zero is a valid value).
         *
         * NOTE: also applies to the Occa/TrainComms session, which must be
         *       allocated a session on the radio server
         *
         * @return    The server index for the corresponding audio link
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */ 
        virtual unsigned long getAudioLinkServerId();

        /**
         * useForTrainCommunication
         *
         * Indicates if this session should be used for train communications.
         *
         * @return    True - can be used for train communications
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */ 
        virtual bool useForTrainCommunication();

        /**
         * getRadioUserName
         *
         * Returns the user name to be used for login into the radio system.
         *
         * @return    user name
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format or if the wrong amount of data is retrieved.
         */ 
        virtual std::string getRadioUserName();

        virtual std::string getConsoleComPort();

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
        RadioSessionEntityData( const RadioSessionEntityData& theRadioSession) {};            
		RadioSessionEntityData& operator=(const RadioSessionEntityData &) {};
        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string AUDIO_LINK;
        static const std::string AUDIO_LINK_SERVER;
        static const std::string USER_NAME;
        static const std::string TRAIN_COMMUMICATIONS;
        static const std::string CONSOLE_ID;
        static const std::string CONSOLE_COM_PORT;
		static const std::string SWITCH_USER_WAIT_PERIOD;
    };
    
    typedef boost::shared_ptr<RadioSessionEntityData> RadioSessionEntityDataPtr;
} //close namespace TA_Base_Core

#endif // !defined(Radio_Session_Entity_Data_INCLUDED_)
