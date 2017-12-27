/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/TrainSettingsEntityData.h $
  * @author Dirk McCormick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * TrainSettingsEntityData provides access to the Train Agent's type.
  * No other entity information is currently required for the Train Agent.
  */

#ifndef ENTITY_ACCESS_TRAIN_SETTINGS_ENTITY_DATA_H
#define ENTITY_ACCESS_TRAIN_SETTINGS_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class TrainSettingsEntityData : public EntityData
    {

    public:
        /**
         * TrainSettingsEntityData (constructor)
         *
         * Constructs a new TrainSettings object that contains only the static values. This
         */
        TrainSettingsEntityData();

        /**
         *
         * Constructs a new TrainSettings object with the specified key.
         *
         * @param key The key to this Train Agent in the database.
         */
        TrainSettingsEntityData(unsigned long key);


        virtual ~TrainSettingsEntityData();


        /**
         * Returns a pointer to a new object that has been initialised with the specified key. 
         * 
         * @param key    The key with which to construct the new object.
         */
        static IEntityData* clone(unsigned long key);

        /**
         *
         * Returns the type of this entity
         *
         * @return The type of this entity as a std::string
         */
        virtual std::string getType();


        /**
         * Returns the type of this entity 
         * 
         * @return The type of this entity as a std::string
         */
        static const std::string getStaticType();


        /**
         * Mark the data contained by this console as invalid. The next call to get...() following a call to invalidate() will cause all the data to be reloaded from the database.
         */
        virtual void invalidate();


        /**
         * Gets the number of retries to send to the train when it times out.
         * 
         * @return the number of times to resend the message
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimsRetryLimit();


        /**
         * Gets the number of times to automatically reply to a TIMS request for reset message.
         * This applies to PA live broadcasts and ongoing PEC conversations.
         * 
         * @return the number of times to automatically send a continue before prompting the operator
         * 
         * 
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimsAutoContinueAckLimit();


        /**
         * Gets the default timeout for any messages sent to TIMS that dont have an explicit timeout set.
         * 
         * @return the default timeout in milliseconds
         * 
         * 
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getDefaultTimsMessageTimeout();


        /**
         * Gets the CCTV1 timer value.
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimerCCTV1();

        /**
         * Gets the PEC2 timer value.
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimerPEC2();
	
		/**
         * Gets the PEC3 timer value.
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimerPEC3();

        /**
         * Gets the PEC4 timer value.
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimerPEC4();

		/**
         * Gets the PEC5 timer value.
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimerPEC5();

        /**
         * Gets the VC1 timer value.
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimerVC1();


        /**
         * Gets the VC2 timer value.
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimerVC2();


        /**
         * Gets the PA1 timer value.
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimerPA1();


        /**
         * Gets the PA2 timer value.
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimerPA2();


        /**
         * Gets the PA3 timer value.
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimerPA3();


        /**
         * Gets the PA4 timer value.
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimerPA4();


        /**
         * Gets the TIS11 timer value.
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimerTIS1();


        /**
         * Gets the TIS2 timer value.
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimerTIS2();


        /**
         * Gets the ISCS1 timer value.
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimerISCS1();


        /**
         * Gets the ISCS2 timer value.
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getTimerISCS2();


        /**
         * Gets the ATC timer value. This is the timer that is used to consider a train's ATC invalid.
         * By default it is set to 3x ATC period - which means a train must miss 3 consecutive ATC tables for it to be considered invalid
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getAtcTimeout();


        /**
         * Gets the OA1 timer value. This is the timer that is used to consider a train's OA1 invalid.
         * By default it is set to 3x OA1 period - which means a train must miss 3 consecutive OA1 tables for it to be considered invalid
         * 
         * @return the timeout in milliseconds
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getOa1Timeout();


        /**
         * This gets the total number of trains in the system. It is used to calculate the % downloaded/upgraded in the TTIS manager.
         * 
         * Note the system design limit is 99. The total number of trains can not exceed this number.
         * 
         * @return the number of configured trains (1-99).
         * 
         * 
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned char getTotalTrainNumber();


        /**
         * Gets the camera number that should be activated by this PEC
         * 
         * @return the camera number - suitable for sending to Train CCTV
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned char getCarOnePecOneCamera();


        /**
         * Gets the camera number that should be activated by this PEC
         * 
         * @return the camera number - suitable for sending to Train CCTV
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned char getCarOnePecTwoCamera();


        /**
         * Gets the camera number that should be activated by this PEC
         * 
         * @return the camera number - suitable for sending to Train CCTV
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned char getCarOnePecThreeCamera();


        /**
         * Gets the camera number that should be activated by this PEC
         * 
         * @return the camera number - suitable for sending to Train CCTV
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned char getCarOnePecFourCamera();


        /**
         * Gets the camera number that should be activated by this PEC
         * 
         * @return the camera number - suitable for sending to Train CCTV
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned char getCarTwoPecOneCamera();


        /**
         * Gets the camera number that should be activated by this PEC
         * 
         * @return the camera number - suitable for sending to Train CCTV
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned char getCarTwoPecTwoCamera();


        /**
         * Gets the camera number that should be activated by this PEC
         * 
         * @return the camera number - suitable for sending to Train CCTV
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned char getCarTwoPecThreeCamera();


        /**
         * Gets the camera number that should be activated by this PEC
         * 
         * @return the camera number - suitable for sending to Train CCTV
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned char getCarTwoPecFourCamera();


        /**
         * Gets the camera number that should be activated by this PEC
         * 
         * @return the camera number - suitable for sending to Train CCTV
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned char getCarThreePecOneCamera();


        /**
         * Gets the camera number that should be activated by this PEC
         * 
         * @return the camera number - suitable for sending to Train CCTV
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned char getCarThreePecTwoCamera();


        /**
         * Gets the camera number that should be activated by this PEC
         * 
         * @return the camera number - suitable for sending to Train CCTV
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned char getCarThreePecThreeCamera();


        /**
         * Gets the camera number that should be activated by this PEC
         * 
         * @return the camera number - suitable for sending to Train CCTV
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned char getCarThreePecFourCamera();


        /**
         * This will return the number of internal threads the agent can grow to.
         * The thread pool can dynamically grow to this limit as necessary.
         * 
         * @return the max threadpool size
         * 
         * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
         * @exception DataException A DataException will be thrown if there is a problem with the data
         */
        unsigned long getMaxThreadPoolSize();

		/**
		 * This will return a string of high priority depot used for state synchronization
		 *
		 *@return high priority sync - Depot
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
		 * @exception DataException A DataException will be thrown if there is a problem with the data
		 */
		std::string getStateSyncLocPriority();

		/**
		 * This will return a string of Train Status 12 protocol
		 *
		 *@return Train Status 12 protocol information
		 *
		 * @exception DatabaseException A DatabaseException will be thrown if there is a problem communicating with the database.
		 * @exception DataException A DataException will be thrown if there is a problem with the data
		 */
		std::string getTrainStatus12();

    private:

        TrainSettingsEntityData( const TrainSettingsEntityData& theRadioSession) {};            
		TrainSettingsEntityData& operator=(const TrainSettingsEntityData &) {return *this;};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
        static const std::string TimsMessageRetryLimit;


        static const std::string AutoContinueAckLimit;


        static const std::string DefaultTimsMessageTimeout;


        static const std::string TimerCctv1;


        static const std::string TimerVc1;


        static const std::string TimerVc2;


        static const std::string TimerPec2;


		static const std::string TimerPec3;


        static const std::string TimerPec4;


		static const std::string TimerPec5;


        static const std::string TimerPa1;


        static const std::string TimerPa2;


        static const std::string TimerPa3;


        static const std::string TimerPa4;


        static const std::string TimerTis1;


        static const std::string TimerTis2;


        static const std::string TimerIscs1;


        static const std::string TimerIscs2;


        static const std::string Oa1Timeout;


        static const std::string AtcTimeout;


        static const std::string TotalTrainNumber;


        static const std::string CarOnePecOneCamera;


        static const std::string CarOnePecTwoCamera;


        static const std::string CarOnePecThreeCamera;


        static const std::string CarOnePecFourCamera;


        static const std::string CarTwoPecOneCamera;


        static const std::string CarTwoPecTwoCamera;


        static const std::string CarTwoPecFourCamera;


        static const std::string CarTwoPecThreeCamera;


        static const std::string CarThreePecOneCamera;


        static const std::string CarThreePecTwoCamera;


        static const std::string CarThreePecThreeCamera;


        static const std::string CarThreePecFourCamera;


        static const std::string ThreadPoolSize;

		static const std::string StateSyncLocPriority;

		static const std::string TrainStatus12;

    };
    
    typedef boost::shared_ptr<TrainSettingsEntityData> TrainSettingsEntityDataPtr;
} // TA_Base_Core

#endif // ENTITY_ACCESS_TRAIN_SETTINGS_ENTITY_DATA_H

