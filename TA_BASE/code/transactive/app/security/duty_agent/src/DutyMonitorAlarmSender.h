/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyMonitorAlarmSender.h $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Encapsulates the sending of alarms related to duty monitoring.
  *
  */

#ifndef DUTYMONITORALARMSENDER_H_INCLUDED
#define DUTYMONITORALARMSENDER_H_INCLUDED

namespace TA_Base_App
{
	class DutyMonitorAlarmSender
	{
	public:
		DutyMonitorAlarmSender();

		void send( const TA_Base_Bus::DASType::DbKey& region );
		void send( const TA_Base_Bus::DASType::DbKey& subsystem, const TA_Base_Bus::DASType::DbKey& region );

	private:

		void send( const TA_Base_Core::MessageType& alarmType, TA_Base_Core::DescriptionParameters& description );

		std::string addRegion   ( TA_Base_Core::DescriptionParameters& description, const TA_Base_Bus::DASType::DbKey& region    );
		std::string addSubsystem( TA_Base_Core::DescriptionParameters& description, const TA_Base_Bus::DASType::DbKey& subsystem );

		static const std::string REGION_NAME;
		static const std::string SUBSYSTEM_NAME;

		//TD18093,jianghp
		//TA_Base_Core::NonUniqueAlarmHelper& m_alarmHelper;
	};
}
#endif // DUTYMONITORALARMSENDER_H_INCLUDED
