#if !defined(AlarmModificationHelper_9D219F88_2F5D_412e_9B29_DCFF1A374D2F__INCLUDED_)
#define AlarmModificationHelper_9D219F88_2F5D_412e_9B29_DCFF1A374D2F__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/alarm/src/AlarmModificationHelper.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/20 22:25:27 $
  * Last modified by:  $Author: huirong.luo $
  * 
  * This class manages closing, acknowledging, setting comments etc on alarms. It is used
  * by the alarm list control. It is not a class for general use by all apps.
  */

#include <vector>
#include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"
//#include "core/alarm/src/AlarmItem.h"
#include "core/alarm/src/AlarmModifyItem.h"

namespace TA_Base_Core
{
    struct AlarmDetailCorbaDef;
    class AlarmHelperBase;

    class AlarmModificationHelper
    {

    public:
        /**
         * constructor
         *
         * @param   alarmHelperBase     the alarmHelperBase to use
         */
        AlarmModificationHelper(AlarmHelperBase& alarmHelperBase);


        /**
         * destructor
         */
        virtual ~AlarmModificationHelper();


        /**
         * acknowledgeAlarms
         *
         * This will acknowledge all the alarms passed in.
         *
         * @param vector<AlarmDetailCorbaDef*>& - A vector of all the alarms to be acknowledged
         *
         * @exception NoAlarmAgentException - This will be thrown if any error occurs while trying to
         *                                    acknowledge alarms. As many alarms as possible will be
         *                                    acknowledged before this exception is thrown.
         */
        void acknowledgeAlarms(std::vector<AlarmDetailCorbaDef*>& alarms, const char * sessionId = NULL);   // Raymond Pau++ TD13367 
        void operatorAcknowledgeAlarms(std::vector<AlarmDetailCorbaDef*>& alarms, const char * sessionId = NULL, bool isOperatorTriggered = true );

		bool operatorAcknowledgeAlarmsEx(std::vector<AlarmModifyItem*>& alarms, std::vector<std::string>&  alarmNoExists, const char * sessionId = NULL, bool isOperatorTriggered = true );


        /**
         * closeAndAcknowledgeAlarms
         *
         * This will acknowledge (if necessary) and close all the alarms passed in.
         *
         * @param vector<AlarmDetailCorbaDef*>& - A vector of all the alarms to be acknowledged and closed
         *
         * @exception NoAlarmAgentException - This will be thrown if any error occurs while trying to
         *                                    acknowledge or close alarms. As many alarms as possible will be
         *                                    acknowledged and closed before this exception is thrown.
         */
        void operatorCloseAndAcknowledgeAlarms(std::vector<AlarmDetailCorbaDef*>& alarms, const char * sessionId = NULL);   // Raymond Pau++ TD13367 

		bool operatorCloseAndAcknowledgeAlarmsEx(std::vector<AlarmModifyItem*>& alarms, std::vector<std::string>&  alarmNoExists, const char * sessionId = NULL);   // Raymond Pau++ TD13367 


        /**
         * updateComments
         *
         * This will update the comments on all the alarms passed in
         *
         * @param vector<AlarmDetailCorbaDef*>& - A vector of all the alarms to have their comment updated
         * @param string - The comment to add to the alarms.
         *
         * @exception NoAlarmAgentException - This will be thrown if any error occurs while trying to
         *                                    update the comment on any alarms. As many alarms as possible
         *                                    will be updated before this exception is thrown.
         * @return bool - Return all failure that occur for one or more alarms
         */
        bool updateComments(std::vector<AlarmDetailCorbaDef*>& alarms, const std::string& comment);

		bool updateComments(std::vector<AlarmModifyItem*>& alarms, const std::string& comment);

        /**
         * setAssociatedPlanWasRun
         *
         * This will update the plan to indicate that a plan has been run for it
         *
         * @param AlarmDetailCorbaDef* - The alarm to update
         *
         * @exception NoAlarmAgentException - This will be thrown if any error occurs while trying to
         *                                    update the plan status of an alarm.
         */
        void setAssociatedPlanWasRun(AlarmDetailCorbaDef* alarm);

		void setAssociatedPlanWasRun(AlarmModifyItem* alarm);

		/**
		* setJobRequestSubmitted
		*
		* set the MMS state of the alarm to reflect that a Job Request has been
		* submitted via that alarm.
		*
		* @param alarmID: The UID of the alarm, PRECONDITION: alarmID must not be empty
		* @param locationKey: The key of the location where the alarm was raised
		*
        * @exception NoAlarmAgentException raised if the submission fails for any reason,
		* as example, the specified alarm does not exist or if the alarm is already in the specified state.
		*/
		void setJobRequestSubmitted(const std::string& alarmID,unsigned long locationKey);

		/**
        * setJobRequestSubmitted
        * This function is triggered by submitting MMS request by an operator manually.
        * set the MMS state of the alarm to reflect that a Job Request has been
        * submitted via that alarm, and acknowledge the alarm.
        *
        * @param alarmID: The UID of the alarm, PRECONDITION: alarmID must not be empty
        * @param locationKey: The key of the location where the alarm was raised
        * @param sessionID: The session ID of the operator who requires to submit MMS request manually.
		* @param bNeedAckAlarm: Set to true if alarm needs to be acknowledge after submitting MSS request.

        * @exception NoAlarmAgentException raised if the submission fails for any reason,
                * as example, the specified alarm does not exist or if the alarm is already in the specified state.
		*/
		void setJobRequestSubmitted(const std::string& alarmID, unsigned long locationKey, const char * sessionId, bool bNeedAckAlarm=true);

		/**
		* setAlarmMmmsState
		*
		* set the MMS state of the alarm to reflect the change in Mms State Type
		*
		* @param alarmID: The UID of the alarm, PRECONDITION: alarmID must not be empty
		* @param locationKey: The key of the location where the alarm was raised
		* @param MMS_STATE_TYPE: The Mms state type setting (MMS_NONE, MMS_SEMI, MMS_AUTO, MS_JR_SENT, MMS_ALM_SENT, MMS_INHIBIT, MMS_FAILED
		*
        * @exception NoAlarmAgentException raised if the submission fails for any reason,
		* as example, the specified alarm does not exist or if the alarm is already in the specified state.
		*/
		void setAlarmMmsState(const std::string& alarmID,unsigned long locationKey, MmsStateType mmsState);

        /**
         * postponePlanExecution
         *
         * This will indicate the user has postponed the running of a plan for the
         * specified alarm.
         *
         * @param AlarmDetailCorbaDef* - The alarm to update
         *
         * @exception NoAlarmAgentException - This will be thrown if any error occurs while trying to
         *                                    update the plan status of an alarm.
         */
        void postponePlanExecution(AlarmDetailCorbaDef* alarm);

		void postponePlanExecution(AlarmModifyItem* alarm);

    private:
        /**
         * disable default constructor
         */
        AlarmModificationHelper();

        /**
         * disable default copy constructor
         */
        AlarmModificationHelper( const AlarmModificationHelper& theAlarmHelper);


        /**
         * acknowledgeAlarmsForLocation
         *
         * This will acknowledge all the alarms for the specified location and then remove them from the
         * vector
         *
         * @param vector<AlarmDetailCorbaDef>& - This will be returned with all alarms that have been processed
         *                                       removed.
         * @param unsigned long - The location key of the alarms to acknowledge
         * 
         * @return bool - Indicates if the acknowledge was successful. false = unsuccessful. As many alarms
         *                as possible will be acked before false is returned.
         */
        bool acknowledgeAlarmsForLocation(std::vector<AlarmDetailCorbaDef*>& alarms, unsigned long locationKey, const char * sessionId = NULL, bool isOperatorTriggered = false ); // Raymond Pau++ TD13367
		
		bool acknowledgeAlarmsForLocation(std::vector<AlarmModifyItem*>& alarms, std::vector<std::string>& alarmsNoExist, unsigned long locationKey, const char * sessionId = NULL, bool isOperatorTriggered = false ); // Raymond Pau++ TD13367

        /**
         * acknowledgeAndCloseAlarmsForLocation
         *
         * This will acknowledge and close all the alarms for the specified location and then remove them
         * from the vector
         *
         * @param vector<AlarmDetailCorbaDef>& - This will be returned with all alarms that have been processed
         *                                       removed.
         * @param unsigned long - The location key of the alarms to acknowledge and close
         * 
         * @return bool - Indicates if the acknowledge and close was successful. false = unsuccessful. As 
         *                many alarms as possible will be acked and closed before false is returned.
         */
        bool acknowledgeAndCloseAlarmsForLocation(std::vector<AlarmDetailCorbaDef*>& alarms, unsigned long locationKey, const char * sessionId = NULL); // Raymond Pau++ TD13367

		bool acknowledgeAndCloseAlarmsForLocation(std::vector<AlarmModifyItem*>& alarms, std::vector<std::string>& alarmNoExists, unsigned long locationKey, const char * sessionId = NULL); // Raymond Pau++ TD13367


        /**
         * updateCommentsForLocation
         *
         * This will update the comments for all the alarms for the specified location and then remove them
         * from the vector
         *
         * @param vector<AlarmDetailCorbaDef>& - This will be returned with all alarms that have been processed
         *                                       removed.
         * @param unsigned long - The location key of the alarms to update
         * 
         * @return bool - Indicates if the comment update was successful. false = unsuccessful. As 
         *                many alarms as possible will be updated before false is returned.
         */
        bool updateCommentsForLocation(std::vector<AlarmDetailCorbaDef*>& alarms,
                                       unsigned long locationKey,
                                       const std::string& comment);

		bool updateCommentsForLocation(std::vector<AlarmModifyItem*>& alarms,
                                       unsigned long locationKey,
                                       const std::string& comment);


		/**
         * calculateCorbaTimeout
         *
         * This will calculate the corbatimeout based on the number of alarms
         *
         * @param unsigned int ulNumAlarms - The number of alarms to be processed
		 *
         *
         * @return unsigned int - returns the number of seconds it will take to process the number of alarms
		 *						taking into consideration processing time
         */
		unsigned short calculateCorbaTimeout(unsigned int ulNumAlarms);
		
        /**
         * store a reference to the AlarmHelperBase class
         */
        AlarmHelperBase& m_alarmHelperBase;

		//TD14558
		//marvin++
		/**
         * The number of alarms to acknowledge per batch
		 *
         */
		static const unsigned int ACKNOWLEDGE_IN_BATCH_COUNT;
		//TD14558
		//++marvin
    };


}; // namespace TA_Base_Core

#endif // !defined(AlarmModificationHelper_9D219F88_2F5D_412e_9B29_DCFF1A374D2F__INCLUDED_)
