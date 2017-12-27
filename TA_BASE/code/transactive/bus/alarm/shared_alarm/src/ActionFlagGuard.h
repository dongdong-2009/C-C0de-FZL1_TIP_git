// ActionFlagGuard.h: interface for the ActionFlagGuard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIONFLAGGUARD_H__67CD5071_7B61_4010_83AD_4D429500CFB6__INCLUDED_)
#define AFX_ACTIONFLAGGUARD_H__67CD5071_7B61_4010_83AD_4D429500CFB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
namespace TA_Base_Bus
{
	class ActionFlagGuard  
	{
	public:
		ActionFlagGuard(bool& actionFlag);
		virtual ~ActionFlagGuard();
	private:
		bool& m_actionFlag;

	};
}
#endif // !defined(AFX_ACTIONFLAGGUARD_H__67CD5071_7B61_4010_83AD_4D429500CFB6__INCLUDED_)
