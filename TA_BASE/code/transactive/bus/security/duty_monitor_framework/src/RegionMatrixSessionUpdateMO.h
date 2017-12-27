// RegionMatrixSessionUpdateMO.h: interface for the RegionMatrixSessionUpdateMO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGIONMATRIXSESSIONUPDATEMO_H__C3F0638D_8A20_494C_8633_8ED726AECE50__INCLUDED_)
#define AFX_REGIONMATRIXSESSIONUPDATEMO_H__C3F0638D_8A20_494C_8633_8ED726AECE50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus 
{
	
	class RegionMatrixSessionUpdateMO :public ACE_Method_Request 
	{
	public:
		RegionMatrixSessionUpdateMO(RegionDutyMatrix& matrix, const std::string& sessionId, bool deleteFlag);
		virtual ~RegionMatrixSessionUpdateMO();
		virtual int call();
	private:
		RegionMatrixSessionUpdateMO& operator=( const RegionMatrixSessionUpdateMO& );
		RegionMatrixSessionUpdateMO( const RegionMatrixSessionUpdateMO& );
	private:
		std::string sessionId_;
		bool deleteFlag_;
		RegionDutyMatrix& matrix_;
	};
}

#endif // !defined(AFX_REGIONMATRIXSESSIONUPDATEMO_H__C3F0638D_8A20_494C_8633_8ED726AECE50__INCLUDED_)
