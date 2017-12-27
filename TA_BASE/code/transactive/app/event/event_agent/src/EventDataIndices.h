/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/EventDataIndices.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __EVENT_DATA_INDICES_H_INCLUDED__
#define __EVENT_DATA_INDICES_H_INCLUDED__

#include "core/event/src/EventItem.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/ordered_index.hpp>

#ifdef BOOST_NO_EXCEPTIONS
void boost::throw_exception( std::exception const & e )
{
	throw e;
}
#endif

/************************* Compare with char* *********************************/
template<class class_T>
struct charptrless : std::binary_function<class_T, class_T, bool> 
{
	bool operator()(const class_T& objX, const class_T& objY) const
	{
		return (strcmp(objX, objY) < 0); 
	}
};

typedef enum 
{
	SORT_EVENT_ID = 0,
	SORT_SEQ_NUMBER,
	SORT_CREATE_TIME
}EVCACHE_SORT;

using namespace boost::multi_index;

typedef multi_index_container<
	TA_Base_Core::EventItem*, 
	indexed_by<
		ordered_unique<BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::EventItem, char*, eventID), charptrless<char*> >,	// by default it sorted by std::less
		ordered_unique<BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::EventItem, TA_Base_Core::EvMgsSequenceNumber, eventSequence)>,	// by default it sorted by std::less
		ordered_non_unique<
			BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::EventItem, TA_Base_Core::MillisecondTimeEx, createTime) ,
			std::less<TA_Base_Core::MillisecondTimeEx>
		>
	>
> EvCacheData;

typedef boost::multi_index::nth_index<EvCacheData, SORT_EVENT_ID>::type		EvCacheData_by_eventID;
typedef boost::multi_index::nth_index<EvCacheData, SORT_SEQ_NUMBER>::type	EvCacheData_by_evSeqNum;
typedef boost::multi_index::nth_index<EvCacheData, SORT_CREATE_TIME>::type	EvCacheData_by_createTime;


#endif //__EVENT_DATA_INDICES_H_INCLUDED__