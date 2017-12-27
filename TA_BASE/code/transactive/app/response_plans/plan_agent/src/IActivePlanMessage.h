#ifndef IACTIVEPLANMESSAGE_H_INCLUDED
#define IACTIVEPLANMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
  *
  *  <description>
  */

namespace TA_Base_App
{
    class ActivePlan;

	//Class declaration
	class IActivePlanMessage
	{
	// Public methods
	public:
		IActivePlanMessage(){}
		virtual ~IActivePlanMessage(){}

		virtual void execute( ActivePlan& activePlan ) const = 0;
	
	private:
		IActivePlanMessage& operator=( const IActivePlanMessage& );
		IActivePlanMessage( const IActivePlanMessage& );

	}; // IActivePlanMessage
}; // TA_Base_App

#endif // IACTIVEPLANMESSAGE_H_INCLUDED
