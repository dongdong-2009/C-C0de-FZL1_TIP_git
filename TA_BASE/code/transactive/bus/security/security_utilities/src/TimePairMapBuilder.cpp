/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/security_utilities/src/TimePairMapBuilder.cpp $
  *  @author:  <your name>
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Implements TimePairMapBuilder.
  */

#ifdef _MSC_VER
	#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "SecurityUtilitiesInternal.h"

namespace TA_Base_Bus
{
	const unsigned long TimePairMapBuilder::MAX_CACHE_SIZE(1000);
	const float         TimePairMapBuilder::ATTRITION(0.6F); // the proportion of old sessions that should be removed

	TimePairMapBuilder::TimePairMapBuilder()
	{
	}

	void TimePairMapBuilder::getOperators( std::vector<std::string>& operators, const std::string& session, time_t time ) const
	{
		SessionMap::const_iterator sesIt;

		if( (sesIt = m_sessionMap.find(session)) != m_sessionMap.end() )
		{
			TimePairMap::const_iterator tpIt;

			TimePair tp(time);

			if( (tpIt = sesIt->second.find(tp)) != sesIt->second.end() )
			{
				operators.assign( tpIt->second.begin(), tpIt->second.end() );
			}
		}
	}

	void TimePairMapBuilder::sessionBegin( const std::string& session, const std::string& operatorName, time_t time )
	{
		std::vector<std::string> ops;

		ops.push_back(operatorName);
		begin( session, ops, time );
	}



	void TimePairMapBuilder::overrideBegin( const std::string& session, const std::string& operatorName, time_t time )
	{
		std::vector<std::string> ops;

		end  ( session, ops, time );

		ops.push_back(operatorName);
		begin( session, ops, time );
	}

	void TimePairMapBuilder::overrideEnd( const std::string& session, time_t time )
	{
		std::vector<std::string> ops;

		end  ( session, ops, time );
	
		std::vector<std::string>::iterator secondOp = ops.begin();
		secondOp++;

		ops.erase( secondOp, ops.end() );
		begin( session, ops, time );

	}

	void TimePairMapBuilder::sessionEnd( const std:: string& session, time_t time )
	{
		std::vector<std::string> ops;

		end( session, ops, time );
	}

	void TimePairMapBuilder::begin( const std::string& session, const std::vector<std::string>& operators, time_t time )
	{
		checkSize();

		std::pair<SessionMap::iterator,bool> sesInsertResult;

		TimePairMap emptyTpMap;

		sesInsertResult = m_sessionMap.insert( SessionMap::value_type(session,emptyTpMap) );

		TimePair tp(time);
		
		std::vector<std::string> emptyOperatorVector;

		std::pair<TimePairMap::iterator,bool> tpInsertResult;

		tpInsertResult = sesInsertResult.first->second.insert( TimePairMap::value_type(tp, emptyOperatorVector) );

		TA_ASSERT( true == tpInsertResult.second, "Failed to insert new TimePair" );

		tpInsertResult.first->second.assign(operators.begin(), operators.end());
	}

	void TimePairMapBuilder::end( const std::string& session, std::vector<std::string>& operators, time_t time )
	{
		checkSize();

		SessionMap::iterator sessionMapIt = m_sessionMap.find(session);

		if(sessionMapIt != m_sessionMap.end())
		{

			TimePairMap::iterator tpIt;

			TimePair tp(time);

			tpIt = sessionMapIt->second.find(tp);

			if(  tpIt != sessionMapIt->second.end() )
			{
				// make a copy of the key (can't change it while it is part of the map)
				// set the time
				TimePair newTp(tpIt->first);
				
				newTp.setUpper(time);

				operators.assign( tpIt->second.begin(), tpIt->second.end() );

				// remove the old entry
				sessionMapIt->second.erase( tpIt );

				// insert the new one
				sessionMapIt->second.insert( TimePairMap::value_type(newTp, operators) );
			}
			// if we cannot find the begin time in the map for this session, then an end has occured before a begin
			// so we cannot update an end time so we will return as there is not much else we can do from here
		}
		//same as above, we have recieved an end session for a session that has not been begun yet
	}

	void TimePairMapBuilder::checkSize()
	{
		if( MAX_CACHE_SIZE > m_sessionMap.size() )
		{
			return;
		}

		std::map<time_t,std::string> sessionAge;

		SessionMap::const_iterator smIt;

		// sort sessions by date
		for( smIt=m_sessionMap.begin(); smIt!=m_sessionMap.end(); smIt++ )
		{
			if( !smIt->second.empty() )
			{
				sessionAge.insert( std::map<time_t,std::string>::value_type(smIt->second.begin()->first.getLower(),smIt->first) );
			}
		}

		std::map<time_t,std::string>::const_iterator saIt;

		unsigned long i=0;

		for( saIt=sessionAge.begin(); saIt!=sessionAge.end() && i<(unsigned long)((float)(MAX_CACHE_SIZE)*ATTRITION); saIt++, i++ )
		{
			m_sessionMap.erase(saIt->second);
		}
	}
}
