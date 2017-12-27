/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP/3001/transactive/bus/maintenance_management/mmsHelper/src/MMSSubmitManager.h $
* @author: huirong.luo
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2011/09/29 14:12:57 $
* Last modified by:  $Author: grace.koh $
* 
* This class provides implementation of IMMSSubmitter for C955 project
*  
*
*/

#if !defined(EA_3D8F72E4_4BA4_4295_9A5A_BFD71999E3DB__INCLUDED_)
#define EA_3D8F72E4_4BA4_4295_9A5A_BFD71999E3DB__INCLUDED_

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/naming/src/NamedObject.h"
#include "core/data_access_interface/src/IAlarmSeverityData.h"
#include "core/data_access_interface/src/IOperator.h" 

#include "IMMSSubmitter.h"

namespace TA_Base_Bus
{
	class RightsLibrary;
}

namespace TA_Base_Bus
{
	class MMSSubmitterDatabase : public IMMSSubmitter
	{

	public:
		MMSSubmitterDatabase();
		virtual ~MMSSubmitterDatabase();


		virtual bool submitAutoAlarmMessage(const TA_Base_Core::AlarmDetailCorbaDef& alarm);
		//virtual bool submitSEMIAlarmMessage(TA_Base_Core::AlarmDetailCorbaDef& alarm, std::string sessionID);
		virtual bool submitJobRequest(JobRequestDetailCorbaDef& jobRequest, std::string sessionID);

		std::string getOperatorDesc(const std::string& sessionID);

		virtual JobRequestDetailCorbaDef* getDataFromAlarm(std::string alarmID,unsigned long alarmLocationKey, std::string sessionID);
	 
		virtual JobRequestDetailCorbaDef* getDataFromSymbol(std::string entityName,  std::string sessionID);
		void getDescAndReportFields(const TA_Base_Core::AlarmDetailCorbaDef& alarm,
			std::string& alarmDescField, std::string& problemReportField);

		std::string createMmsAlarmDesc( 
			const time_t alarmOccurence,
			const std::string& alarmSeverity,
			const std::string& alarmDesc,
			const std::string& dataPointValue,
			bool truncate /*= true*/);

		std::string getAlarmSeverityName(const TA_Base_Core::AlarmDetailCorbaDef& p_alarm);
		std::string getAlarmSeverityNameInCache(unsigned long key);
		std::string toDataPointValue(const TA_Base_Core::AlarmDetailCorbaDef& p_alarm);
		std::string getOperatorName (unsigned long key);
		void loadOperators();

		std::map<unsigned long, TA_Base_Core::IOperator*>          m_operators;
		std::map<unsigned long, TA_Base_Core::IAlarmSeverityData*> m_alarmSeverities;

	private:

		// The object exposed by the MMS Agent for Job Request submission:
		TA_Base_Core::NamedObject<TA_Base_Bus::IMmsJobRequestSubmitterCorbaDef,
			TA_Base_Bus::IMmsJobRequestSubmitterCorbaDef_ptr,
			TA_Base_Bus::IMmsJobRequestSubmitterCorbaDef_var> m_jobRequestSubmmit;;

		bool m_needInitSubmitter;
		TA_Base_Core::ReEntrantThreadLockable m_lockSubmitter;


		bool initJobRequestSubmmit();

		/**
		* alarmStateIsOk
		*
		*
		* @return true if the alarm is in a state that allows a Job Request
		* to be submitted from it. If not populate the reason param with 
		* the reason why not. 
		*
		* All that is required is that the alarm exists, is not normalized. and a 
		* Job Request is not already raised on the alarm
		* 
		*
		* @throws DependencyFailure if the data can't be retrieved
		*/
		bool alarmStateIsOk(const TA_Base_Bus::JobRequestDetailCorbaDef& jobRequest, 
			std::string& reason, TA_Base_Bus::ESubmissionDeniedError& eType);

		bool isRaiseJobRequestPermitted(const std::string& sessionID, unsigned long dataNodeKey);

		TA_Base_Core::ReEntrantThreadLockable m_lockRightLibrary;
		TA_Base_Bus::RightsLibrary* m_rightsLibrary;
	};

}
#endif // !defined(EA_3D8F72E4_4BA4_4295_9A5A_BFD71999E3DB__INCLUDED_)
