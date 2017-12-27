// AlarmIndexs.h: interface for the AlarmIndexs class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMINDEXS_H__0A8E9166_91ED_4F2E_99AA_2A00285E4F12__INCLUDED_)
#define AFX_ALARMINDEXS_H__0A8E9166_91ED_4F2E_99AA_2A00285E4F12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#pragma warning(disable:4503)
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include "bus/alarm/shared_alarm/src/SharedAllocator.h"

#include "core/alarm/src/SharedMem.h"

// BOOST_NO_EXCEPTIONS macro user implement for boost_1_39_0
/*
The header <boost/throw_exception.hpp> defines the helper function boost::throw_exception. 
It is intended to be used in Boost libraries that need to throw exceptions, but support configurations 
and platforms where exceptions aren't available, as indicated by the presence of the BOOST_NO_EXCEPTIONS configuration macro. 

When BOOST_NO_EXCEPTIONS is not defined, boost::throw_exception(e) is equivalent to throw e. Otherwise, 
the function is left undefined, and the user is expected to supply an appropriate definition. 
Callers of throw_exception are allowed to assume that the function never returns; therefore, 
if the user-defined throw_exception returns, the behavior is undefined.
*/
namespace boost
{
    void throw_exception( std::exception const & e )
    {
        throw e;
    }
}

/************************* Compare with char* *********************************/
template<class _Ty>
struct charptrless : std::binary_function<_Ty, _Ty, bool> 
{
	bool operator()(const _Ty& _X, const _Ty& _Y) const
	{
		return (stricmp(_X, _Y) < 0); 
	}
};

//CL-20563(sorting) and CL-20969 --luohr 12/10/2009
template<class _Ty>
struct ackbycompare : std::binary_function<_Ty, _Ty, bool> 
{
	bool operator()(const _Ty& _X, const _Ty& _Y) const
	{
		//return (strcmp(_X, _Y) < 0); 
		long lx=strlen(_X);
		long ly=strlen(_Y);
		if (lx==0 && ly==0) return false;
		if (lx==0) return true;			
		return false;
	}
};


template<class _Ty>
struct alwaysfalse : std::binary_function<_Ty, _Ty, bool> 
{
	bool operator()(const _Ty& _X, const _Ty& _Y) const
	{
		return false; 
	}
};
/*****************************************************************************/

/*************************** All Alarm **********************************/
using namespace boost::multi_index;

/**************************	Index members ****************************/
struct severityIndex:  boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, unsigned long, alarmSeverity),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, TA_Base_Core::ItemMillisecondTime, transactiveTime)
>{};

struct defaultIndex:  boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, char*, alarmAcknowledgedBy), //need verify
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, unsigned short, state), 
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, unsigned long, alarmSeverity),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, TA_Base_Core::ItemMillisecondTime, transactiveTime)
>{};

struct timeIndex:  boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, TA_Base_Core::ItemMillisecondTime, transactiveTime),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, unsigned long, alarmSeverity)
>{};

struct assetIndex:  boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, char*, assetName),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, TA_Base_Core::ItemMillisecondTime, transactiveTime)
>{};

struct descriptionIndex:  boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, char*, alarmDescription),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, TA_Base_Core::ItemMillisecondTime, transactiveTime)
>{};

struct ackIndex:  boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, char*, alarmAcknowledgedBy),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, TA_Base_Core::ItemMillisecondTime, transactiveTime)
>{};

struct commentIndex:  boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, char*, alarmComments),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, TA_Base_Core::ItemMillisecondTime, transactiveTime)
>{};

struct dssIndex:  boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, char*, decisionSupportState),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, TA_Base_Core::ItemMillisecondTime, transactiveTime)
>{};

struct mmsIndex:  boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, char*, mmsState),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, TA_Base_Core::ItemMillisecondTime, transactiveTime)
>{};

struct valueIndex:  boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, char*, alarmValue),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, TA_Base_Core::ItemMillisecondTime, transactiveTime)
>{};

struct avalancheIndex:  boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, char*, avalanche),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, TA_Base_Core::ItemMillisecondTime, transactiveTime)
>{};

struct omTypeIndex:  boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, char*, alarmOperatorMode),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, TA_Base_Core::ItemMillisecondTime, transactiveTime)
>{};

struct strAlarmTypeIndex:  boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, char*, strAlarmType),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, TA_Base_Core::ItemMillisecondTime, transactiveTime)
>{};
struct subsytemTypeIndex:  boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, char*, subsytemType),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, TA_Base_Core::ItemMillisecondTime, transactiveTime)
>{};
struct systemkeyTypeIndex:  boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, char*, systemType),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, TA_Base_Core::ItemMillisecondTime, transactiveTime)
>{};

//++ noel
// added for automatic retrieval of the highest alarm severity of unacknowledged alarm
struct severityUnAckIndex:  boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, char*, alarmAcknowledgedBy),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, unsigned long, alarmSeverity),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, TA_Base_Core::ItemMillisecondTime, transactiveTime)
>{};

//++ Noel
// added for duty change fix
struct locationIndex: boost::multi_index::composite_key<
	TA_Base_Core::AlarmItem*,
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, unsigned long, locationKey),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, unsigned long, dataSource),
	BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, unsigned long, subsystemKey)

>{};


/*********************************************************************/
//HP+-, modification
/*************************** Alarm Index **********************************/
struct alarmIndices:boost::multi_index::indexed_by<
	//	NOSORT_BY_HASH
	boost::multi_index::ordered_unique< BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, char*, alarmID), charptrless<char*>
	>,

	//	NOSORT_BY_SUBSYSTEM
	boost::multi_index::ordered_non_unique<BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, unsigned long, subsystemKey)
	>,

	// 	SORT_BY_DEFAULT
	// liuyang++ change default sort order of time column
	//boost::multi_index::ordered_non_unique< defaultIndex, boost::multi_index::composite_key_compare<charptrless<char*>, std::greater<unsigned short>, std::less<unsigned short>, std::less<TA_Base_Core::ItemMillisecondTime> >
	boost::multi_index::ordered_non_unique< defaultIndex, boost::multi_index::composite_key_compare<ackbycompare<char*>, std::greater<unsigned short>, std::less<unsigned short>, std::greater<TA_Base_Core::ItemMillisecondTime> >
	// ++liuyang 
	>,		

	//	SORT_BY_DATETIME
	boost::multi_index::ordered_non_unique< timeIndex
#if defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
	,boost::multi_index::composite_key_result_less<timeIndex::result_type>
#endif
	>,	

	//	SORT_BY_SEVERITY
	boost::multi_index::ordered_non_unique< 
	severityIndex
#if defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
	,boost::multi_index::composite_key_result_less<severityIndex::result_type>
#endif
	>,

	//	SORT_BY_ASSET
	boost::multi_index::ordered_non_unique< assetIndex, boost::multi_index::composite_key_compare<
	charptrless<char*>,
	std::less<TA_Base_Core::ItemMillisecondTime>
	>
	>,							
	//	SORT_BY_DESCRIPTION
	boost::multi_index::ordered_non_unique< descriptionIndex, boost::multi_index::composite_key_compare<
	charptrless<char*>,
	std::less<TA_Base_Core::ItemMillisecondTime>
	>
	>,		

	//	SORT_BY_VALUE
	boost::multi_index::ordered_non_unique< valueIndex, boost::multi_index::composite_key_compare<
	charptrless<char*>,
	std::less<TA_Base_Core::ItemMillisecondTime>
	>
	>,							
	// 	SORT_BY_MMS
	boost::multi_index::ordered_non_unique< mmsIndex, boost::multi_index::composite_key_compare<
	charptrless<char*>,
	std::less<TA_Base_Core::ItemMillisecondTime>
	>
	>,							
	// 	SORT_BY_DSS
	boost::multi_index::ordered_non_unique< dssIndex, boost::multi_index::composite_key_compare<
	charptrless<char*>,
	std::less<TA_Base_Core::ItemMillisecondTime>
	>
	>,							
	// 	SORT_BY_AVALANCHE
	boost::multi_index::ordered_non_unique< avalancheIndex, boost::multi_index::composite_key_compare<
	charptrless<char*>,
	std::less<TA_Base_Core::ItemMillisecondTime>
	>
	>,										
	// 	SORT_BY_ACK
	boost::multi_index::ordered_non_unique< ackIndex, boost::multi_index::composite_key_compare<
	charptrless<char*>,
	std::less<TA_Base_Core::ItemMillisecondTime>
	>
	>,										
	// 	SORT_BY_COMMENTS
	boost::multi_index::ordered_non_unique< commentIndex, boost::multi_index::composite_key_compare<
	charptrless<char*>,
	std::less<TA_Base_Core::ItemMillisecondTime>
	>
	>,
	// 	SORT_BY_ALARMOPERATORTYPE
	boost::multi_index::ordered_non_unique< omTypeIndex, boost::multi_index::composite_key_compare<
	charptrless<char*>,
	std::less<TA_Base_Core::ItemMillisecondTime>
	>
	>,
	// 	SORT_BY_STRALARMTYPE
	boost::multi_index::ordered_non_unique< strAlarmTypeIndex, boost::multi_index::composite_key_compare<
	charptrless<char*>,
	std::less<TA_Base_Core::ItemMillisecondTime>
	>
	>,
	// 	SORT_BY_SUBSYSTEMTYPE
	boost::multi_index::ordered_non_unique< subsytemTypeIndex, boost::multi_index::composite_key_compare<
	charptrless<char*>,
	std::less<TA_Base_Core::ItemMillisecondTime>
	>
	>,
	// 	SORT_BY_SYSTEMKEYTYPE
	boost::multi_index::ordered_non_unique< systemkeyTypeIndex, boost::multi_index::composite_key_compare<
	charptrless<char*>,
	std::less<TA_Base_Core::ItemMillisecondTime>
	>
	>,
	// SORT_BY_LOCATION
	boost::multi_index::ordered_non_unique< locationIndex
#if defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
	,boost::multi_index::composite_key_result_less<locationIndex::result_type>
#endif
	>,
	// SORT_BY_ATSFLAG
	boost::multi_index::ordered_non_unique<BOOST_MULTI_INDEX_MEMBER(TA_Base_Core::AlarmItem, bool, isExternalAlarm)
	>,

	// SORT_BY_SEVERITYUNACK
	boost::multi_index::ordered_non_unique<severityUnAckIndex, boost::multi_index::composite_key_compare<
	charptrless<char*>,
	std::less<unsigned short>,
	std::less<TA_Base_Core::ItemMillisecondTime>
	>
	>
>{};


typedef multi_index_container<
TA_Base_Core::AlarmItem*,
alarmIndices,
fixed_managed_winShmem::allocator<TA_Base_Core::AlarmItem*>::type
> AlarmIndex;

typedef boost::multi_index::nth_index<AlarmIndex,NOSORT_BY_HASH>::type		alarmindex_by_id;

typedef boost::multi_index::nth_index<AlarmIndex,NOSORT_BY_SUBSYSTEM>::type	alarmindex_by_subsystem;

typedef boost::multi_index::nth_index<AlarmIndex,SORT_BY_DEFAULT>::type		alarmindex_by_default;

typedef boost::multi_index::nth_index<AlarmIndex,SORT_BY_DATETIME>::type		alarmindex_by_time;

typedef boost::multi_index::nth_index<AlarmIndex,SORT_BY_SEVERITY>::type		alarmindex_by_severity;

typedef boost::multi_index::nth_index<AlarmIndex,SORT_BY_ASSET>::type			alarmindex_by_asset;

typedef boost::multi_index::nth_index<AlarmIndex,SORT_BY_DESCRIPTION>::type		alarmindex_by_description;

typedef boost::multi_index::nth_index<AlarmIndex,SORT_BY_VALUE>::type			alarmindex_by_value;

typedef boost::multi_index::nth_index<AlarmIndex,SORT_BY_MMS>::type				alarmindex_by_mms;

typedef boost::multi_index::nth_index<AlarmIndex,SORT_BY_DSS>::type				alarmindex_by_dss;

typedef boost::multi_index::nth_index<AlarmIndex,SORT_BY_AVALANCHE>::type		alarmindex_by_avalanche;

typedef boost::multi_index::nth_index<AlarmIndex,SORT_BY_ACK>::type				alarmindex_by_ack;

typedef boost::multi_index::nth_index<AlarmIndex,SORT_BY_COMMENTS>::type		alarmindex_by_comment;

typedef boost::multi_index::nth_index<AlarmIndex,SORT_BY_ALARMOPERATORTYPE>::type alarmindex_by_omtype;

typedef boost::multi_index::nth_index<AlarmIndex,SORT_BY_STRALARMTYPE>::type	alarmindex_by_strAlarmType;

typedef boost::multi_index::nth_index<AlarmIndex,SORT_BY_SUBSYSTEMTYPE>::type	alarmindex_by_subsytemType;

typedef boost::multi_index::nth_index<AlarmIndex,SORT_BY_SYSTEMKEYTYPE>::type	alarmindex_by_systemkeyType;

typedef boost::multi_index::nth_index<AlarmIndex,SORT_BY_LOCATION>::type		alarmindex_by_location;

typedef boost::multi_index::nth_index<AlarmIndex, SORT_BY_ALARMEXTFLAG>::type		alarmindex_by_atsFlag;

typedef boost::multi_index::nth_index<AlarmIndex, SORT_BY_SEVERITYUNACK>::type	alarmindex_by_serverityUnAck;
/************************************************************************/

typedef std::pair<alarmindex_by_location::iterator, alarmindex_by_location::iterator> T_indexAlarmLocRange; 

#endif // !defined(AFX_ALARMINDEXS_H__0A8E9166_91ED_4F2E_99AA_2A00285E4F12__INCLUDED_)
