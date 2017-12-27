#ifndef TIMEPAIR_H_INCLUDED
#define TIMEPAIR_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/security_utilities/src/TimePair.h $
  *  @author:  <your name>
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  The key used in a map of operators associated with a session
  *  at any given time.
  */

// System includes

namespace TA_Base_Bus
{
	class TimePair
	{
	public:
		TimePair();
		TimePair( time_t time );
		TimePair( const TimePair& copyThis );

		void setUpper( time_t upper );

		bool operator< ( const TimePair& rhs ) const;

		time_t getLower() const{ return(m_lower); }

	private:
		time_t m_lower;
		time_t m_upper;
	};
}

#endif //TIMEPAIR_H_INCLUDED
