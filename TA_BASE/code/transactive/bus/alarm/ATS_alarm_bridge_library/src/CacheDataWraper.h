// CacheDataWraper.h: interface for the CacheDataWraper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CACHEDATAWRAPER_H__186B5930_5036_4265_8F0F_BDFAD7EE677C__INCLUDED_)
#define AFX_CACHEDATAWRAPER_H__186B5930_5036_4265_8F0F_BDFAD7EE677C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
    enum CacheDataOperation
	{
		CacheDataInitialCreate=0,
		CacheDataUpdate = 1		
	};
    template<typename item>
	class CacheDataWraper  
	{
	public:
		
		CacheDataWraper()
		{
		   operation=CacheDataUpdate;
		   cacheDatas=NULL;
		}
		virtual ~CacheDataWraper()
		{
			if(cacheDatas !=NULL)
			{
				cacheDatas->clear();
				delete cacheDatas;
				cacheDatas=NULL;
			}
		}
        CacheDataOperation operation;        
		//typedef std::vector< boost::shared_ptr<item> > ICacheDataList;
		std::vector< boost::shared_ptr<item> >*  cacheDatas;
		
		//std::string msgType;
	};
}

#endif // !defined(AFX_CACHEDATAWRAPER_H__186B5930_5036_4265_8F0F_BDFAD7EE677C__INCLUDED_)
