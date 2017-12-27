// RegionMatrixDutyUpdateMO.h: interface for the RegionMatrixDutyUpdateMO class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGIONMATRIXDUTYUPDATEMO_H__992507F5_E762_49CA_B08E_7FB201BBACDA__INCLUDED_)
#define AFX_REGIONMATRIXDUTYUPDATEMO_H__992507F5_E762_49CA_B08E_7FB201BBACDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus
{
	class RegionDutyMatrix;
	class RegionMatrixDutyUpdateMO :public ACE_Method_Request 
	{
	public:
		RegionMatrixDutyUpdateMO(RegionDutyMatrix& matrix_,
				DutyAgentTypeCorbaDef::RegionDutySequence& regionDutySeq,
				EDutyUpdateTupe updateType);
		virtual ~RegionMatrixDutyUpdateMO();
		virtual int call();
	private:
		RegionMatrixDutyUpdateMO& operator=( const RegionMatrixDutyUpdateMO& );
		RegionMatrixDutyUpdateMO( const RegionMatrixDutyUpdateMO& );

	private:
		RegionDutyMatrix& matrix_;
		DutyAgentTypeCorbaDef::RegionDutySequence duty_;
		EDutyUpdateTupe updateType_;
	};
}

#endif // !defined(AFX_REGIONMATRIXDUTYUPDATEMO_H__992507F5_E762_49CA_B08E_7FB201BBACDA__INCLUDED_)
