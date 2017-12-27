/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/DutyManager.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages all duty related tasks such as popping out the 
  * duty request and notification dialogs
  */


#if !defined(DutyManager_B0D5AD53_1126_418b_90BE_D22186AE8DCE__INCLUDED_)
#define DutyManager_B0D5AD53_1126_418b_90BE_D22186AE8DCE__INCLUDED_

#include "app/system_control/control_station/src/ControlStationConstants.h"
#include <string>
#include <vector>
#include "bus\security\duty_agent\IDL\src\IDutyAgentCorbaDef.h"
#include "bus/security/duty_agent/IDL/src/IDutyAgentQueryCorbaDef.h"
#include "core\message\types\DutyAgentBroadcastComms_MessageTypes.h"
#include "core\message\src\SpecialisedMessageSubscriber.h"
#include "core\message\IDL\src\CommsMessageCorbaDef.h"
#include "core/naming/src/NamedObject.h"

class DutyManager : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
{
public:

    struct SubsystemDetail
    {
        std::string regionName;
        std::vector<std::string> subsystemName;
    };

    struct DutyDetail
    {
        std::string operatorName;
        std::string profileName;
        std::string locationName;
        std::vector<SubsystemDetail> subsystem;
    };

    struct DutyNotificationDetail
    {
        std::vector<DutyDetail> gained;
        std::vector<DutyDetail> denied;
        std::vector<DutyDetail> lost;
		std::vector<DutyDetail> changed;
    };

    struct DutyRequestDetail
    {
        DutyDetail duty;
        std::string uniqueId;
    };

	typedef std::vector<DutyDetail> DutyDetailList;
	typedef std::vector<DutyDetail>::iterator DutyDetailIter;
	typedef std::vector<SubsystemDetail>::iterator SubsystemDetailIter;

    /** 
      * DutyManager
      *
      * Constructs the duty manager.
      *
      */
    DutyManager();


    /** 
      * ~DutyManager
      *
      * Unsubscribes to the messaging system
      *
      */
    virtual ~DutyManager();
    

	/**
	* receiveSpecialisedMessage
	*
	* This is overriden by the client to receive their specialise message
	* 
	* @param	T	The narrowed Message
	*/
    virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);

    void respond(const std::string& uniqueId, const TA_Base_App::TA_ControlStation::EDutyResponse response);

    /**
     * loseExclusiveDuty
     *
     * Called when the current session is being logged out. Determines if the current session holds any exclusive
     * duties, and warns the user that those sub-systems will be dutiless once they have logged out.
     */
    bool loseExclusiveDuty();

private:

	void getDutyChanged(DutyNotificationDetail* det); 	// TD19075

    /** 
      * processDutyChange
      *
      * Process the Duty Change notification message
      *
      * @param message The duty comms message 
      */
    void processDutyChange(const TA_Base_Core::CommsMessageCorbaDef& message);

    /** 
      * processDutyRequest
      *
      * Process the Duty Request notification message
      *
      * @param message The duty comms message 
      */
    void processDutyRequest(const TA_Base_Core::CommsMessageCorbaDef& message);


    /** 
      * repackageDutyData
      *
      * Repackage the duty tree data into a structure that we can display easily.
      *
      * @param source The original data in the form of DutyTreeSequence
      * @param destination The resultant data.
      *
      */
    void repackageDutyData(TA_Base_Bus::DutyAgentTypeCorbaDef::DutyTreeSequence& source, std::vector<DutyDetail>& destination);


    // guards the callback map
    TA_Base_Core::NonReEntrantThreadLockable m_callbackLock;

    // callback map which maps the unique Id to duty agent object that will
    // be handling the request with such Id
    typedef std::map<std::string, TA_Base_Bus::IDutyAgentRespCorbaDef_ptr> CallbackMap;
    CallbackMap m_callback;

    // Define the named object for contact to the duty agent
    TA_Base_Core::NamedObject<TA_Base_Bus::IDutyAgentQueryCorbaDef,
                         TA_Base_Bus::IDutyAgentQueryCorbaDef_ptr,
                         TA_Base_Bus::IDutyAgentQueryCorbaDef_var> m_dutyAgentQuery;
};

#endif // !defined(DutyManager_B0D5AD53_1126_418b_90BE_D22186AE8DCE__INCLUDED_)
