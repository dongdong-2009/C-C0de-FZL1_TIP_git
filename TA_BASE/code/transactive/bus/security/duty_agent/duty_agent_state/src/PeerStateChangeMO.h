// PeerStateChangeMO.h: interface for the PeerStateChangeMO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PEERSTATECHANGEMO_H__DECA1586_55EB_4F93_B3FB_CEA229F41202__INCLUDED_)
#define AFX_PEERSTATECHANGEMO_H__DECA1586_55EB_4F93_B3FB_CEA229F41202__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus
{
	class PeerStateChangeMO : public ACE_Method_Request 
	{
	public:
		PeerStateChangeMO(DutyState& state);
		virtual ~PeerStateChangeMO();
		virtual int call();

		
	private:
		PeerStateChangeMO& operator=( const PeerStateChangeMO& );
		PeerStateChangeMO( const PeerStateChangeMO& );
		
	private:
		DutyState m_state;
		
	};
}


#endif // !defined(AFX_PEERSTATECHANGEMO_H__DECA1586_55EB_4F93_B3FB_CEA229F41202__INCLUDED_)
