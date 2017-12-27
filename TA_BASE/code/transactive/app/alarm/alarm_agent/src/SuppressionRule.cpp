/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/SuppressionRule.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include "app/alarm/alarm_agent/src/SuppressionRule.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"

namespace TA_Base_App
{

    //
    // Constructor
    //
    SuppressionRule::SuppressionRule(unsigned long entityKey, unsigned long entityType, unsigned long alarmTypeKey,
		TA_Base_App::AbstractRule::ETriggerType trigger, unsigned long key, const std::string& alarmDescription )
    : AbstractRule(entityKey, entityType, alarmTypeKey, trigger, key)
    {
		m_description = alarmDescription;
	}


    //
    // Destructor
    //
    SuppressionRule::~SuppressionRule()
    {}


    //
    // process
    //
    void SuppressionRule::process( const TA_Base_Core::AlarmDetailCorbaDef& message,
                                   TA_Base_App::AbstractRule::ETriggerType trigger)
    {}


    //
    // alarmShouldBeSuppressed
    //
    bool SuppressionRule::alarmShouldBeSuppressed( const TA_Base_Core::AlarmDetailCorbaDef& message )
    {
		bool suppress = false;
		// Suppression rule is always trigger on SUBMIT
        if ( doesRuleApply(message, TA_Base_App::AbstractRule::SUBMIT) )
		{
			// Check whether the description is set
			// Do a compare if it is
			if ( !m_description.empty() )
			{
				std::string messageDescription( message.alarmDescription.in() );
				if ( !messageDescription.empty() && 
					m_description.compare(messageDescription) == 0 )
				{
					suppress = true;
				}
			}
			else // Description is not a suppression param
			{
				suppress = true;
			}
		}

		return suppress;
    }
};
