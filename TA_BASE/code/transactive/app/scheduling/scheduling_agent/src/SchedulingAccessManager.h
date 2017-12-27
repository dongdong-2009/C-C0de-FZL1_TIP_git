/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingAccessManager.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  *
  * This class is used to manage access to the Scheduling Agent's
  * CORBA interface.
  *
  */


#ifndef SCHEDULING_ACCESS_MANAGER_H
#define SCHEDULING_ACCESS_MANAGER_H

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "bus/security/rights_library/src/RightsLibrary.h"

#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"

#include <string>

namespace TA_Base_App
{



class SchedulingAccessManager
{

public:


	/**
	 *  Gets the single instance of this class.
     *  Note that the first time this method is called, initialise must be
     *  called.
	 */
	static SchedulingAccessManager& getInstance();


    /**
     *  Initialises the object.
     *
     *  This method is used instead of a constructor.
     *  It must be called the first time getInstance is invoked, or
     *  subsequent invocations will cause an assertion.
     *
     *  @param entityKey  The entity key for the scheduling agent
     *
     *  @DatabaseException
     *  if there is a problem communicating with the database
     */
    void initialise(unsigned long entityKey);


    /**
     *  Checks if the given session has access to the Scheduling Agent
     *  for the given action.
     *
     *  @exception AccessDeniedException
     *  Thrown if the given session does not have permission to perform the
     *  given action.
     */
    void checkIfActionIsPermitted(
        const std::string& sessionID, unsigned long actionID);


    /**
     *  Destructor.
     */
    ~SchedulingAccessManager();



private:

	SchedulingAccessManager();
    SchedulingAccessManager(const SchedulingAccessManager&);
    const SchedulingAccessManager& operator=(const SchedulingAccessManager&);


    // The Rights Library from which sessions are retrieved
    TA_Base_Bus::RightsLibrary* m_rightsLibrary;

    // The Scheduling Agent's resource ID
    unsigned long m_resourceID;

    
    // The single instance of this class
    static SchedulingAccessManager* m_instance;

    // The lock used to stop multiple access to the instance of this class
    static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
};



}; // TA_Base_App


#endif // SCHEDULING_ACCESS_MANAGER_H
