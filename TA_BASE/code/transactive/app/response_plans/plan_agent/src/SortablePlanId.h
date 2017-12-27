#ifndef SORTABLEPLANID_H_INCLUDED
#define SORTABLEPLANID_H_INCLUDED

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
  *  A thread-safe Singleton factory for converting database data into
  *  application data and vice-versa.
  */

namespace TA_Base_Core
{
    struct ActivePlanId;
}

namespace TA_Base_App
{
	class SortablePlanId
	{
	public:
		SortablePlanId();
		SortablePlanId( const SortablePlanId& );
		SortablePlanId(const TA_Base_Core::ActivePlanId& plan);
		bool operator<( const SortablePlanId& rhs ) const;

        void set( const TA_Base_Core::ActivePlanId& newPlan );
        void setPlan( const unsigned long plan );
        void setInstance( const unsigned long instance );

        TA_Base_Core::ActivePlanId get( void ) const;
        const unsigned long getPlan( void ) const;
        const unsigned long getInstance( void ) const;

	private:
        SortablePlanId& operator=( const SortablePlanId& );

		TA_Base_Core::ActivePlanId m_plan;
	};

} // TA_Base_App

#endif //SORTABLEPLANID_H_INCLUDED
