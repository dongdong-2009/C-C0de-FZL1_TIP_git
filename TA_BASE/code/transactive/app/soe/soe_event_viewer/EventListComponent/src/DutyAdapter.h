/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/EventListComponent/src/DutyAdapter.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2009/12/03 15:46:08 $
  * Last modified by:  $Author: huirong.luo $
  *
  * This class provides an interface between the local duty agent and the DAI
  * responsible for obtaining events based on the provided filter.
  */
#if !defined(_DUTY_ADAPTER_H_)
#define _DUTY_ADAPTER_H_

#include "bus/security/duty_agent/IDL/src/IDutyAgentCorbaDef.h"
#include "core/data_access_interface/src/CombinedEventAccessFactory.h"

#include "core/naming/src/NamedObject.h"

class DutyAdapter
{
public:
    typedef std::vector< unsigned long > KeyList;
    typedef std::map< unsigned long, KeyList > DutyMap;

    /**
     * Constructor
     */
    DutyAdapter();


    /**
     * Destructor
     */
    virtual ~DutyAdapter();


    /**
     * getDutyMap
     *
     * Returns all regions and subsystems that the current user is able to view.
     * The result will be a subset of the filter information provided.
     *
     * @param locationKeys The list of locations the user wants to filter on (empty list means ALL).
     * @param physicalSubsystemKeys The list of subsystems the user wants to filter on (empty list means ALL).
     *                              NOTE: This list must be in ASCENDING order.
     *
     * @return DutyMap The locations/subsystems that the user can and wants to view.
     *
     * @throw DutyAgentException This is thrown when a problem was encountered talking to the duty agent.
     */
    DutyMap getDutyMap( const KeyList& locationKeys, const KeyList& physicalSubsystemKeys );

    /**
     * getFullDutyMap
     *
     * Returns all regions and subsystems that the current user is able to view.
     * The result will be a subset of the filter information provided.
     *
     * @param locationKeys The list of locations the user wants to filter on (empty list means ALL).
     * @param physicalSubsystemKeys The list of physical subsystems the user wants to filter on (empty list means ALL).
	 * @param applicationSubsystemKeys The list of application subsystems the user wants to filter on (empty list means ALL).
     *                              NOTE: This list must be in ASCENDING order.
     *
     * @return FullDutyMap The locations/subsystems/subsystemstates that the user can and wants to view.
     *
     * @throw DutyAgentException This is thrown when a problem was encountered talking to the duty agent.
     */
	TA_Base_Core::CombinedEventAccessFactory::FullDutyMap getFullDutyMap( const KeyList& locationKeys, const KeyList& physicalSubsystemKeys, const KeyList& applicationSubsystemKeys );

private:
    // Don't allow a singleton to be copied.
    DutyAdapter( const DutyAdapter& );
    DutyAdapter& operator=( const DutyAdapter& );

    /**
     * queryDutyAgent
     *
     * Retrieves the current duty from the local duty agent.
     *
     * @return DutyMap The current duty for the operator.
     */
    DutyMap queryDutyAgent();


    /**
     * createSubsystemList
     *
     * Creates a list of the subsystem keys that satisfy both the list provided by the user
     * and the current duty.
     *
     * @param physicalSubsystemKeys The subsystem keys specified by the user.
     * @param subsystemDuty The subsystem keys from the duty agent.
     *
     * @return KeyList The list of subsystem keys satisfying the filter and duty.
     */
    KeyList createSubsystemList( const KeyList& physicalSubsystemKeys, const KeyList& subsystemDuty );

    // The local duty agent.
    TA_Base_Core::NamedObject<TA_Base_Bus::IDutyAgentCorbaDef,TA_Base_Bus::IDutyAgentCorbaDef_ptr,TA_Base_Bus::IDutyAgentCorbaDef_var> m_dutyAgent;
};

#endif // !defined(_DUTY_ADAPTER_H_)