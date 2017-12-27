#ifndef RIGHTSLIBRARY_H_INCLUDED
#define RIGHTSLIBRARY_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_library/src/RightsLibrary.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2015/01/21 10:20:13 $
  *  Last modified by:  $Author: liwei.gong $
  *
  *  Declaration of RightsLibrary and RightsLibraryFactory. RightsLibraryFactory
  *  creates RightsLibrary objects. RightsLibrary objects are used to a query
  *  rights.
  */

// System includes
#include <string>
#include <list>
#include <map>
#include <set>


#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/RightsException.h"

// These includes are for users of the rights library. They
// need the Resource access headers to pass in the resource key.
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"

namespace TA_Base_Bus
{
	/**
	  * Rights Library type identifiers. Passed into the RightsLibraryFactory
	  * to specify the type of Rights Library to build.
	  *
	  * ALWAYS_TRUE_RL - Build a rights library that always returns true.
	  *
	  * DEFAULT_RL     - Use the default (project specific) Rights Library
	  *                  type. For C830 this is the CORBA Rights Library.
	  */
	enum ERightsLibraryType
	{
		ALWAYS_TRUE_RL,
		DEFAULT_RL    ,
		UNDEFINED_RL
	};

	/**
	  * Decision Module identifiers.
	  *
	  * RIGHTS_DM - The decision to deny access was made by the
	  *               Rights decision module.
	  * MUTEX_DM  - The decision to deny access was made by the
	  *               mutual exclusion module.
	  */
	enum EDecisionModule
	{
		RIGHTS_DM,
		MUTEX_DM ,
		UNDEFINED_DM
	};

	enum ERightsChangeType
	{
		RC_SessionBegin,
		RC_SessionEnd,	
		RC_SessionOverrideBegin,
		RC_SessionOverrideEnd,
		RC_DutyUpdate,
		RC_DutyRecovery
	};
	
	typedef std::map<unsigned long, std::list<unsigned long> > DutyChanges; //location-->subsystems, when DutyRecovery, only locations are used.
	typedef std::map<std::string, DutyChanges> SessionDutyChanges;

	typedef std::map<unsigned long, std::set<unsigned long> > DutyChangesSet; //location-->subsystems, when DutyRecovery, only locations are used.
	typedef std::map<std::string, DutyChangesSet> SessionDutyChangesMap;
	
	class IRightsChangeObserver
	{
	public:
		virtual void OnRightsChange(std::string sessionId, ERightsChangeType changeType, DutyChanges& changes )=0;
		virtual ~IRightsChangeObserver(){};
	};
	
	typedef std::list<unsigned long> ActionList;
	typedef std::list<unsigned long> ResourceList;
	/**
	  * RightsLibrary
	  * 
	  * This abstract interface can be instantiated. It will
	  * always return true.
	  */
	class RightsLibrary
	{
	public:

		/**
		  * Constructor
		  *
		  * Constructs a RightsLibrary object.
		  * 
		  * @exceptions RightsException, If either insufficient attributes
		  *                              were found by the rights processor or
		  *                              a policy applying to the request did not
		  *                              exits the exception string will contain
		  *                              Indeterminate OR Not Applicable respectively.
		  *                              Otherwise the sting will contian the message
		  *                              retrived from any third-party exceptions
		  *                              caught by this method.
		  */
		RightsLibrary(){}
		virtual ~RightsLibrary(){}

		/**
		  * isActionPermittedOnResource
		  *
		  * Makes a decision as th wheter a *subject* can perform
		  * an *action* on a *resource*. 
		  * This library does not catch any exceptions.
		  *
		  * @return bool, true == permit, false == deny.
		  *
		  * @param 	sessionId,      used to look up profiles etc.
		  * 		resourceKey,    only resources with an associated key
		  * 		                are access controlled.
		  * 		actionKey,      only actions with an associated key
		  * 		                are access controlled.
		  * 		reasonString,   only populated when a request is denied.
		  * 		decisionModule, some clients of the library may want to
		  * 		                behave differently depending on why the
		  * 		                deny decision was made.
		  * 
		  * @exceptions RightsException, If either insufficient attributes
		  *                              were found by the rights processor or
		  *                              a policy applying to the request did not
		  *                              exits the exception string will contain
		  *                              Indeterminate OR Not Applicable respectively.
		  *                              Otherwise the sting will contian the message
		  *                              retrived from any third-party exceptions
		  *                              caught by this method.
		  */
		virtual bool isActionPermittedOnResource(
					const std::string& sessionId, unsigned long resourceKey,
					unsigned long actionKey, std::string& reasonString,
					EDecisionModule& decisionModule ){ return( true ); }

		virtual bool isActionPermittedOnResourceAndLocSub(
					const std::string& sessionId, unsigned long resourceKey, unsigned long locationKey, unsigned long subsystemKey,
					unsigned long actionKey, std::string& reasonString,
					EDecisionModule& decisionModule ){ return( true ); }

		virtual void isActionPermittedOnResources(
			const std::string& sessionId, ResourceList& resourceKeys,ResourceList& returnResourceList,
			unsigned long actionKey, std::string& reasonString,
					EDecisionModule& decisionModule )
		{ 
			ResourceList::iterator it=resourceKeys.begin();
			for(;it!=resourceKeys.end();it++)
			{
				returnResourceList.push_back(*it);
			}
		}

		virtual void  areActionsPermittedOnSubsystemAndLocSub(
			const std::string& sessionId, unsigned long resourceKey, unsigned long locationKey, unsigned long subsystemKey,
			ActionList & actionKeys,ActionList & returnActionList ,std::string& reasonString,
			EDecisionModule& decisionModule )
		{ 	
			ActionList::iterator it= actionKeys.begin();
			for(;it!= actionKeys.end();it++)
			{
				returnActionList.push_back(*it);

			}
		}
		virtual bool isActionPermittedOnResourceAndLocSubWithoutDuty(
					const std::string& sessionId, unsigned long resourceKey, unsigned long locationKey, unsigned long subsystemKey,
					unsigned long actionKey, std::string& reasonString,
					EDecisionModule& decisionModule ){ return( true ); }


		virtual bool isActionPermittedOnResourceWithoutDuty(
					const std::string& sessionId, unsigned long resourceKey,
					unsigned long actionKey, std::string& reasonString,
					EDecisionModule& decisionModule ){ return( true ); }
	
		/**
		  * resetCache
		  *
		  * If results are cached, this can be used to force the cache
		  * to be reset. This may be required where the user of the library
		  * knows before the library does that rights have changed. The
		  * Duty Manager is the only known application at the time of writing.
		  *
		  */
		virtual void resetCache(){};

	
		virtual void registerObserver(IRightsChangeObserver& observer){};
		virtual void unregisterObserver(IRightsChangeObserver& observer){};

	private:
		RightsLibrary( const RightsLibrary& );
		RightsLibrary& operator=( const RightsLibrary& );
	};


	/**
	  * RightsLibraryFactory
	  *
	  * Constructs Rights Libraries. This could become a Singleton once a project
	  * wide method for properly instatiating and destroying them becomes available.
	  *
	  */
	class RightsLibraryFactory
	{
	public:

        enum EOwner { DEFAULT, MANAGER, CONTROL_STATION };

		RightsLibraryFactory();
		~RightsLibraryFactory(){}

		/**
		  * buildRightsLibrary
		  *
		  *
		  * @return RightsLibrary*, The Rights Library that was built. The caller
		  *                         is responsible for deleting this object.
		  *
		  * @param 	ERightsLibraryType, Selects a library type to build. If left 
		  *                             blank the defualt type will be built.
		  * 
		  */
		RightsLibrary* buildRightsLibrary( ERightsLibraryType rlType = DEFAULT_RL, EOwner owner = DEFAULT ) const;

	private:
		RightsLibraryFactory( const RightsLibraryFactory& );
		RightsLibraryFactory& operator=( const RightsLibraryFactory& );
	};
}

#endif // RIGHTSLIBRARY_H_INCLUDED
