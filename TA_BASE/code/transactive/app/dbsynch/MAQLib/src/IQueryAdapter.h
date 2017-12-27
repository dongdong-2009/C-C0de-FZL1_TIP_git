//////////////////////////////////////////////////////////////////////
///		@file		IQueryAdapter.h
///		@author		zhilin,ouyang
///		@date		2012-01-12 10:44:41
///
///		@brief	    Define an interface for database query operations. 
///					whenever add a new database server should derive  
///                 from this interface and implement its operations
//////////////////////////////////////////////////////////////////////
#ifndef IQUERYADAPTER_H
#define IQUERYADAPTER_H
#include "DbCommonData.h"

NS_BEGIN(TA_AQ_Lib)

class IQueryAdapter
{
public:
	virtual bool hasMore() = 0;
		
	virtual void fetchInitial( int numColumns, Buffer& data, long lMaxRows ) = 0;
		
	virtual void fetchMore( int numColumns, Buffer& data, long lMaxRows ) = 0;
		
	virtual bool cleanQuery() = 0;
		
	virtual bool isActive() = 0;

	virtual ~IQueryAdapter() {};

};  

NS_END(TA_AQ_Lib) 

#endif  /// IQUERYADAPTER_H





























