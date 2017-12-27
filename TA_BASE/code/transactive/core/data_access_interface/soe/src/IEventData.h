#ifndef IEVENTDATA_INCLUDED
#define IEVENTDATA_INCLUDED

#include <sys/timeb.h>
#include <string>

namespace TA_Base_Core
{
	// interface for event ( SOE )
	class IEventData
	{
	public:
		IEventData(){}
		virtual ~IEventData(){}

		// interface methods list
		virtual unsigned long getPKey() const = 0;
		virtual void setPKey( const unsigned long key ) = 0;

		virtual timeb getSourceTime() const = 0;
		virtual void setSourceTime( const timeb& t ) = 0;
	};


}	// namespace TA_Base_Core





#endif	// #ifndef IEVENTDATA_INCLUDED
