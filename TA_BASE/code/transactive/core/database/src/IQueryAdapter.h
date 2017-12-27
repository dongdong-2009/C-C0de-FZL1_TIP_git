//////////////////////////////////////////////////////////////////////
///		@file		IQueryAdapter.h
///		@author		zhilin,ouyang
///		@date		2010-11-24 10:44:41
///
///		@brief	    Define an interface for database query operations. 
///					whenever add a new database server should derive  
///                 from this interface and implement its operations
//////////////////////////////////////////////////////////////////////
#ifndef IQUERYADAPTER_H
#define IQUERYADAPTER_H
#include "core/database/src/CommonType.h"

NS_BEGIN(TA_Base_Core)

class IQueryAdapter
{
public:
	virtual bool hasMore() = 0;
		
	virtual void fetchInitial( int numColumns, Buffer& data, long lMaxRows ) = 0;
		
	virtual void fetchMore( int numColumns, Buffer& data, long lMaxRows ) = 0;
		
	virtual bool cleanQuery() = 0;
		
	virtual bool isActive( unsigned long ulMaxTimeoutInSecs ) = 0;

	virtual ~IQueryAdapter() {};

};  // end of class IQueryAdapter

NS_END(TA_Base_Core)  // end of namespace TA_Base_Core

#endif  /// IQUERYADAPTER_H