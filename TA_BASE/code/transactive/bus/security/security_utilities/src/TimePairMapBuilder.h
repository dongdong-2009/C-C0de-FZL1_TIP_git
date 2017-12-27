#ifndef TIMEPAIRMAPBUILDER_H_INCLUDED
#define TIMEPAIRMAPBUILDER_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/security_utilities/src/TimePairMapBuilder.h $
  *  @author:  <your name>
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Inserts event data into a TimePairMap.
  */

#if defined(MSC_VER)
#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif

namespace TA_Base_Bus
{
	class TimePairMapBuilder
	{
	public:
		TimePairMapBuilder();

		void getOperators( std::vector<std::string>& operators, const std::string& session, time_t time ) const;

		void sessionBegin ( const std::string& session, const std::string& operatorName, time_t time );
		void overrideBegin( const std::string& session, const std::string& operatorName, time_t time );
		void overrideEnd  ( const std::string& session, time_t time);
		void sessionEnd   ( const std::string& session, time_t time );

	private:

		static const unsigned long MAX_CACHE_SIZE;
		static const float         ATTRITION;

		typedef std::map<TimePair,std::vector<std::string> > TimePairMap;
		typedef std::map<std::string,TimePairMap > SessionMap;

		void begin( const std::string& session, const std::vector<std::string>& operators, time_t time );
		void end  ( const std::string& session, std::vector<std::string>& operators,       time_t time );

		void checkSize();

		SessionMap m_sessionMap;
	};
}

#endif //TIMEPAIRMAPBUILDER_H_INCLUDED
