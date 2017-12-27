// IStatusStrategy.h: interface for the IStatusStrategy class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISTATUSSTRATEGY_H__639A55F9_FBAB_4876_B6B2_49917EB0305C__INCLUDED_)
#define AFX_ISTATUSSTRATEGY_H__639A55F9_FBAB_4876_B6B2_49917EB0305C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class StatusChecker;

class IStatusStrategy  
{
public:
	IStatusStrategy();
	virtual ~IStatusStrategy();
public:
	void detectStatusChanged(StatusChecker * statusChecker);
};

#endif // !defined(AFX_ISTATUSSTRATEGY_H__639A55F9_FBAB_4876_B6B2_49917EB0305C__INCLUDED_)
