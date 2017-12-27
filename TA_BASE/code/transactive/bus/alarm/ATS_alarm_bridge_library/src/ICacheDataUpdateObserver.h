// ICacheDataUpdateObserver.h: interface for the ICacheDataUpdateObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICACHEDATAUPDATEOBSERVER_H__91E6FBD6_E8B6_44AF_8943_B7FEFF5E0D54__INCLUDED_)
#define AFX_ICACHEDATAUPDATEOBSERVER_H__91E6FBD6_E8B6_44AF_8943_B7FEFF5E0D54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CacheDataWraper.h"
#include "boost/shared_ptr.hpp"

namespace TA_Base_Bus
{
	template <typename t>
	class ICacheDataUpdateObserver  
	{
	public:
		ICacheDataUpdateObserver()
		{

		}
		virtual ~ICacheDataUpdateObserver()
		{

		}
		virtual void processData(boost::shared_ptr<CacheDataWraper <t> > datas) = 0;		 
	};
}

#endif // !defined(AFX_ICACHEDATAUPDATEOBSERVER_H__91E6FBD6_E8B6_44AF_8943_B7FEFF5E0D54__INCLUDED_)
