#if !defined(XACMLFACTORYINTERNAL_H_INCLUDED)
#define XACMLFACTORYINTERNAL_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_agent/xacml_factory/src/XacmlFactoryInternal.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 *
 */

#include "XacmlFactory.h"

#include "ace/Synch.h"
#include <limits>

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "bus/security/authentication_agent/IDL/src/IAuthenticationAgentCorbaDef.h"
#include "core/naming/src/NamedObject.h"
#include "core/data_access_interface/src/Profile.h"
#include "core/data_access_interface/src/Action.h"
#include "core/data_access_interface/src/Resource.h"
#include "core/data_access_interface/src/Subsystem.h"
#include "core/data_access_interface/src/ActionGroup.h"
#include "core/data_access_interface/src/ActionGroupAccessFactory.h"
#include "core/data_access_interface/src/ActionAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessageConstants.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/types/DutyAgentBroadcastComms_MessageTypes.h"
#include "core/message/types/DutyAgentComms_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/DutyAgentEntityData.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/timers/src/AbstractThreadedTimeoutUser.h"

#include "bus/security/rights_agent/xacml_factory/src/LocSubSesProf.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
namespace TA_Base_Bus
{
	namespace RightsConst
	{
		const std::string RESOURCE_TYPE( "RESOURCE_TYPE" );
		const std::string PHYSICAL     ( "PHYSICAL"      );
		const std::string LOGICAL      ( "LOGICAL"       );
		const std::string RIGHTS       ( "RIGHTS"        );
		const std::string DUTY         ( "DUTY"          );
		const std::string DISPLAY_ONLY ( "DISPLAY_ONLY"  );
		const std::string TRUE_S		   ( "TRUE"          );
		const std::string FALSE_S        ( "FALSE"         );
		const std::string DISPLAY_ONLY_PHYSICAL("DISPLAY_ONLY_PHYSICAL");
		const std::string DISPLAY_ONLY_LOGICAL("DISPLAY_ONLY_LOGICAL");
	}

	namespace RightsType
	{
		typedef std::map< unsigned long, TA_Base_Core::IProfile* > ProfileMap;
		typedef std::map< unsigned long, TA_Base_Core::IAction* > ActionMap;
		// TODO: update IResource and use that instead.
		typedef std::map< unsigned long, TA_Base_Core::IEntityData* > ResourceMap;
		typedef std::map< unsigned long, TA_Base_Core::IActionGroup* > ActionGroupMap;
		typedef std::map< unsigned long, TA_Base_Core::ISubsystem* > SubsystemMap;

		typedef std::map< LocSubSesProf,unsigned long > LocSubDutyMap;
	}
}

#include "bus/security/rights_agent/xacml_factory/src/XacmlPolicyFactory.h"

#include "bus/security/rights_agent/xacml_factory/src/ContextOnlineUpdatable.h"

#include "bus/security/duty_agent/IDL/src/IDutyAgentQueryCorbaDef.h"
#include "bus/security/duty_agent/IDL/src/DutyAgentTypeCorbaDef.h"
#include "bus/security/rights_agent/xacml_factory/src/ContextSubDutyMsgSubscriber.h"
#include "bus/security/rights_agent/xacml_factory/src/XacmlContextFactory.h"
#include "bus/security/rights_agent/xacml_factory/src/RightsAgentInfo.h"

#endif // !defined(XACMLFACTORYINTERNAL_H_INCLUDED)
