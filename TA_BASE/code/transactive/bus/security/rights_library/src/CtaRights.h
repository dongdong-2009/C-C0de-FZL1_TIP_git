#if !defined(CtaRights_210A68F5_3B6E_4a91_8577_9EAA4840660D__INCLUDED_)
#define CtaRights_210A68F5_3B6E_4a91_8577_9EAA4840660D__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_library/src/CtaRights.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This interface is used to determine if a particular session is allowed to perform an action
  * on a resource.
  */

#include <memory>

#include "bus/security/rights_library/public_interfaces/ItaRights.h"

namespace TA_Base_Bus
{
    class RightsLibrary; // Forward declaration

    class CtaRights : public virtual ItaRights 
    {

    public:
        /**
         * Constructor
         */
	    CtaRights();


        /**
         * Destructor
         */
	    virtual ~CtaRights();

	    
        /**
         * isActionPermittedOnResource
         *
	     * Makes a decision on whether a given subject is allowed to perform a given
	     * action on a given resource.
         *
	     * @param session    Session ID. The "subject" of the request. Identifies the user
	     *                   for whom the request is being made.
	     * @param resource    The entity  on which the subject wishes to perform the
	     *                    action.
	     * @param action    The action (a.k.a function) the subject wishes to perform.
	     *                  Taken from an enumerated list of access controlled actions that matches the
	     *                  list in the database.
	     * @param reason    If a request is denied (i.e. the return value of false) then
	     *                  this string contains the reason. Otherwise it is empty.
	     * 
	     * @throws RightsException
	     */
	    virtual bool isActionPermittedOnResource(const std::string& session,
                                                 unsigned long resource, 
                                                 unsigned long action, 
                                                 std::string& reason);


        /**
         * isActionPermittedOnResourceAndLocSub
         *
	     * This is the same as isActionPermittedOnResource, except it allows to specify the
         * location and subsystem keys for effciency, the client may already have them loaded from
         * the database, in that case there is no need to load them internally.
         *
	     * @param session    Session ID. The "subject" of the request. Identifies the user
	     *                   for whom the request is being made.
	     * @param resource    The entity  on which the subject wishes to perform the
	     *                    action.
         * @param location    The location of the resource
         * @param subsystem    The subsystem of the resource
	     * @param action    The action (a.k.a function) the subject wishes to perform.
	     *                  Taken from an enumerated list of access controlled actions that matches the
	     *                  list in the database.
	     * @param reason    If a request is denied (i.e. the return value of false) then
	     *                  this string contains the reason. Otherwise it is empty.
	     * 
	     * @throws RightsException
	     */
	    virtual bool isActionPermittedOnResourceAndLocSub( const std::string& session,
                                                           unsigned long resource,
                                                           unsigned long location,
                                                           unsigned long subsystem,
                                                           unsigned long action,
                                                           std::string& reason );

        /**
         * isActionPermittedOnResourceAndLocSubWithoutDuty
         *
	     * This is the same as isActionPermittedOnResource, except it allows to specify the
         * location and subsystem keys for effciency, the client may already have them loaded from
         * the database, in that case there is no need to load them internally.
         *
	     * @param session    Session ID. The "subject" of the request. Identifies the user
	     *                   for whom the request is being made.
	     * @param resource    The entity  on which the subject wishes to perform the
	     *                    action.
         * @param location    The location of the resource
         * @param subsystem    The subsystem of the resource
	     * @param action    The action (a.k.a function) the subject wishes to perform.
	     *                  Taken from an enumerated list of access controlled actions that matches the
	     *                  list in the database.
	     * @param reason    If a request is denied (i.e. the return value of false) then
	     *                  this string contains the reason. Otherwise it is empty.
	     * 
	     * @throws RightsException
	     */														   
		virtual bool isActionPermittedOnResourceAndLocSubWithoutDuty( const std::string& session,
                                                           unsigned long resource,
                                                           unsigned long location,
                                                           unsigned long subsystem,
                                                           unsigned long action,
                                                           std::string& reason );														   
                             
        
    private:

        /**
         * Disable copy Constructor.
         */
        CtaRights(const CtaRights& rhs);


        /**
         * Disable assignment operator
         */
        const CtaRights& operator=(const CtaRights& rhs);
        
        // The library that will be use to actually determine the rights
        std::auto_ptr<RightsLibrary> m_rightsLibrary;

    };
}
#endif // !defined(CtaRights_210A68F5_3B6E_4a91_8577_9EAA4840660D__INCLUDED_)
